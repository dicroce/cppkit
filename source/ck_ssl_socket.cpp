
#include "cppkit/ck_ssl_socket.h"
#include "openssl/err.h"
#include "openssl/crypto.h"
#include "openssl/x509v3.h"
#include <thread>

#ifdef IS_POSIX
  #include <fcntl.h>
  #include <signal.h>
#else
  #include <Wincrypt.h>
  #undef X509_NAME
  #undef X509_EXTENSIONS
#endif

using namespace cppkit;
using namespace std;

static const int XSSL_ERROR_CRITICAL = -2;
static const int XSSL_ERROR_NONE = 0;
static const int XSSL_ERROR_RETRY = 1;

recursive_mutex* ck_ssl_socket::static_init::_mutexes;
recursive_mutex ck_ssl_socket::static_init::_initLok;
bool ck_ssl_socket::static_init::_init(false);
static vector<X509*> windowsCerts;

recursive_mutex ck_ssl_socket::_storeLok;
bool ck_ssl_socket::_certStoreInitialized = false;

void ck_ssl_socket::static_init::lock(int mode, int n, const char* file, int line)
{
    if( mode & CRYPTO_LOCK )
        _mutexes[n].lock();
    else _mutexes[n].unlock();
}

size_t ck_ssl_socket::static_init::id()
{
    return std::hash<std::thread::id>()(std::this_thread::get_id());
}

struct CRYPTO_dynlock_value* ck_ssl_socket::static_init::dynlock_create(const char* file, int line)
{
    return new CRYPTO_dynlock_value;
}

void ck_ssl_socket::static_init::dynlock(int mode, struct CRYPTO_dynlock_value* lock, const char* file, int line)
{
    if( lock != NULL )
    {
        if( mode & CRYPTO_LOCK )
            lock->_mutex.lock();
        else
            lock->_mutex.unlock();
    }
}

void ck_ssl_socket::static_init::dynlock_destroy(struct CRYPTO_dynlock_value* lock, const char* file, int line)
{
    delete lock;
}

ck_ssl_socket::static_init::static_init()
{
    unique_lock<recursive_mutex>(_initLok);

    if( ! _init )
    {
        _init = true;

        SSL_library_init();
        SSL_load_error_strings();
        ERR_load_BIO_strings();

        int mutexCount = CRYPTO_num_locks();
        _mutexes = new recursive_mutex[mutexCount];

        CRYPTO_set_locking_callback(&ck_ssl_socket::static_init::lock);

        //TESTING BELOW BUT IS DEPRECATED IN FAVOR OF
        //CRYPTO_THREADID_set_callback(&ck_ssl_socket::StaticInit::THREADID);
        //I don't think we need this as OpenSSL default implementation will be used if we
        //don't set this.
        //CRYPTO_set_id_callback(&ck_ssl_socket::static_init::id);

        CRYPTO_set_dynlock_create_callback(&ck_ssl_socket::static_init::dynlock_create);
        CRYPTO_set_dynlock_lock_callback(&ck_ssl_socket::static_init::dynlock);
        CRYPTO_set_dynlock_destroy_callback(&ck_ssl_socket::static_init::dynlock_destroy);
    }
}

ck_ssl_socket::static_init::~static_init()
{
    unique_lock<recursive_mutex> guard(_initLok);

    if( _init )
    {
        _init = false;
        ERR_free_strings();
        CRYPTO_set_locking_callback(NULL);
        delete [] _mutexes;

#ifdef IS_WINDOWS
        for( auto cert : windowsCerts )
            X509_free( cert );
        windowsCerts.clear();
#endif
    }
}

const ck_ssl_socket::static_init _sslStaticInit;

ck_ssl_socket::ck_ssl_socket() :
    _ctx( NULL ),
    _ssl( NULL ),
    _sok(),
    _hostName(),
    _verifyConnection(false),
    _privateKeyPassword(),
    _bindAddress(),
    _bindPort(0),
    _caCertificateFilePath(),
    _pemCertificateChainFilePath(),
    _pemCertificateFilePath(),
    _pemRSAPrivateKeyFilePath(),
    _sslLok()
{
    ck_raw_socket::socket_startup();

    _ctx = SSL_CTX_new( SSLv23_method() );
    SSL_CTX_set_options( _ctx, SSL_OP_NO_SSLv2 );
    SSL_CTX_set_options( _ctx, SSL_OP_NO_SSLv3 );
    SSL_CTX_set_default_passwd_cb( _ctx, _private_key_password_cb );
    SSL_CTX_set_default_passwd_cb_userdata( _ctx, this );

    // Setting cipher list based on the AWS ELB 2015-05
    // http://docs.aws.amazon.com/ElasticLoadBalancing/latest/DeveloperGuide/elb-security-policy-table.html
    // Removing HIGH:MEDIUM because of logjam attack on DHE EDH
    SSL_CTX_set_cipher_list(_ctx, "ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:AES128-GCM-SHA256:AES128-SHA256:AES128-SHA:AES256-GCM-SHA384:AES256-SHA256:AES256-SHA:DES-CBC3-SHA");
    SSL_CTX_set_session_cache_mode(_ctx, SSL_SESS_CACHE_OFF);
    SSL_CTX_set_mode(_ctx, SSL_MODE_AUTO_RETRY);
    SSL_CTX_set_default_verify_paths( _ctx );

    //const char * openSslVersion = SSLeay_version(SSLEAY_VERSION);
    // if openSslVersion >= 1.0.2 set to use ECDHE
    //SSL_CTX_set_ecdh_auto(_ctx, 1);
}

ck_ssl_socket::ck_ssl_socket( ck_ssl_socket&& obj ) throw()
{
    unique_lock<recursive_mutex> guard( _sslLok );

    unique_lock<recursive_mutex> objGuard( obj._sslLok );

    _ctx = obj._ctx;
    obj._ctx = NULL;

    _ssl = obj._ssl;
    obj._ssl = NULL;

    _sok = std::move(obj._sok);

    _hostName = std::move(obj._hostName);

    _verifyConnection = obj._verifyConnection;

    _privateKeyPassword = std::move(obj._privateKeyPassword);

    _bindAddress = std::move(obj._bindAddress);

    _bindPort = obj._bindPort;

    _caCertificateFilePath = std::move(obj._caCertificateFilePath);
    _pemCertificateChainFilePath = std::move(obj._pemCertificateChainFilePath);
    _pemCertificateFilePath = std::move(obj._pemCertificateFilePath);
    _pemRSAPrivateKeyFilePath = std::move(obj._pemRSAPrivateKeyFilePath);
}

ck_ssl_socket::~ck_ssl_socket() throw()
{
    unique_lock<recursive_mutex> guard( _sslLok );

    close();

    if( _ctx )
        SSL_CTX_free( _ctx );
}

ck_ssl_socket& ck_ssl_socket::operator = ( ck_ssl_socket&& obj ) throw()
{
    unique_lock<recursive_mutex> guard( _sslLok );

    unique_lock<recursive_mutex> objGuard( obj._sslLok );

    _ctx = obj._ctx;
    obj._ctx = NULL;

    _ssl = obj._ssl;
    obj._ssl = NULL;

    _sok = std::move(obj._sok);

    _hostName = std::move(obj._hostName);

    _verifyConnection = obj._verifyConnection;

    _privateKeyPassword = std::move(obj._privateKeyPassword);

    _bindAddress = std::move(obj._bindAddress);

    _bindPort = obj._bindPort;

    _caCertificateFilePath = std::move(obj._caCertificateFilePath);
    _pemCertificateChainFilePath = std::move(obj._pemCertificateChainFilePath);
    _pemCertificateFilePath = std::move(obj._pemCertificateFilePath);
    _pemRSAPrivateKeyFilePath = std::move(obj._pemRSAPrivateKeyFilePath);

    return *this;
}

void ck_ssl_socket::verify_connection( bool verify )
{
    unique_lock<recursive_mutex> g( ck_ssl_socket::_storeLok );

    _verifyConnection = verify;

#ifdef IS_WINDOWS

    if( !ck_ssl_socket::_certStoreInitialized )
    {
        // walk windows cert store and save certs into global static
        HCERTSTORE store = CertOpenSystemStore(NULL, L"ROOT");

        for( auto certCtx = CertEnumCertificatesInStore(store, NULL);
             certCtx != NULL;
             certCtx = CertEnumCertificatesInStore(store, certCtx) )
        {
            if(certCtx->dwCertEncodingType & X509_ASN_ENCODING)
            {


                X509* myX509 = NULL;
                const unsigned char* buffer=certCtx->pbCertEncoded; 
                int len=certCtx->cbCertEncoded; 
                myX509 = d2i_X509(NULL,&buffer,len);
                if( myX509 )
                    windowsCerts.push_back( myX509 );
            }
        }

        CertCloseStore(store,0);

        ck_ssl_socket::_certStoreInitialized = true;
    }
    // Every ck_ssl_socket has its own _ctx, so we need to add the globally discovered X509's to the current
    // cert store here. This must occur before Connect(), so this is the perfect place for this.
    auto store = SSL_CTX_get_cert_store( _ctx );
    for( auto& cert : windowsCerts )
        X509_STORE_add_cert( store, cert );
#endif
}

void ck_ssl_socket::create( int af )
{
    _sok.create( af );
}

void ck_ssl_socket::connect( const ck_string& host, int port )
{
    unique_lock<recursive_mutex> guard( _sslLok );

    if( _sok.valid() )
        _sok.close();

    if( _ssl )
        close();

    _ssl = SSL_new( _ctx );
    if( !_ssl )
        CK_STHROW( ck_socket_exception, ("Unable to allocate SSL object.") );

    ck_string ip = host;
    if( ck_socket_address::is_hostname( host ) )
    {
        vector<ck_string> addresses = ck_resolve( ck_socket_address::get_address_family(host), host );
        if( addresses.size() == 0 )
            CK_STHROW( ck_socket_exception, ( "Unable to resolve hostname." ));

        ip = addresses[0];
        // If someone called SetHost(), use that value instead
        if( !_hostName.empty() )
            SSL_set_tlsext_host_name( _ssl, _hostName.c_str() );
        else
            SSL_set_tlsext_host_name( _ssl, host.c_str() );
    }

    _sok.connect( ip, port );

    if( _hostName.empty() )
        _hostName = host;

#ifdef IS_WINDOWS
    ULONG mode = 1;
    ioctlsocket( _sok.get_sok_id(), FIONBIO, &mode);
#else
    int flags = fcntl( _sok.get_sok_id(), F_GETFL, 0);
    fcntl( _sok.get_sok_id(), F_SETFL, flags | O_NONBLOCK );
#endif

    int fd = (int)_sok.get_sok_id();
    SSL_set_fd(_ssl, fd);

#ifndef IS_WINDOWS
    signal( SIGPIPE, SIG_IGN );
#endif

    bool verified = false;

    //If someone has asked to verify, then we must SSL_connect in order
    //to retreive the X509 cert.  Otherwise we do a passive connect
    if( _verifyConnection )
    {
        //Do handshake here to get X509
        int error = -1;
        int result = 0;

        // XXX - used to use member called connectTimeoutMillis
        uint64_t waitTime = 10000;
        do
        {
            ERR_clear_error();
            result = SSL_connect( _ssl );
            error = _handle_error( result, waitTime );
        }
        while( waitTime > 0 && error > XSSL_ERROR_NONE );

        if( result == 1 && error == XSSL_ERROR_NONE )
        {
            X509 *cert = SSL_get_peer_certificate( _ssl );
            if( cert != NULL )
            {
                if( SSL_get_verify_result( _ssl ) == X509_V_OK )
                {
                    int check = _x509_check_host(cert, _hostName);
                    //If check is 1, the certificate matched the hostname
                    verified = (check == 1);

                    if( ! verified )
                        CK_LOG_ERROR("WARNING: Possible man-in-the-middle attack detected.  The hostname specified %s does not match the hostname in the server's X509 certificate.", _hostName.c_str() );
                }
                else
                {
                    CK_LOG_ERROR("Unable to verify the ssl certificate presented by: %s", _hostName.c_str() );
                }

                X509_free( cert );
            }
            else CK_LOG_ERROR("Verification was requested but the server didn't return an X509 Certificate.");

            if( !verified )
                CK_STHROW( ck_ssl_socket_auth_exception, ("Unable to verify the ssl certificate presented by: %s", _hostName.c_str()));
        }

        return;
    }

    // This call lets OpenSSL know this socket will be used as a client. It also
    // enable auto renegotiation when doing io.
    SSL_set_connect_state( _ssl );
}

void ck_ssl_socket::listen( int backlog )
{
    _sok.listen( backlog );
}

void ck_ssl_socket::bind( int port, const ck_string& ip )
{
    unique_lock<recursive_mutex> guard(_sslLok);

    _ssl = SSL_new(_ctx);

    // First, cache the information passed.
    _bindPort = port;
    _bindAddress = ip;

    _sok.bind( port, ip );

    SSL_set_fd( _ssl, (int)_sok.get_sok_id() );
}

ck_ssl_socket ck_ssl_socket::accept()
{
    unique_lock<recursive_mutex> guard(_sslLok);

    ck_ssl_socket sok;
    ck_socket regSok = _sok.accept();

    sok._sok = std::move( regSok );
    sok._ssl = SSL_new( _ctx );

#ifdef IS_WINDOWS
    ULONG mode = 1;
    ioctlsocket( sok._sok.get_sok_id(), FIONBIO, &mode);
#else
    int flags = fcntl( sok._sok.get_sok_id(), F_GETFL, 0);
    fcntl( sok._sok.get_sok_id(), F_SETFL, flags | O_NONBLOCK );
#endif

    SSL_set_fd( sok._ssl, (int)sok._sok.get_sok_id() );

    // This call lets openssl know this socket is to be used by a server.
    // It also enable rengotion on io.
    SSL_set_accept_state( sok._ssl );

    SSL_accept( sok._ssl );

    return std::move(sok);
}

int ck_ssl_socket::raw_send( const void* buf, size_t len )
{
    if( _ssl == NULL )
        CK_STHROW( ck_socket_exception, ("Invalid _ssl object.") );

    if( !buf || (len <= 0) )
        CK_THROW(( "Invalid argument passed to ck_ssl_socket::raw_send()."));

    int bytesJustWritten = 0;
    int error = -1;

    unique_lock<recursive_mutex> guard( _sslLok );

    do
    {
        if( !_sok.valid() )
            return -1;

        ERR_clear_error();
        bytesJustWritten = SSL_write( _ssl, buf, (int)len );
        uint64_t waitTimeMillis = 5000;
        error = _handle_error( bytesJustWritten, waitTimeMillis );
    } while( error > XSSL_ERROR_NONE );

    return bytesJustWritten;
}

int ck_ssl_socket::raw_recv( void* buf, size_t len )
{
    if( _ssl == NULL )
        CK_STHROW( ck_socket_exception, ("Invalid _ssl object.") );

    if( !buf || (len <= 0) )
        CK_THROW(( "Invalid argument passed to ck_ssl_socket::raw_recv()."));

    int bytesJustReceived = 0;
    int error = -1;

    unique_lock<recursive_mutex> guard( _sslLok );

    do
    {
        if( !_sok.valid() )
            return -1;

        ERR_clear_error();
        bytesJustReceived = SSL_read( _ssl, buf, (int)len );
        uint64_t waitTimeMillis = 5000;
        error = _handle_error( bytesJustReceived, waitTimeMillis );
    } while( error > XSSL_ERROR_NONE );

    return bytesJustReceived;
}

void ck_ssl_socket::send( const void* buf, size_t len )
{
    if( _ssl == NULL )
        CK_STHROW( ck_socket_exception, ("Invalid _ssl object.") );

    if( !buf || (len <= 0) )
        CK_THROW(( "Invalid argument passed to ck_ssl_socket::send()."));

    uint64_t waitTimeMillis = 5000;

    size_t bytesToWrite = len;
    const char* writer = (char*)buf;

    while( _sok.valid() && (bytesToWrite > 0) )
    {
        int bytesJustWritten = 0;
        int error = -1;

        unique_lock<recursive_mutex> guard( _sslLok );

        do
        {
            ERR_clear_error();
            bytesJustWritten = SSL_write( _ssl, writer, (int)bytesToWrite );
            error = _handle_error( bytesJustWritten, waitTimeMillis );
        } while( _sok.valid() && waitTimeMillis > 0 && error > XSSL_ERROR_NONE );

        if( bytesJustWritten > 0 )
        {
            bytesToWrite -= bytesJustWritten;
            writer += bytesJustWritten;
        }
        else if( error == XSSL_ERROR_CRITICAL )
            CK_STHROW( ck_socket_exception, ("Critical error encountered with SSL socket io.") );
    }

    if( bytesToWrite > 0 )
        CK_STHROW( ck_socket_exception, ("Incomplete IO.") );
}

void ck_ssl_socket::recv( void* buf, size_t len )
{
    if( _ssl == NULL )
        CK_STHROW( ck_socket_exception, ("Invalid _ssl object.") );

    if( !buf || (len <= 0) )
        CK_THROW(( "Invalid argument passed to ck_ssl_socket::Recv()."));

    uint64_t waitTimeMillis = 5000;

    int bytesToRecv = len;
    char* writer = (char*)buf;

    while( _sok.valid() && (bytesToRecv > 0) )
    {
        int bytesJustReceived = 0;

        int error = -1;
        unique_lock<recursive_mutex> guard(_sslLok);
        do
        {
            ERR_clear_error();
            bytesJustReceived = SSL_read( _ssl, writer, (int)bytesToRecv );
            error = _handle_error( bytesJustReceived, waitTimeMillis );
        }
        while( _sok.valid() && waitTimeMillis > 0 && error > XSSL_ERROR_NONE );

        if( bytesJustReceived > 0 )
        {
            bytesToRecv -= bytesJustReceived;
            writer += bytesJustReceived;
        }
        else if( error == XSSL_ERROR_CRITICAL )
            CK_STHROW( ck_socket_exception, ("Critical error encountered with SSL socket io.") );
    }

    if( bytesToRecv > 0 )
        CK_STHROW( ck_socket_exception, ("Incomplete IO.") );
}

void ck_ssl_socket::close()
{
    unique_lock<recursive_mutex> guard(_sslLok);

    if( _ssl )
    {
        SSL_shutdown(_ssl);
        // A bug in SSL_free() doesn't release the string allocated by BIO_set_conn_hostname(), the work
        // around is to set it to NULL right before we free the SSL*.
        SSL_set_tlsext_host_name( _ssl, NULL );

        SSL_free(_ssl);

        _ssl = NULL;
    }

    if( _sok.valid() )
        _sok.close();
}

bool ck_ssl_socket::recv_wont_block( uint64_t& millis ) const
{
    return _sok.recv_wont_block( millis );
}

bool ck_ssl_socket::send_wont_block( uint64_t& millis ) const
{
    return _sok.send_wont_block( millis );
}

void ck_ssl_socket::use_ca_certificate_file( const ck_string& path )
{
    _caCertificateFilePath = path;
    if( _caCertificateFilePath.length() > 0 )
        SSL_CTX_load_verify_locations( _ctx, _caCertificateFilePath.c_str(), NULL );
}

void ck_ssl_socket::use_pem_certificate_file( const ck_string& path )
{
    _pemCertificateFilePath = path;
    if( SSL_CTX_use_certificate_file( _ctx, _pemCertificateFilePath.c_str(), SSL_FILETYPE_PEM ) != 1 )
        CK_THROW(( "Invalid certificate file. PEM format?" ));
}

void ck_ssl_socket::use_pem_certificate_chain_file( const ck_string& path )
{
    _pemCertificateChainFilePath = path;
    if( ! _pemCertificateChainFilePath.empty() )
    {
        if( SSL_CTX_use_certificate_chain_file( _ctx, _pemCertificateChainFilePath.c_str() ) != 1 )
            CK_THROW(( "Invalid certificate chain file. PEM format?" ));
    }    
}

void ck_ssl_socket::use_pem_rsa_private_key_file( const ck_string& path, const ck_string& privateKeyPassword )
{
    _pemRSAPrivateKeyFilePath = path;
    _privateKeyPassword = privateKeyPassword;

    if( SSL_CTX_use_RSAPrivateKey_file( _ctx, _pemRSAPrivateKeyFilePath.c_str(), SSL_FILETYPE_PEM ) != 1 )
        CK_THROW(( "Invalid private key file. RSA? PEM?" ));
}

int ck_ssl_socket::_handle_error( int error, uint64_t& waitTime )
{
    //Check if Close() was called by another thread before a Send/Recv/BufferedRecv was called.
    if( _ssl == NULL )
        return XSSL_ERROR_CRITICAL;

    //This loop pops errors off of the openssl error queue
    //so we don't end up with an ever growing queue
    unsigned long errVal;
    while( (errVal = ERR_get_error()) != 0 )
        CK_LOG_ERROR("SSL Error: %s", ERR_error_string(errVal, NULL) );

    //This line converts the error we get from doing IO into
    //a value we can pass to the switch statement
    int sslError = SSL_get_error(_ssl, (int)error );

    switch ( sslError )
    {
    case SSL_ERROR_NONE:
        return XSSL_ERROR_NONE;
    case SSL_ERROR_WANT_READ:
        if( !recv_wont_block( waitTime ) )
            CK_STHROW( ck_socket_exception, ("recv timeout") );
        break;
    case SSL_ERROR_WANT_WRITE:
        if( !send_wont_block( waitTime ) )
            CK_STHROW( ck_socket_exception, ("send timeout") );
        break;
    default:
        return XSSL_ERROR_CRITICAL;
    }

    //Check if Close() was called while  in _WaitToRecv/Send
    if( _ssl == NULL )
        return XSSL_ERROR_CRITICAL;

    return XSSL_ERROR_RETRY;
}

int ck_ssl_socket::_private_key_password_cb( char *buf, int size, int rwflag, void *userdata )
{
    if( rwflag != 0 )
        CK_THROW(( "Right now, we only support decrypting private keys."));

    if( !userdata )
        CK_THROW(( "No userdata passed to password callback."));

    ck_ssl_socket* sok = (ck_ssl_socket*)userdata;

    if( size < (int)sok->_privateKeyPassword.length() )
        CK_THROW(( "Not enough space for the password."));

    memset( buf, 0, size );
    memcpy( buf, sok->_privateKeyPassword.c_str(), sok->_privateKeyPassword.length() );

    return (int)sok->_privateKeyPassword.length();
}

int ck_ssl_socket::_x509_check_host(X509 *xcert, const ck_string& hostname)
{
    GENERAL_NAMES *gens = NULL;
    X509_NAME *name = NULL;
    int i;

    gens = (GENERAL_NAMES*)X509_get_ext_d2i(xcert, NID_subject_alt_name, NULL, NULL);
    if (gens)
    {
        int rv = 0;
        for (i = 0; i < sk_GENERAL_NAME_num(gens); i++)
        {
            GENERAL_NAME *gen = NULL;
            ASN1_STRING *cstr = NULL;
            gen = sk_GENERAL_NAME_value(gens, i);

            if(gen->type != GEN_DNS)
                continue;

            cstr = gen->d.dNSName;

            if ( _compare_strings(cstr, V_ASN1_IA5STRING, hostname) )
            {
                rv = 1;
                break;
            }
        }
        GENERAL_NAMES_free(gens);
        if (rv)
            return 1;

    }

    i = -1;
    name = X509_get_subject_name(xcert);
    while((i = X509_NAME_get_index_by_NID(name, NID_commonName, i)) >= 0)
    {
        X509_NAME_ENTRY *ne;
        ASN1_STRING *str;
        ne = X509_NAME_get_entry(name, i);
        str = X509_NAME_ENTRY_get_data(ne);
        if ( _compare_strings( str, -1, hostname ) )
            return 1;
    }
    return 0;
}

/* Compare while ASCII ignoring case. */
int ck_ssl_socket::_equal_no_case(const unsigned char *pattern, size_t pattern_len,
                                  const unsigned char *subject, size_t subject_len)
{
    if (pattern_len != subject_len)
        return 0;
    while (pattern_len)
    {
        unsigned char l = *pattern;
        unsigned char r = *subject;
        /* The pattern must not contain NUL characters. */
        if (l == 0)
            return 0;
        if (l != r)
        {
            if ('A' <= l && l <= 'Z')
                l = (l - 'A') + 'a';
            if ('A' <= r && r <= 'Z')
                r = (r - 'A') + 'a';
            if (l != r)
                return 0;
        }
        ++pattern;
        ++subject;
        --pattern_len;
    }
    return 1;
}

/* Compare the prefix and suffix with the subject, and check that the
   characters in-between are valid. */
int ck_ssl_socket::_wildcard_match(const unsigned char *prefix, size_t prefix_len,
                                   const unsigned char *suffix, size_t suffix_len,
                                   const unsigned char *subject, size_t subject_len)
{
    const unsigned char *wildcard_start;
    const unsigned char *wildcard_end;
    const unsigned char *p;
    if (subject_len < prefix_len + suffix_len)
        return 0;
    if (!_equal_no_case(prefix, prefix_len, subject, prefix_len))
        return 0;
    wildcard_start = subject + prefix_len;
    wildcard_end = subject + (subject_len - suffix_len);
    if (!_equal_no_case(wildcard_end, suffix_len, suffix, suffix_len))
        return 0;
    /* The wildcard must match at least one character. */
    if (wildcard_start == wildcard_end)
        return 0;
    /* Check that the part matched by the wildcard contains only
       permitted characters and only matches a single label. */
    for (p = wildcard_start; p != wildcard_end; ++p)
        if (!(('0' <= *p && *p <= '9') ||
                    ('A' <= *p && *p <= 'Z') ||
                    ('a' <= *p && *p <= 'z') ||
                    *p == '-'))
            return 0;
    return 1;
}

/* Checks if the memory region consistens of [0-9A-Za-z.-]. */
int ck_ssl_socket::_valid_domain_characters(const unsigned char *p, size_t len)
{
    while (len)
    {
        if (!(('0' <= *p && *p <= '9') ||
                    ('A' <= *p && *p <= 'Z') ||
                    ('a' <= *p && *p <= 'z') ||
                    *p == '-' || *p == '.'))
            return 0;
        ++p;
        --len;
    }
    return 1;
}

/* Find the '*' in a wildcard pattern.  If no such character is found
   or the pattern is otherwise invalid, returns NULL. */
const unsigned char *ck_ssl_socket::_wildcard_find_star(const unsigned char *pattern,
                                                         size_t pattern_len)
{
    const unsigned char *star = (unsigned char*)memchr(pattern, '*', pattern_len);
    size_t dot_count = 0;
    const unsigned char *suffix_start;
    size_t suffix_length;
    if (star == NULL)
        return NULL;
    suffix_start = star + 1;
    suffix_length = (pattern + pattern_len) - (star + 1);
    if (!(_valid_domain_characters(pattern, star - pattern) &&
          _valid_domain_characters(suffix_start, suffix_length)))
        return NULL;
    /* Check that the suffix matches at least two labels. */
    while (suffix_length)
    {
        if (*suffix_start == '.')
            ++dot_count;
        ++suffix_start;
        --suffix_length;
    }
    if (dot_count < 2)
        return NULL;
    return star;
}

/* Compare using wildcards. */
int ck_ssl_socket::_equal_wildcard(const unsigned char *pattern, size_t pattern_len,
                                    const unsigned char *subject, size_t subject_len)
{
    const unsigned char *star = _wildcard_find_star(pattern, pattern_len);
    if (star == NULL)
        return _equal_no_case(pattern, pattern_len,
                              subject, subject_len);
    return _wildcard_match(pattern, star - pattern,
                           star + 1, (pattern + pattern_len) - star - 1,
                           subject, subject_len);
}

/* Compare an ASN1_STRING to a supplied string. If they match
 * return 1. If cmp_type > 0 only compare if string matches the
 * type, otherwise convert it to UTF8.
 */

int ck_ssl_socket::_compare_strings(ASN1_STRING *a, int cmp_type, ck_string hostname )
{
    if (!a->data || !a->length)
        return 0;

    if (cmp_type > 0)
    {
        if (cmp_type != a->type)
            return 0;
        if (cmp_type == V_ASN1_IA5STRING)
            return _equal_wildcard(a->data, a->length, (unsigned char*)hostname.c_str(), hostname.length() );
        if (a->length == (int)hostname.length() && !memcmp(a->data, (unsigned char*)hostname.c_str(), hostname.length() ))
            return 1;
        else
            return 0;
    }
    else
    {
        int astrlen = 0, rv = 0;
        unsigned char *astr = NULL;
        try
        {
            astrlen = ASN1_STRING_to_UTF8(&astr, a);
            if (astrlen < 0)
                return -1;
            rv = _equal_wildcard(astr, astrlen, (unsigned char*)hostname.c_str(), hostname.length() );
            OPENSSL_free(astr);
        }
        catch(...)
        {
            if( astr )
                OPENSSL_free(astr);
            throw;
        }
        return rv;
    }
}

ck_ssl_socket_auth_exception::ck_ssl_socket_auth_exception(const char* msg, ...) :
    ck_exception()
{
    va_list args;
    va_start(args, msg);
    set_msg(ck_string::format(msg, args));
    va_end(args);
}