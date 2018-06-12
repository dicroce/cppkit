
#include "framework.h"
#include "test_ck_server_threaded.h"
#include "cppkit/ck_server_threaded.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;
using namespace cppkit;

REGISTER_TEST_FIXTURE(test_ck_server_threaded);

void test_ck_server_threaded::setup()
{
}

void test_ck_server_threaded::teardown()
{
}

void test_ck_server_threaded::test_basic()
{
    int port = RTF_NEXT_PORT();

    ck_server_threaded<ck_socket> s( port, [](ck_buffered_socket<ck_socket>& conn){
        uint32_t inv;
        conn.recv( &inv, 4 );
        ++inv;
        conn.send( &inv, 4 );

        conn.close();
    } );

    thread t([&](){
        s.start();
    });

    try
    {
        usleep(1000000);
        ck_socket socket;

        RTF_ASSERT_NO_THROW( socket.connect( "127.0.0.1", port ) );

        uint32_t val = 41;
        socket.send( &val, 4 );
        socket.recv( &val, 4 );

        RTF_ASSERT( val == 42 );

        socket.close();

        s.stop();
        t.join();
    }
    catch(ck_exception& ex)
    {
        printf("%s\n",ex.what());
    }
    catch(exception& ex)
    {
        printf("%s\n",ex.what());
    }
}