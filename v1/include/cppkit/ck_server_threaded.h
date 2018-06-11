
#ifndef cppkit_server_threaded_h
#define cppkit_server_threaded_h

#include "cppkit/ck_socket.h"
#include "cppkit/ck_ssl_socket.h"
#include "cppkit/ck_socket_address.h"
#include "cppkit/ck_string.h"
#include "cppkit/os/ck_platform.h"
#include <mutex>
#include <thread>
#include <functional>

namespace cppkit
{

template<class SOK_T>
class ck_server_threaded
{
private:
    struct conn_context
    {
        bool done {false};
        ck_buffered_socket<SOK_T> connected;
        std::thread th;
    };

public:
    CK_API ck_server_threaded( int port, std::function<void(ck_buffered_socket<SOK_T>& conn)> connCB, const ck_string& sockAddr = ck_string() ) :
        _bufferedServerSocket(),
        _port( port ),
        _connCB( connCB ),
        _sockAddr( sockAddr ),
        _connectedContexts(),
        _running( false )
    {
    }

    CK_API virtual ~ck_server_threaded() throw()
    {
        stop();

        for( const auto& c : _connectedContexts )
        {
            c->done = true;
            c->connected.close();
            c->th.join();
        }
    }

    CK_API void stop()
    {
        if( _running )
        {
            _running = false;

            FULL_MEM_BARRIER();

            SOK_T sok;
            sok.connect( (_sockAddr.empty() || _sockAddr == cppkit::ip4_addr_any) ? "127.0.0.1" : _sockAddr, _port );
        }
    }

    CK_API void start()
    {
        try
        {
            _configure_server_socket();
        }
        catch( std::exception& ex )
        {
            CK_LOG_NOTICE("Exception (%s) caught while initializing ck_server_threaded. Exiting.", ex.what());
            return;
        }
        catch( ... )
        {
            CK_LOG_NOTICE("Unknown exception caught while initializing ck_server_threaded. Exiting.");
            return;
        }

        _running = true;

        while( _running )
        {
            try
            {
                auto cc = std::make_shared<struct conn_context>();

                cc->connected = _bufferedServerSocket.accept();

                if( !_running )
                    continue;

                if( cc->connected.buffer_recv() )
                {
                    _connectedContexts.remove_if( []( const std::shared_ptr<struct conn_context>& context )->bool {
                        if( context->done )
                        {
                            context->th.join();
                            return true;
                        }
                        return false;
                    });

                    cc->done = false;

                    FULL_MEM_BARRIER();
                    
                    cc->th = std::thread( &ck_server_threaded::_thread_start, this, cc );

                    _connectedContexts.push_back( cc );
                }
            }
            catch( std::exception& ex )
            {
                CK_LOG_NOTICE("Exception (%s) occured while responding to connection.",ex.what());
            }
            catch( ... )
            {
                CK_LOG_NOTICE("An unknown exception has occurred while responding to connection.");
            }
        }
    }

    CK_API bool started() const { return _running; }

    CK_API SOK_T& get_socket() { return _bufferedServerSocket.get_socket(); }

private:
    void _configure_server_socket()
    {
        if( _sockAddr.empty() )
            _bufferedServerSocket.bind( _port );
        else _bufferedServerSocket.bind( _port, _sockAddr );

        _bufferedServerSocket.listen();
    }

    void _thread_start( std::shared_ptr<struct conn_context> cc )
    {
        try
        {
            _connCB( cc->connected );
            cc->done = true;
        }
        catch(...)
        {
            if( !cc->done )
                throw;
        }
    }

    ck_buffered_socket<SOK_T> _bufferedServerSocket;
    int _port;
    std::function<void(ck_buffered_socket<SOK_T>& conn)> _connCB;
    ck_string _sockAddr;
    std::list<std::shared_ptr<struct conn_context>> _connectedContexts;
    bool _running;
};

}

#endif
