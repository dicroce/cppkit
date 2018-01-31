
#include "cppkit/ck_socket.h"
#include "cppkit/ck_md5.h"
#include "cppkit/os/ck_error_msg.h"
#include "cppkit/os/ck_time_utils.h"

#ifdef IS_POSIX
  #include <poll.h>
  #include <ifaddrs.h>
  #include <linux/if.h>
  #include <sys/ioctl.h>
#else
  #include <iphlpapi.h>
#endif

using namespace cppkit;
using namespace std;

static const int POLL_NFDS = 1;

bool ck_raw_socket::_sokSysStarted = false;
recursive_mutex ck_raw_socket::_sokLock;

ck_raw_socket::ck_raw_socket() :
#ifdef IS_POSIX
    _sok( -1 ),
#else
    _sok( INVALID_SOCKET ),
#endif
    _addr( 0 ),
    _host()
{
    socket_startup();
}

ck_raw_socket::ck_raw_socket( ck_raw_socket&& obj ) throw() :
    _sok( std::move( obj._sok ) ),
    _addr( std::move( obj._addr ) ),
    _host( std::move( obj._host ) )
{
#ifdef IS_POSIX
    obj._sok = -1;
#else
	obj._sok = INVALID_SOCKET;
#endif
    obj._host = ck_string();
}

ck_raw_socket::~ck_raw_socket() throw()
{
    if( valid() )
        close();
}

ck_raw_socket& ck_raw_socket::operator = ( ck_raw_socket&& obj ) throw()
{
    _sok = std::move( obj._sok );
#ifdef IS_POSIX
	obj._sok = -1;
#else
	obj._sok = INVALID_SOCKET;
#endif
	_addr = std::move(obj._addr);
    _host = std::move( obj._host );
    obj._host = ck_string();
    return *this;
}

void ck_raw_socket::create( int af )
{
    _sok = (SOCKET) ::socket( af, SOCK_STREAM, 0 );

    if( _sok <= 0 )
        CK_STHROW( ck_socket_exception, ("Unable to create socket: %s", ck_get_last_error_msg().c_str()) );

    int on = 1;
    if( ::setsockopt( (SOCKET)_sok, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(int) ) < 0 )
        CK_STHROW( ck_socket_exception, ("Unable to configure socket: %s", ck_get_last_error_msg().c_str()) );
}

void ck_raw_socket::connect( const ck_string& host, int port )
{    
    if( !valid() )
        create( ck_socket_address::get_address_family(host) );

    _host = host;

    _addr.set_address( host, port );

    int err = ::connect( _sok, _addr.get_sock_addr(), _addr.sock_addr_size());

	if (err < 0)
	{
		ck_string err = ck_get_last_error_msg();
		if(valid())
			CK_STHROW(ck_socket_connect_exception, ("Unable to connect to %s:%d reason: %s", _host.c_str(), port, err.c_str()));
	}
}

void ck_raw_socket::listen( int backlog )
{
    if( ::listen( _sok, backlog ) < 0 )
        CK_STHROW( ck_socket_exception, ( "Unable to listen on bound port: %s", ck_get_last_error_msg().c_str()) );
}

void ck_raw_socket::bind( int port, const ck_string& ip )
{
    _addr.set_address( ip, port );

    if( !valid() )
        create( _addr.address_family() );

    if( ::bind( _sok, _addr.get_sock_addr(), _addr.sock_addr_size() ) )
        CK_STHROW( ck_socket_exception, ( "Unable to bind given port and IP: %s", ck_get_last_error_msg().c_str()) );
}

ck_raw_socket ck_raw_socket::accept()
{
    if( _sok <= 0 )
        CK_STHROW( ck_socket_exception, ( "Unable to accept() on uninitialized socket." ));

    ck_raw_socket clientSocket;

    int clientSok = 0;
    socklen_t addrLength = _addr.sock_addr_size();

#ifdef IS_WINDOWS
    clientSok = (int)::accept( (SOCKET)_sok,
                               _addr.get_sock_addr(),
                               (int *) &addrLength );
#else
    clientSok = ::accept( _sok,
                          _addr.get_sock_addr(),
                          &addrLength );
#endif

    // Since the socket can be closed by another thread while we were waiting in accept(),
    // we only throw here if _sok is still a valid fd.
    if( valid() && clientSok <= 0 )
        CK_STHROW( ck_socket_exception, ( "Unable to accept inbound connection: %s", ck_get_last_error_msg().c_str()));

    clientSocket._sok = clientSok;

    return std::move(clientSocket);
}

int ck_raw_socket::raw_send( const void* buf, size_t len )
{
    int ret = 0;

#ifdef IS_WINDOWS
    ret = ::send(_sok, (char*)buf, (int)len, 0);
#else
    ret = (int)::send(_sok, buf, len, MSG_NOSIGNAL);
#endif

    return ret;
}

int ck_raw_socket::raw_recv( void* buf, size_t len )
{
    int ret = 0;

#ifdef IS_WINDOWS
    ret = ::recv(_sok, (char*)buf, (int)len, 0);
#else
    ret = (int)::recv(_sok, buf, len, 0);
#endif

    return ret;
}

void ck_raw_socket::close()
{
    if( _sok < 0 )
        return;

    SOCKET sokTemp = _sok;
    int err;

#ifdef IS_POSIX
    _sok = -1;
#else
	_sok = INVALID_SOCKET;
#endif

    FULL_MEM_BARRIER();

#ifdef IS_WINDOWS
    err = closesocket( sokTemp );
#else
    err = ::close( sokTemp );
#endif

    if( err < 0 )
        CK_LOG_WARNING( "Failed to close socket: %s", ck_get_last_error_msg().c_str() );
}

bool ck_raw_socket::recv_wont_block( uint64_t& millis ) const
{
    struct timeval beforePoll = { 0, 0 };
    ck_gettimeofday(&beforePoll);

    int retVal = 0;

#ifdef IS_POSIX 
    struct pollfd fds[POLL_NFDS];
    int nfds = POLL_NFDS;

    fds[0].fd = _sok;
    fds[0].events = POLLIN | POLLHUP | POLLERR | POLLNVAL | POLLRDHUP;
    fds[0].revents = 0;

    retVal = poll(fds, nfds, (int)millis );
#else
    WSAPOLLFD fds[POLL_NFDS];
    ULONG nfds = POLL_NFDS;

    fds[0].fd = _sok;    
	fds[0].events = POLLRDNORM;
    fds[0].revents = 0;

    retVal = WSAPoll(fds, nfds, (int)millis );
#endif

    if( retVal == 0 )
    {
        millis = 0;
        return false;
    }

    struct timeval afterPoll = { 0, 0 };
    ck_gettimeofday(&afterPoll);

    struct timeval delta = { 0, 0 };
    timersub( &afterPoll, &beforePoll, &delta );

    uint64_t deltaMillis = (delta.tv_sec * 1000) + (delta.tv_usec / 1000);
    
    millis = (deltaMillis >= millis) ? 0 : (millis - deltaMillis);

    if( retVal < 0 )
        CK_STHROW( ck_socket_exception, ("poll() error: %s", ck_get_last_error_msg().c_str()) );

    return true;
}

bool ck_raw_socket::send_wont_block( uint64_t& millis ) const
{
    struct timeval beforePoll = { 0, 0 };
    ck_gettimeofday(&beforePoll);

    int retVal = 0;

#ifdef IS_POSIX 
    struct pollfd fds[POLL_NFDS];
    int nfds = POLL_NFDS;

    fds[0].fd = _sok;
    fds[0].events = POLLOUT | POLLHUP;
    fds[0].revents = 0;

    retVal = poll(fds, nfds, millis );
#else
    WSAPOLLFD fds[POLL_NFDS];
    ULONG nfds = POLL_NFDS;

    fds[0].fd = _sok;
	fds[0].events = POLLWRNORM;
    fds[0].revents = 0;

    retVal = WSAPoll(fds, nfds, (int)millis );
#endif

    if( retVal == 0 )
    {
        millis = 0;
        return false;
    }

    struct timeval afterPoll = { 0, 0 };
    ck_gettimeofday(&afterPoll);

    struct timeval delta = { 0, 0 };
    timersub( &afterPoll, &beforePoll, &delta );

    uint64_t deltaMillis = (delta.tv_sec * 1000) + (delta.tv_usec / 1000);
    
    millis = (deltaMillis >= millis) ? 0 : (millis - deltaMillis);

    if( retVal < 0 )
        CK_STHROW( ck_socket_exception, ("poll() error: %s", ck_get_last_error_msg().c_str()) );

    return true;
}

ck_string ck_raw_socket::get_peer_ip() const
{
    struct sockaddr_storage peer;
    int peerLength = sizeof(peer);

#ifdef IS_WINDOWS
    if ( getpeername(_sok,(sockaddr*)&peer,&peerLength) < 0 )
    {
        CK_LOG_WARNING("Unable to get peer ip. %s", ck_get_last_error_msg().c_str());
        return "";
    }
#else
    if ( getpeername(_sok,(sockaddr*)&peer,(socklen_t*)&peerLength) < 0 )
    {
        CK_LOG_WARNING("Unable to get peer ip: %s", ck_get_last_error_msg().c_str());
        return "";
    }
#endif

    return ck_socket_address::address_to_string((sockaddr*)&peer, (socklen_t)peerLength);
}

ck_string ck_raw_socket::get_local_ip() const
{
    struct sockaddr_storage local;
    int addrLength = sizeof(local);

#ifdef IS_WINDOWS
    if ( getsockname(_sok, (sockaddr*)&local, &addrLength) < 0 )
    {
        CK_LOG_WARNING("Unable to get local ip. %s", ck_get_last_error_msg().c_str());
        return "";
    }
#else
    if ( getsockname(_sok, (sockaddr*)&local, (socklen_t*)&addrLength) < 0 )
    {
        CK_LOG_WARNING("Unable to get local ip: %s", ck_get_last_error_msg().c_str());
        return "";
    }
#endif

    return ck_socket_address::address_to_string((sockaddr*)&local, (socklen_t)addrLength);
}

void ck_raw_socket::socket_startup()
{
#ifdef IS_WINDOWS
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD( 2, 2 );

    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 )
        CK_STHROW( ck_socket_exception, ( "Unable to load WinSock DLL." ));

    if ( LOBYTE( wsaData.wVersion ) != 2 ||
         HIBYTE( wsaData.wVersion ) != 2 )
    {
        CK_STHROW( ck_socket_exception, ( "Unable to load WinSock DLL." ));
    }
#endif
}

vector<ck_string> cppkit::ck_resolve( int type, const ck_string& name )
{
    vector<ck_string> addresses;

    struct addrinfo hints, *addrInfo = nullptr;
    memset( &hints, 0, sizeof( hints ) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_CANONNAME;// | AI_NUMERICHOST;

    int err = getaddrinfo( name.c_str(), 0, &hints, &addrInfo );
    if (err)
        CK_STHROW( ck_socket_exception, ("Failed to resolve address by hostname: %s", ck_get_error_msg(err).c_str()) );

    for( struct addrinfo* cur = addrInfo; cur != 0; cur = cur->ai_next )
    {
        // We're only interested in IPv4 and IPv6
        if( (cur->ai_family != AF_INET) && (cur->ai_family != AF_INET6) )
            continue;

        if( cur->ai_addr->sa_family == type )
            addresses.push_back( ck_socket_address::address_to_string(cur->ai_addr, (socklen_t)cur->ai_addrlen) );
    }

    freeaddrinfo( addrInfo );

    return addresses;
}

map<string,vector<ck_string>> cppkit::ck_get_interface_addresses( int af )
{
    map<string,vector<ck_string> > interfaceAddresses;

#ifdef IS_WINDOWS
    ULONG adapterInfoBufferSize = (sizeof( IP_ADAPTER_ADDRESSES ) * 32);
    unsigned char adapterInfoBuffer[(sizeof( IP_ADAPTER_ADDRESSES ) * 32)];
    PIP_ADAPTER_ADDRESSES adapterAddress = (PIP_ADAPTER_ADDRESSES)&adapterInfoBuffer[0];

    int err = GetAdaptersAddresses( af,
                                    0,
                                    0,
                                    adapterAddress,
                                    &adapterInfoBufferSize );

    if( err != ERROR_SUCCESS )
        CK_STHROW( ck_socket_exception, ("Unable to query available network interfaces. %s", ck_get_error_msg(err).c_str() ));

    while( adapterAddress )
    {
        wchar_t ipstringbuffer[46];
        DWORD ipbufferlength = 46;

        if( WSAAddressToStringW( (LPSOCKADDR)adapterAddress->FirstUnicastAddress->Address.lpSockaddr,
                                 adapterAddress->FirstUnicastAddress->Address.iSockaddrLength,
                                 nullptr,
                                 ipstringbuffer,
                                 &ipbufferlength ) == 0 )
        {
            ck_string key = (PWCHAR)adapterAddress->FriendlyName;
            ck_string val = ipstringbuffer;

            if( interfaceAddresses.find(key) == interfaceAddresses.end() )
            {
                std::vector<ck_string> addresses;
                interfaceAddresses.insert( make_pair(key, addresses) );
            }
            interfaceAddresses.find(key)->second.push_back( val );
        }

        adapterAddress = adapterAddress->Next;
    }
#else
    struct ifaddrs* ifaddrs = nullptr, *ifa = nullptr;
    int family = 0, s = 0;
    char host[NI_MAXHOST];

    if( getifaddrs( &ifaddrs ) == -1 )
        CK_STHROW( ck_socket_exception, ( "Unable to query network interfaces: %s", ck_get_last_error_msg().c_str() ));

    for( ifa = ifaddrs; ifa != nullptr; ifa = ifa->ifa_next )
    {
        if( ifa->ifa_addr == nullptr )
        continue;

        family = ifa->ifa_addr->sa_family;

        if( family != af )
            continue;

        ck_string key = ifa->ifa_name;
        s = getnameinfo( ifa->ifa_addr,
                         (family==AF_INET) ?
                             sizeof( struct sockaddr_in ) :
                             sizeof( struct sockaddr_in6 ),
                         host,
                         NI_MAXHOST,
                         nullptr,
                         0,
                         NI_NUMERICHOST );

        // s will be 0 if getnameinfo was successful
        if( !s )
        {

            if( interfaceAddresses.find(key) == interfaceAddresses.end() )
            {
                std::vector<ck_string> addresses;
                interfaceAddresses.insert( make_pair(key, addresses) );
            }
            ck_string val = host;
            interfaceAddresses.find(key)->second.push_back( val );
        }
        else
            CK_LOG_WARNING("Failed on call to getnameinfo(). %s", ck_get_error_msg(s).c_str());
    }

    freeifaddrs( ifaddrs );
#endif

    return interfaceAddresses;
}

vector<uint8_t> cppkit::ck_get_hardware_address(const ck_string& ifname)
{
    vector<uint8_t> buffer(6);

#ifdef IS_LINUX
    int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    if(fd < 0)
        CK_THROW(("Unable to create datagram socket."));

    struct ifreq s;
    strcpy(s.ifr_name, ifname.c_str());

    if(ioctl(fd, SIOCGIFHWADDR, &s) < 0)
    {
        close(fd);

        CK_THROW(("Unable to query MAC address."));
    }

    close(fd);

    memcpy(&buffer[0], &s.ifr_addr.sa_data[0], 6);
#else
	IP_ADAPTER_INFO adapterInfo[16];
	DWORD dwBufLen = sizeof(adapterInfo);

    if(GetAdaptersInfo(adapterInfo, &dwBufLen) != ERROR_SUCCESS)
        CK_THROW(("Unable to query adapter info."));

    memcpy(&buffer[0], adapterInfo->Address, 6);
#endif

    return buffer;
}

ck_string cppkit::ck_get_device_uuid(const cppkit::ck_string& ifname)
{
    auto hwaddr = ck_get_hardware_address(ifname);

    ck_md5 h;
    h.update(&hwaddr[0], 6);
    h.finalize();

    vector<uint8_t> buffer(16);
    h.get(&buffer[0]);
    // 6957a8f7-f1ab-4e87-9377-62cea97766f5
    return ck_string::format("%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                             buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7],
                             buffer[8], buffer[9], buffer[10], buffer[11], buffer[12], buffer[13], buffer[14], buffer[15]);
}

int ck_socket::raw_send( const void* buf, size_t len )
{
    return _sok.raw_send( buf, len );
}

int ck_socket::raw_recv( void* buf, size_t len )
{
    return _sok.raw_recv( buf, len );
}

void ck_socket::send( const void* buf, size_t len )
{
    int bytesToSend = (int)len;
    const uint8_t* reader = (uint8_t*)buf;

    uint64_t timeout = _ioTimeOut;

    while( valid() && bytesToSend > 0 && timeout > 0 )
    {
        if( send_wont_block( timeout ) )
        {
            int bytesJustSent = _sok.raw_send( reader, bytesToSend );
            if(bytesJustSent <= 0)
            {
                close();
                CK_STHROW(ck_socket_exception, ("io error in send()"));
            }
            else
            {
                reader += bytesJustSent;
                bytesToSend -= bytesJustSent;
            }
        }
        else CK_STHROW( ck_socket_exception, ("send timeout") );
    }

    if( bytesToSend > 0 )
        CK_STHROW(ck_socket_exception, ("incomplete io in send()."));
}

void ck_socket::recv( void* buf, size_t len )
{
    int bytesToRecv = (int)len;
    uint8_t* writer = (uint8_t*)buf;

    uint64_t timeout = _ioTimeOut;

    while( valid() && bytesToRecv > 0 && timeout > 0 )
    {
        if( recv_wont_block( timeout ) )
        {
            int bytesJustRecv = _sok.raw_recv( writer, bytesToRecv );
     
            if(bytesJustRecv <= 0)
            {
                close();
                CK_STHROW(ck_socket_exception, ("io error in recv()"));
            }
            else
            {
                writer += bytesJustRecv;
                bytesToRecv -= bytesJustRecv;
            }
        }
        else CK_STHROW( ck_socket_exception, ("recv timeout") );
    }

    if( bytesToRecv > 0 )
        CK_STHROW(ck_socket_exception, ("incomplete io in recv()."));
}

uint16_t cppkit::ck_ntohs(uint16_t x)
{
    return ntohs(x);
}

uint16_t cppkit::ck_htons(uint16_t x)
{
    return htons(x);
}

uint32_t cppkit::ck_ntohl(uint32_t x)
{
    return ntohl(x);
}

uint32_t cppkit::ck_htonl(uint32_t x)
{
    return htonl(x);
}

uint64_t cppkit::ck_ntohll(uint64_t x)
{
    return (((uint64_t) ntohl(x & 0xFFFFFFFF)) << 32LL) + ntohl(x >> 32);
}

uint64_t cppkit::ck_htonll(uint64_t x)
{
    return (((uint64_t) htonl(x & 0xFFFFFFFF)) << 32LL) + htonl(x >> 32);
}

ck_socket_connect_exception::ck_socket_connect_exception(const char* msg, ...) :
    ck_exception()
{
    va_list args;
    va_start(args, msg);
    set_msg(ck_string::format(msg, args));
    va_end(args);
}

ck_socket_exception::ck_socket_exception(const char* msg, ...) :
    ck_exception()
{
    va_list args;
    va_start(args, msg);
    set_msg(ck_string::format(msg, args));
    va_end(args);
}
