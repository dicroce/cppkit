
#ifndef cppkit_ck_ssl_socket_h
#define cppkit_ck_ssl_socket_h

#include "cppkit/ck_socket.h"
#include "cppkit/interfaces/ck_stream_io.h"
#include "cppkit/interfaces/ck_pollable.h"
#include <mutex>
#include "openssl/ssl.h"

extern "C"
{
    struct CRYPTO_dynlock_value
    {
        std::recursive_mutex _mutex;
    };
}

class ck_ssl_socket_test;

namespace cppkit
{

class ck_ssl_socket : public ck_stream_io, public ck_pollable
{
    friend class ::ck_ssl_socket_test;

public:
    CK_API ck_ssl_socket();
    CK_API ck_ssl_socket( ck_ssl_socket&& obj ) throw();
    CK_API ck_ssl_socket( const ck_ssl_socket& ) = delete;
    CK_API virtual ~ck_ssl_socket() throw();

    CK_API ck_ssl_socket& operator = ( ck_ssl_socket&& obj ) throw();
    CK_API ck_ssl_socket& operator = ( const ck_socket& ) = delete;

    CK_API void verify_connection( bool verify );

    CK_API void create( int af );

    CK_API void connect( const ck_string& host, int port );
    CK_API void listen( int backlog = ck_socket::MAX_BACKLOG );
    CK_API void bind( int port, const ck_string& ip = "" );
    CK_API ck_ssl_socket accept();

    CK_API SOCKET get_sok_id() const { return _sok.get_sok_id(); }

    CK_API void set_host_name( const ck_string& hostName ) { _hostName = hostName; }

    // ck_socket_io API
    CK_API virtual int raw_send( const void* buf, size_t len );
    CK_API virtual int raw_recv( void* buf, size_t len );

    // ck_stream_io API
    CK_API virtual bool valid() const { return _sok.valid(); }
    CK_API virtual void send( const void* buf, size_t len );
    CK_API virtual void recv( void* buf, size_t len );

    CK_API void close();

    // ck_pollable API
    CK_API virtual bool recv_wont_block( uint64_t& millis ) const;
    CK_API virtual bool send_wont_block( uint64_t& millis ) const;

    /// Use this path to a CACertificate file to verify the SSL socket
    /// \param path The path to a PEM encoded certificate file.
    CK_API virtual void use_ca_certificate_file( const cppkit::ck_string& path );

    /// Associate this SSL socket with a particular certificate.
    /// \param path The path to a PEM encoded certificate file.
    CK_API virtual void use_pem_certificate_file( const cppkit::ck_string& path );

    /// Associate this SSL socket with a particular certificate chain file.
    /// Calling this method will override any calls to use_pem_certificate_file.
    /// The certificates must be in PEM format and must be sorted starting with the subject's
    /// certificate (actual client or server certificate), followed by intermediate CA
    /// certificates if applicable, and ending at the highest level (root) CA.
    /// \param path The path to a PEM encoded certificate file.
    CK_API virtual void use_pem_certificate_chain_file( const cppkit::ck_string& path );

    /// Associate this SSL socket with a particular private key.
    /// \param path The path to a PEM encoded certificate file.
    /// \param privateKeyPassword If the private key is encrypted, decrypt it with this password.

    CK_API virtual void use_pem_rsa_private_key_file( const cppkit::ck_string& path,
                                                      const cppkit::ck_string& privateKeyPassword = "" );

private:
    int _handle_error( int error, uint64_t& waitTime );

    static int _private_key_password_cb( char *buf, int size, int rwflag, void *userdata );
    static int _x509_check_host(X509 *xcert, const ck_string& hostname);
    static int _equal_no_case(const unsigned char *pattern, size_t pattern_len, const unsigned char *subject, size_t subject_len);
    static int _wildcard_match(const unsigned char *prefix, size_t prefix_len, const unsigned char *suffix, size_t suffix_len,
                               const unsigned char *subject, size_t subject_len);
    static int _valid_domain_characters(const unsigned char *p, size_t len);
    static const unsigned char *_wildcard_find_star(const unsigned char *pattern, size_t pattern_len);
    static int _equal_wildcard(const unsigned char *pattern, size_t pattern_len, const unsigned char *subject, size_t subject_len);
    static int _compare_strings(ASN1_STRING *a, int cmp_type, ck_string hostname );

    SSL_CTX* _ctx;
    SSL* _ssl;
    ck_socket _sok;
    ck_string _hostName;
    bool _verifyConnection;
    ck_string _privateKeyPassword;
    ck_string _bindAddress;
    int _bindPort;
    ck_string _caCertificateFilePath;
    ck_string _pemCertificateChainFilePath;
    ck_string _pemCertificateFilePath;
    ck_string _pemRSAPrivateKeyFilePath;

    std::recursive_mutex _sslLok;

    static std::recursive_mutex _storeLok;
    static bool _certStoreInitialized;

public:
    /// We have some global SSL state initializatoin that is done with the
    /// static initialization trick.
    class static_init
    {
    public:
        static_init();
        ~static_init();

        static void lock(int mode, int n, const char* file, int line);
        static size_t id();
        static struct CRYPTO_dynlock_value* dynlock_create(const char* file, int line);
        static void dynlock(int mode, struct CRYPTO_dynlock_value* lock, const char* file, int line);
        static void dynlock_destroy(struct CRYPTO_dynlock_value* lock, const char* file, int line);

    private:
        static std::recursive_mutex* _mutexes;
        static std::recursive_mutex _initLok;
        static bool _init;
    };

    friend class static_init;

    const static static_init _sslstatic_init;
};

class ck_ssl_socket_auth_exception : public ck_exception
{
public:
    CK_API ck_ssl_socket_auth_exception(const char* msg, ...);
    CK_API virtual ~ck_ssl_socket_auth_exception() throw() {}
};

}
#endif
