
#include "framework.h"
#include "test_ck_socket.h"
#include "cppkit/ck_socket.h"
#include "cppkit/ck_string_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;
using namespace cppkit;

REGISTER_TEST_FIXTURE(test_ck_socket);

void test_ck_socket::setup()
{
}

void test_ck_socket::teardown()
{
}

void test_ck_socket::test_client_server()
{
    try
    {
        const vector<string> addrs = { "127.0.0.1", "localhost", "::1" };

        int port = RTF_NEXT_PORT();

        for( auto addr : addrs )
        {
            thread t([&](){
                ck_socket server_sok;
                server_sok.bind( port, addr );
                server_sok.listen();
                auto connected = server_sok.accept();
                unsigned int val = 0;
                connected.recv(&val, 4);
                val+=1;
                connected.send(&val, 4);
            });

            usleep( 250000 );

            unsigned int val = 41;
            ck_socket client_sok;

            client_sok.connect(addr, port);

            // Test get_peer_ip()
            RTF_ASSERT_NO_THROW( client_sok.get_peer_ip() );
            RTF_ASSERT( client_sok.get_peer_ip() != "0.0.0.0" );

            // Test get_local_ip()
            RTF_ASSERT_NO_THROW( client_sok.get_local_ip() );
            RTF_ASSERT( client_sok.get_local_ip() != "0.0.0.0" );

            client_sok.send( &val, 4 );
            client_sok.recv( &val, 4 );

            RTF_ASSERT( val == 42 );

            t.join();
        }
    }
    catch(...)
    {
        printf("CAUGHT SOMETHING!\n");
    }
}

void test_ck_socket::test_move_constructable()
{
    try
    {
        ck_socket sokA;
        sokA.connect( "www.google.com", 80 );
        string query = "GET /\r\n\r\n";
        ck_socket sokB = std::move(sokA); // move ctor
        sokB.send( query.c_str(), query.length() );
        char buffer[1024];
        memset( buffer, 0, 1024 );
        sokB.recv( buffer, 1023 );
        sokB.close();
        string response = buffer;
        RTF_ASSERT( ck_string_utils::contains(response, "google") );
        RTF_ASSERT( sokA._sok._sok == -1 );
    }
    catch(const ck_exception& ex)
    {
        printf("SOMETHING THROW IN test_move_constructable!\n");
    }
}

void test_ck_socket::test_move_assignable()
{
    ck_socket sokA;
    sokA.connect( "www.google.com", 80 );
    string query = "GET /\r\n\r\n";
    ck_socket sokB;
    sokB = std::move(sokA); // move assignment
    sokB.send( query.c_str(), query.length() );
    char buffer[1024];
    memset( buffer, 0, 1024 );
    sokB.recv( buffer, 1023 );
    sokB.close();
    string response = buffer;
    RTF_ASSERT( ck_string_utils::contains(response, "google") );
    RTF_ASSERT( sokA._sok._sok == -1 );
}

void test_ck_socket::test_close_warm_socket()
{
    int port = RTF_NEXT_PORT();

    thread t([&](){
        ck_socket serverSocket;

        serverSocket.bind( port, "127.0.0.1" );

        serverSocket.listen();

        auto clientSocket = serverSocket.accept();

        uint32_t val = 0;
        clientSocket.recv( &val, 4 );

        val = val + 1;

        clientSocket.send( &val, 4 );
    });
    t.detach();

    usleep( 250000 );
    ck_socket socket;

    RTF_ASSERT_NO_THROW( socket.connect( "127.0.0.1", port ) );

    uint32_t val = 41;

    socket.send( &val, 4 );

    socket.recv( &val, 4 );

    RTF_ASSERT( val == 42 );

    socket.close();

    RTF_ASSERT( socket._sok._sok == -1 );
}

void test_ck_socket::test_wont_block()
{
    int port = RTF_NEXT_PORT();

    thread t([&](){
        ck_socket server_sok;
        server_sok.bind( port );
        server_sok.listen();
        auto connected = server_sok.accept();
        unsigned int val = 13;
        uint64_t timeout = 5000;
        if( connected.wait_till_send_wont_block( timeout ) )
            connected.send( &val, 4 );
    });
    t.detach();

    usleep( 250000 );

    bool read_thirteen = false;

    ck_socket client_sok;
    client_sok.connect( "127.0.0.1", port );
    uint64_t timeout = 5000;
    if( client_sok.wait_till_recv_wont_block( timeout ) )
    {
        unsigned int val = 0;
        client_sok.recv( &val, 4 );
        if( val == 13 )
            read_thirteen = true;
    }
    
    RTF_ASSERT( read_thirteen == true );
}

void test_ck_socket::test_buffered_socket()
{
    ck_buffered_socket<ck_socket> bufSok( 4096 );
    bufSok.connect( "www.google.com", 80 );

    string query = "GET /\r\n\r\n";
    bufSok.send( query.c_str(), query.length() );

    char buffer[1024];
    memset( buffer, 0, 1024 );
    bufSok.recv( buffer, 1023 );

    bufSok.close();
    string response = buffer;
    RTF_ASSERT( ck_string_utils::contains(response, "google") );
}

void test_ck_socket::test_hardware_address()
{
//    auto addr = ck_get_device_uuid("lo");
//    printf("device_id: %s\n",addr.c_str());

//    auto hwaddr = ck_get_hardware_address("enp0s3");
//    printf("hardware address: ");
//    for(int i=0; i < 6; ++i)
//        printf("%02x",hwaddr[i]);
//    printf("\n");
}
