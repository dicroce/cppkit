
#include "cppkit/ck_socket.h"
#include "cppkit/ck_sha_256.h"
#include "cppkit/ck_string_utils.h"

#include <poll.h>
#include <ifaddrs.h>
#ifdef IS_DARWIN
#include <net/if.h>
#include <net/if_dl.h>
#else
#include <linux/if.h>
#endif
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/ioctl.h>
#include <sys/time.h>

using namespace cppkit;
using namespace std;

static const int POLL_NFDS = 1;

bool ck_raw_socket::_sokSysStarted = false;
recursive_mutex ck_raw_socket::_sokLock;

ck_raw_socket::ck_raw_socket() :
    _sok( -1 ),
    _addr( 0 ),
    _host()
{
}

ck_raw_socket::ck_raw_socket( ck_raw_socket&& obj ) noexcept :
    _sok( std::move( obj._sok ) ),
    _addr( std::move( obj._addr ) ),
    _host( std::move( obj._host ) )
{
    obj._sok = -1;
    obj._host = string();
}

ck_raw_socket::~ck_raw_socket() noexcept
{
    if( valid() )
        close();
}

ck_raw_socket& ck_raw_socket::operator = ( ck_raw_socket&& obj ) noexcept
{
    if(valid())
        close();

    _sok = std::move( obj._sok );
	obj._sok = -1;
	_addr = std::move(obj._addr);
    _host = std::move( obj._host );
    obj._host = string();
    return *this;
}

void ck_raw_socket::create( int af )
{
    _sok = (SOCKET) ::socket( af, SOCK_STREAM, 0 );

    if( _sok <= 0 )
        CK_STHROW( ck_socket_exception, ("Unable to create socket.") );

    int on = 1;
    if( ::setsockopt( (SOCKET)_sok, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(int) ) < 0 )
        CK_STHROW( ck_socket_exception, ("Unable to configure socket.") );
}

void ck_raw_socket::connect( const string& host, int port )
{
    if( !valid() )
        create( ck_socket_address::get_address_family(host) );

    _host = host;

    _addr.set_address( host, port );

    int err = ::connect( _sok, _addr.get_sock_addr(), _addr.sock_addr_size());

	if (err < 0)
	{
		if(valid())
			CK_STHROW(ck_socket_connect_exception, ("Unable to connect to %s:%d.", _host.c_str(), port));
	}
}

void ck_raw_socket::listen( int backlog )
{
    if( ::listen( _sok, backlog ) < 0 )
        CK_STHROW( ck_socket_exception, ( "Unable to listen on bound port") );
}

void ck_raw_socket::bind( int port, const string& ip )
{
    _addr.set_address( ip, port );

    if( !valid() )
        create( _addr.address_family() );

    if( ::bind( _sok, _addr.get_sock_addr(), _addr.sock_addr_size() ) )
        CK_STHROW( ck_socket_exception, ( "Unable to bind given port and IP.") );
}

ck_raw_socket ck_raw_socket::accept()
{
    if( _sok <= 0 )
        CK_STHROW( ck_socket_exception, ( "Unable to accept() on uninitialized socket." ));

    ck_raw_socket clientSocket;

    int clientSok = 0;
    socklen_t addrLength = _addr.sock_addr_size();

    clientSok = ::accept( _sok,
                          _addr.get_sock_addr(),
                          &addrLength );

    // Since the socket can be closed by another thread while we were waiting in accept(),
    // we only throw here if _sok is still a valid fd.
    if( valid() && clientSok <= 0 )
        CK_STHROW( ck_socket_exception, ( "Unable to accept inbound connection."));

    clientSocket._sok = clientSok;

    return clientSocket;
}

int ck_raw_socket::raw_send( const void* buf, size_t len )
{
    return (int)::send(_sok, buf, len, 0);
}

int ck_raw_socket::raw_recv( void* buf, size_t len )
{
    return (int)::recv(_sok, buf, len, 0);
}

void ck_raw_socket::close()
{
    if( _sok < 0 )
        return;

    SOCKET sokTemp = _sok;
    int err;

    _sok = -1;

    FULL_MEM_BARRIER();

    err = ::close( sokTemp );

    if( err < 0 )
        CK_LOG_WARNING( "Failed to close socket." );
}

bool ck_raw_socket::wait_till_recv_wont_block( uint64_t& millis ) const
{
    struct timeval beforePoll = { 0, 0 };
    gettimeofday(&beforePoll, nullptr);

    int retVal = 0;

    struct pollfd fds[POLL_NFDS];
    int nfds = POLL_NFDS;

    #ifndef POLLRDHUP
    #define POLLRDHUP 0x2000
    #endif

    fds[0].fd = _sok;
    fds[0].events = POLLIN | POLLHUP | POLLERR | POLLNVAL | POLLRDHUP;
    fds[0].revents = 0;

    retVal = poll(fds, nfds, (int)millis );

    if( retVal == 0 )
    {
        millis = 0;
        return false;
    }

    struct timeval afterPoll = { 0, 0 };
    gettimeofday(&afterPoll, nullptr);

    struct timeval delta = { 0, 0 };
    timersub( &afterPoll, &beforePoll, &delta );

    uint64_t deltaMillis = (delta.tv_sec * 1000) + (delta.tv_usec / 1000);

    millis = (deltaMillis >= millis) ? 0 : (millis - deltaMillis);

    if( retVal < 0 )
        CK_STHROW( ck_socket_exception, ("poll() error.") );

    return true;
}

bool ck_raw_socket::wait_till_send_wont_block( uint64_t& millis ) const
{
    struct timeval beforePoll = { 0, 0 };
    gettimeofday(&beforePoll, nullptr);

    int retVal = 0;

    struct pollfd fds[POLL_NFDS];
    int nfds = POLL_NFDS;

    fds[0].fd = _sok;
    fds[0].events = POLLOUT | POLLHUP;
    fds[0].revents = 0;

    retVal = poll(fds, nfds, millis );

    if( retVal == 0 )
    {
        millis = 0;
        return false;
    }

    struct timeval afterPoll = { 0, 0 };
    gettimeofday(&afterPoll, nullptr);

    struct timeval delta = { 0, 0 };
    timersub( &afterPoll, &beforePoll, &delta );

    uint64_t deltaMillis = (delta.tv_sec * 1000) + (delta.tv_usec / 1000);

    millis = (deltaMillis >= millis) ? 0 : (millis - deltaMillis);

    if( retVal < 0 )
        CK_STHROW( ck_socket_exception, ("poll() error.") );

    return true;
}

string ck_raw_socket::get_peer_ip() const
{
    struct sockaddr_storage peer;
    int peerLength = sizeof(peer);

    if ( getpeername(_sok,(sockaddr*)&peer,(socklen_t*)&peerLength) < 0 )
    {
        CK_LOG_WARNING("Unable to get peer ip.");
        return string();
    }

    return ck_socket_address::address_to_string((sockaddr*)&peer, (socklen_t)peerLength);
}

string ck_raw_socket::get_local_ip() const
{
    struct sockaddr_storage local;
    int addrLength = sizeof(local);

    if ( getsockname(_sok, (sockaddr*)&local, (socklen_t*)&addrLength) < 0 )
    {
        CK_LOG_WARNING("Unable to get local ip.");
        return "";
    }

    return ck_socket_address::address_to_string((sockaddr*)&local, (socklen_t)addrLength);
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
        if( wait_till_send_wont_block( timeout ) )
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
        if( wait_till_recv_wont_block( timeout ) )
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

ck_socket_connect_exception::ck_socket_connect_exception(const char* msg, ...) :
    ck_exception()
{
    va_list args;
    va_start(args, msg);
    _msg = ck_string_utils::format(msg, args);
    va_end(args);
}

ck_socket_exception::ck_socket_exception(const char* msg, ...) :
    ck_exception()
{
    va_list args;
    va_start(args, msg);
    _msg = ck_string_utils::format(msg, args);
    va_end(args);
}

vector<string> cppkit::ck_networking::ck_resolve( int type, const string& name )
{
    vector<string> addresses;

    struct addrinfo hints, *addrInfo = nullptr;
    memset( &hints, 0, sizeof( hints ) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_CANONNAME;// | AI_NUMERICHOST;

    int err = getaddrinfo( name.c_str(), 0, &hints, &addrInfo );
    if (err)
        CK_STHROW( ck_socket_exception, ("Failed to resolve address by hostname.") );

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

map<string,vector<string>> cppkit::ck_networking::ck_get_interface_addresses( int af )
{
    map<string,vector<string> > interfaceAddresses;

    struct ifaddrs* ifaddrs = nullptr, *ifa = nullptr;
    int family = 0, s = 0;
    char host[NI_MAXHOST];

    if( getifaddrs( &ifaddrs ) == -1 )
        CK_STHROW( ck_socket_exception, ( "Unable to query network interfaces."));

    for( ifa = ifaddrs; ifa != nullptr; ifa = ifa->ifa_next )
    {
        if( ifa->ifa_addr == nullptr )
        continue;

        family = ifa->ifa_addr->sa_family;

        if( family != af )
            continue;

        string key = ifa->ifa_name;
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
                std::vector<string> addresses;
                interfaceAddresses.insert( make_pair(key, addresses) );
            }
            string val = host;
            interfaceAddresses.find(key)->second.push_back( val );
        }
        else
            CK_LOG_WARNING("Failed on call to getnameinfo().");
    }

    freeifaddrs( ifaddrs );

    return interfaceAddresses;
}

vector<uint8_t> cppkit::ck_networking::ck_get_hardware_address(const string& ifname)
{
    vector<uint8_t> buffer(6);

#ifdef IS_DARWIN

    // here comes
    int mib[6]; // galaxy defender...
    mib[0] = CTL_NET;
    mib[1] = AF_ROUTE;
    mib[2] = 0;
    mib[3] = AF_LINK;
    mib[4] = NET_RT_IFLIST;

    if((mib[5] = if_nametoindex(ifname.c_str())) == 0)
        CK_THROW(("Unable to query MAC address."));

    size_t len;
    if(sysctl(mib, 6, NULL, &len, NULL, 0) < 0)
        CK_THROW(("Unable to query MAC address."));

    vector<uint8_t> buf(len);

    if(sysctl(mib, 6, &buf[0], &len, NULL, 0) < 0)
        CK_THROW(("Unable to query MAC address."));

    struct sockaddr_dl* sdl = (struct sockaddr_dl *)((struct if_msghdr *)&buf[0] + 1);

    unsigned char* ptr = (unsigned char *)LLADDR(sdl);

    memcpy(&buffer[0], ptr, 6);

#else

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

#endif

    return buffer;
}

string cppkit::ck_networking::ck_get_device_uuid(const std::string& ifname)
{
    auto hwaddr = ck_get_hardware_address(ifname);

    ck_sha_256 h;
    h.update(&hwaddr[0], 6);
    h.finalize();

    vector<uint8_t> buffer(32);
    h.get(&buffer[0]);
    // 6957a8f7-f1ab-4e87-9377-62cea97766f5
    return ck_string_utils::format("%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                            buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7],
                            buffer[8], buffer[9], buffer[10], buffer[11], buffer[12], buffer[13], buffer[14], buffer[15]);
}

uint16_t cppkit::ck_networking::ck_ntohs(uint16_t x)
{
    return ntohs(x);
}

uint16_t cppkit::ck_networking::ck_htons(uint16_t x)
{
    return htons(x);
}

uint32_t cppkit::ck_networking::ck_ntohl(uint32_t x)
{
    return ntohl(x);
}

uint32_t cppkit::ck_networking::ck_htonl(uint32_t x)
{
    return htonl(x);
}

uint64_t cppkit::ck_networking::ck_ntohll(uint64_t x)
{
    return (((uint64_t) ntohl(x & 0xFFFFFFFF)) << 32LL) + ntohl(x >> 32);
}

uint64_t cppkit::ck_networking::ck_htonll(uint64_t x)
{
    return (((uint64_t) htonl(x & 0xFFFFFFFF)) << 32LL) + htonl(x >> 32);
}
