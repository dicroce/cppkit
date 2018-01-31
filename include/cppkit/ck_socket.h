
/// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=--=-=-=-=-=-
/// cppkit - http://www.cppkit.org
/// Copyright (c) 2013, Tony Di Croce
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without modification, are permitted
/// provided that the following conditions are met:
///
/// 1. Redistributions of source code must retain the above copyright notice, this list of conditions and
///    the following disclaimer.
/// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
///    and the following disclaimer in the documentation and/or other materials provided with the
///    distribution.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
/// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
/// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
/// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
/// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
/// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
/// TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
/// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///
/// The views and conclusions contained in the software and documentation are those of the authors and
/// should not be interpreted as representing official policies, either expressed or implied, of the cppkit
/// Project.
/// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=--=-=-=-=-=-

#ifndef cppkit_socket_h
#define cppkit_socket_h

#include "cppkit/interfaces/ck_stream_io.h"
#include "cppkit/interfaces/ck_socket_io.h"
#include "cppkit/interfaces/ck_pollable.h"
#include "cppkit/os/ck_platform.h"
#include "cppkit/ck_socket_address.h"
#include <memory>
#include <map>
#include <mutex>

#ifdef IS_WINDOWS
  #include <WinSock2.h>
  #include <ws2tcpip.h>
  #define SOCKET_SHUT_FLAGS SD_BOTH
  #define SOCKET_SHUT_SEND_FLAGS SD_SEND
  #define SOCKET_SHUT_RECV_FLAGS SD_RECEIVE
#else
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <sys/time.h>
  #include <netinet/in.h>
  #include <netdb.h>
  #include <unistd.h>
  #include <arpa/inet.h>
  typedef int SOCKET;
  #define SOCKET_SHUT_FLAGS SHUT_RDWR
  #define SOCKET_SHUT_SEND_FLAGS SHUT_WR
  #define SOCKET_SHUT_RECV_FLAGS SHUT_RD
#endif

class ck_socket_test;

namespace cppkit
{

// Simple tcp socket.
class ck_raw_socket : public ck_socket_io, public ck_pollable
{
    friend class ::ck_socket_test;

public:
    enum ck_raw_socket_defaults
    {
        MAX_BACKLOG = 5
    };

    CK_API ck_raw_socket();
    CK_API ck_raw_socket( ck_raw_socket&& obj ) throw();
    CK_API ck_raw_socket( const ck_raw_socket& ) = delete;
    CK_API virtual ~ck_raw_socket() throw();

    CK_API ck_raw_socket& operator = ( ck_raw_socket&& obj ) throw();
    CK_API ck_raw_socket& operator = ( const ck_raw_socket& ) = delete;

    CK_API void create( int af );

    CK_API void connect( const ck_string& host, int port );
    CK_API void listen( int backlog = MAX_BACKLOG );
    CK_API void bind( int port, const ck_string& ip = "" );
    CK_API ck_raw_socket accept();

    CK_API inline SOCKET get_sok_id() const { return _sok; }

    CK_API inline bool valid() const
	{ 
#ifdef IS_POSIX
		return (_sok > 0) ? true : false;
#else
		return (_sok != INVALID_SOCKET) ? true : false;
#endif
	}

    // ck_socket_io
    CK_API virtual int raw_send( const void* buf, size_t len );
    CK_API virtual int raw_recv( void* buf, size_t len );

    CK_API void close();

    // ck_pollable
    CK_API virtual bool recv_wont_block( uint64_t& millis ) const;
    CK_API virtual bool send_wont_block( uint64_t& millis ) const;

    CK_API ck_string get_peer_ip() const;
    CK_API ck_string get_local_ip() const;

    static void socket_startup();

protected:
    SOCKET _sok;
    ck_socket_address _addr;
    ck_string _host;

    static bool _sokSysStarted;
    static std::recursive_mutex _sokLock;
};

class ck_socket : public ck_stream_io, public ck_pollable
{
    friend class ::ck_socket_test;

public:
    enum ck_socket_defaults
    {
        MAX_BACKLOG = 5
    };

    CK_API inline ck_socket() : _sok(), _ioTimeOut(5000) {}
    CK_API inline ck_socket( ck_socket&& obj ) throw() :
        _sok( std::move(obj._sok) ),
        _ioTimeOut( std::move(obj._ioTimeOut )) {
    }

    CK_API ck_socket( const ck_socket& ) = delete;
    CK_API inline virtual ~ck_socket() throw() {}

    CK_API inline ck_socket& operator = ( ck_socket&& obj ) throw() {
        _sok = std::move(obj._sok);
        _ioTimeOut = std::move(obj._ioTimeOut);
        return *this;
    }
    CK_API ck_socket& operator = ( const ck_socket& ) = delete;

    CK_API void set_io_timeout( uint64_t ioTimeOut ) { _ioTimeOut = ioTimeOut; }

    CK_API inline void create( int af ) { _sok.create(af); }

    CK_API inline void connect( const ck_string& host, int port ) { _sok.connect(host, port); }
    CK_API inline void listen( int backlog = MAX_BACKLOG ) { _sok.listen(backlog); }
    CK_API inline void bind( int port, const ck_string& ip = "" ) { _sok.bind(port, ip ); }
    CK_API inline ck_socket accept() { auto r = _sok.accept(); ck_socket s; s._sok = std::move(r); return std::move(s); }

    CK_API inline SOCKET get_sok_id() const { return _sok.get_sok_id(); }

    // ck_socket_io
    CK_API virtual int raw_send( const void* buf, size_t len );

    CK_API virtual int raw_recv( void* buf, size_t len );

    /// ck_stream_io API
    CK_API inline virtual bool valid() const { return _sok.valid(); }

    CK_API virtual void send( const void* buf, size_t len );

    CK_API virtual void recv( void* buf, size_t len );

    CK_API inline void close() { _sok.close(); }

    // ck_pollable
    CK_API inline virtual bool recv_wont_block( uint64_t& millis ) const { return _sok.recv_wont_block(millis); }
    CK_API inline virtual bool send_wont_block( uint64_t& millis ) const { return _sok.send_wont_block(millis); }

    CK_API inline ck_string get_peer_ip() const { return _sok.get_peer_ip(); }
    CK_API inline ck_string get_local_ip() const { return _sok.get_local_ip(); }

    static inline void socket_startup() { ck_raw_socket::socket_startup(); }

private:
    ck_raw_socket _sok;
    uint64_t _ioTimeOut;
};

// ck_buffered_socket is templated on another socket type. The idea is that you can add
// buffering to any underlying socket type.

template<class SOK>
class ck_buffered_socket : public ck_stream_io, public ck_pollable
{
public:
    friend class ::ck_socket_test;

public:
    enum ck_buffered_socket_defaults
    {
        MAX_BACKLOG = 5
    };

    CK_API inline ck_buffered_socket( size_t bufferSize = 4096 ) :
        _sok(),
        _buffer(),
        _bufferOff(0)
    {
        _buffer.reserve(bufferSize);
    }

    CK_API inline ck_buffered_socket( ck_buffered_socket&& obj ) throw() :
        _sok( std::move(obj._sok) ),
        _buffer( std::move(obj._buffer) ),
        _bufferOff( std::move( obj._bufferOff ) )
    {
        obj._bufferOff = 0;
    }

    CK_API ck_buffered_socket( const ck_buffered_socket& ) = delete;

    CK_API inline virtual ~ck_buffered_socket() throw() {}

    CK_API inline ck_buffered_socket& operator = ( ck_buffered_socket&& obj ) throw()
    {
        _sok = std::move(obj._sok);
        _buffer = std::move(obj._buffer);
        _bufferOff = std::move(obj._bufferOff);
        obj._bufferOff = 0;
        return *this;
    }

    CK_API ck_buffered_socket& operator = ( const ck_buffered_socket& ) = delete;

    // Returns a reference to the underlying socket. This is especially useful if the underlying socket
    // type is SSL, in which case we don't have t a full API implemented.
    CK_API SOK& inner() { return _sok; }

    CK_API inline void create( int af ) { _sok.create(af); }

    CK_API inline void connect( const ck_string& host, int port ) { _sok.connect(host, port); }
    CK_API inline void listen( int backlog = MAX_BACKLOG ) { _sok.listen(backlog); }
    CK_API inline void bind( int port, const ck_string& ip = "" ) { _sok.bind(port, ip ); }
    CK_API inline ck_buffered_socket accept() { ck_buffered_socket bs(_buffer.capacity()); auto s = _sok.accept(); bs._sok = std::move(s); return std::move(bs); }

    CK_API inline SOCKET get_sok_id() const { return _sok.get_sok_id(); }

    CK_API bool buffer_recv()
    {
        if( _avail_in_buffer() > 0 )
            return true;

        size_t bufferCapacity = _buffer.capacity();

        _buffer.resize( bufferCapacity );

        int bytesRead = _sok.raw_recv( &_buffer[0], bufferCapacity );

        if( bytesRead >= 0 )
        {
            _buffer.resize( bytesRead );
            _bufferOff = 0;
        }

        return (bytesRead > 0) ? true : false;
    }

    /// ck_stream_io API
    CK_API inline virtual bool valid() const { return _sok.valid(); }

    CK_API virtual void send( const void* buf, size_t len )
    {
        _sok.send( buf, len );
    }

    CK_API virtual void recv( void* buf, size_t len )
    {
        size_t bytesToRecv = len;
        uint8_t* dst = (uint8_t*)buf;

        while( bytesToRecv > 0 )
        {
            size_t avail = _avail_in_buffer();

            if( avail == 0 )
            {
                buffer_recv();
                continue;
            }

            size_t ioSize = (bytesToRecv <= avail) ? bytesToRecv : avail;

            memcpy( dst, &_buffer[_bufferOff], ioSize );
            dst += ioSize;
            _bufferOff += ioSize;
            bytesToRecv -= ioSize;
        }
    }

    CK_API inline void close() { _sok.close(); }

    // ck_pollable
    CK_API inline virtual bool recv_wont_block( uint64_t& millis ) const { return _sok.recv_wont_block(millis); }
    CK_API inline virtual bool send_wont_block( uint64_t& millis ) const { return _sok.send_wont_block(millis); }

    CK_API inline ck_string get_peer_ip() const { return _sok.get_peer_ip(); }
    CK_API inline ck_string get_local_ip() const { return _sok.get_local_ip(); }

    CK_API inline SOK& get_socket() { return _sok; }

    static inline void socket_startup() { ck_raw_socket::socket_startup(); }

private:
    inline size_t _avail_in_buffer() { return _buffer.size() - _bufferOff; }

    SOK _sok;
    std::vector<uint8_t> _buffer;
    size_t _bufferOff;
};

CK_API std::vector<ck_string> ck_resolve( int type, const ck_string& name );

CK_API std::map<std::string,std::vector<ck_string>> ck_get_interface_addresses( int af );

CK_API std::vector<uint8_t> ck_get_hardware_address(const cppkit::ck_string& ifname);

CK_API cppkit::ck_string ck_get_device_uuid(const cppkit::ck_string& ifname);

CK_API uint16_t ck_ntohs(uint16_t x);

CK_API uint16_t ck_htons(uint16_t x);

CK_API uint32_t ck_ntohl(uint32_t x);

CK_API uint32_t ck_htonl(uint32_t x);

CK_API uint64_t ck_ntohll(uint64_t x);

CK_API uint64_t ck_htonll(uint64_t x);

class ck_socket_connect_exception : public ck_exception
{
public:
    CK_API ck_socket_connect_exception(const char* msg, ...);
    CK_API virtual ~ck_socket_connect_exception() throw() {}
};

class ck_socket_exception : public ck_exception
{
public:
    CK_API ck_socket_exception(const char* msg, ...);
    CK_API virtual ~ck_socket_exception() throw() {}
};

}

#endif
