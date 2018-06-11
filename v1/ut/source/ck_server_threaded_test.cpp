
#include "framework.h"
#include "ck_server_threaded_test.h"
#include "cppkit/os/ck_time_utils.h"
#include "cppkit/ck_server_threaded.h"
#include "cppkit/ck_socket.h"

using namespace std;
using namespace cppkit;

REGISTER_TEST_FIXTURE(ck_server_threaded_test);

void ck_server_threaded_test::setup()
{
}

void ck_server_threaded_test::teardown()
{
}

void ck_server_threaded_test::test_basic()
{
    int port = ut_next_port();

    thread t([&](){
        ck_server_threaded<ck_socket> s( port, [](ck_buffered_socket<ck_socket>& conn){
            uint32_t inv;
            conn.recv( &inv, 4 );
            ++inv;
            conn.send( &inv, 4 );

            conn.close();
        } );
        s.start();
    });
    t.detach();

    try
    {
        ck_sleep(1);
        ck_socket socket;

        UT_ASSERT_NO_THROW( socket.connect( "127.0.0.1", port ) );

        uint32_t val = 41;
        socket.send( &val, 4 );
        socket.recv( &val, 4 );

        UT_ASSERT( val == 42 );

        socket.close();
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
