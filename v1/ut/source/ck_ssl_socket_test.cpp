
#include "framework.h"
#include "ck_ssl_socket_test.h"
#include "cppkit/ck_ssl_socket.h"
#include "cppkit/os/ck_large_files.h"
#include "cppkit/os/ck_time_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <memory>

using namespace std;
using namespace cppkit;

REGISTER_TEST_FIXTURE(ck_ssl_socket_test);

namespace FOO
{
#include "CACert.cpp"
#include "ServerCRT_1.cpp"
#include "ServerCRT_2.cpp"
#include "PrivateKey_1.cpp"
#include "PrivateKey_2.cpp"
}

void ck_ssl_socket_test::setup()
{
    FILE* outFile = fopen( "CACert.pem", "wb" );
    fwrite( FOO::CACertPEM, 1, sizeof( FOO::CACertPEM ), outFile );
    fclose( outFile );

    outFile = fopen( "ServerCRT1.crt", "wb" );
    fwrite( FOO::ServerCRT_1, 1, sizeof( FOO::ServerCRT_1 ), outFile );
    fclose( outFile );

    outFile = fopen( "ServerCRT2.crt", "wb" );
    fwrite( FOO::ServerCRT_2, 1, sizeof( FOO::ServerCRT_2 ), outFile );
    fclose( outFile );

    outFile = fopen( "PrivateKey1.key", "wb" );
    fwrite( FOO::PrivateKey_1, 1, sizeof( FOO::PrivateKey_1 ), outFile );
    fclose( outFile );

    outFile = fopen( "PrivateKey2.key", "wb" );
    fwrite( FOO::PrivateKey_2, 1, sizeof( FOO::PrivateKey_2 ), outFile );
    fclose( outFile );
}

void ck_ssl_socket_test::teardown()
{
    ck_unlink( "CACert.pem" );
    ck_unlink( "ServerCRT1.crt" );
    ck_unlink( "ServerCRT2.crt" );
    ck_unlink( "PrivateKey1.key" );
    ck_unlink( "PrivateKey2.key" );
}

void ck_ssl_socket_test::test_client_server()
{
    int port = ut_next_port();

    thread t([&](){
        ck_ssl_socket serverSocket;
        serverSocket.use_pem_certificate_file( "ServerCRT1.crt" );
        serverSocket.use_pem_rsa_private_key_file( "PrivateKey1.key" );
        serverSocket.bind( port, "127.0.0.1" );
        serverSocket.listen();

        auto clientSocket = serverSocket.accept();

        uint32_t val = 0;
        clientSocket.recv( &val, 4 );

        val = val + 1;

        clientSocket.send( &val, 4 );
    });
    t.detach();

    ck_usleep( 250000 );

    ck_ssl_socket socket;

    // Self signed cert will not verify...
    UT_ASSERT_NO_THROW( socket.connect( "127.0.0.1", port ) );

    uint32_t val = 41;

    socket.send( &val, 4 );

    socket.recv( &val, 4 );

    UT_ASSERT( val == 42 );
}

void ck_ssl_socket_test::test_move_constructable()
{
    ck_ssl_socket sokA;
    sokA.connect( "www.google.com", 443 );
    ck_string query = "GET /\r\n\r\n";
    ck_ssl_socket sokB = std::move(sokA); // move ctor
    sokB.send( query.c_str(), query.length() );
    char buffer[1024];
    memset( buffer, 0, 1024 );
    sokB.recv( buffer, 1023 );
    sokB.close();
    ck_string response = buffer;
    UT_ASSERT( response.to_lower().contains("google") );
}

void ck_ssl_socket_test::test_move_assignable()
{
    ck_ssl_socket sokA;
    sokA.connect( "www.google.com", 443 );
    ck_string query = "GET /\r\n\r\n";
    ck_ssl_socket sokB;
    sokB = std::move(sokA); // move assignment
    sokB.send( query.c_str(), query.length() );
    char buffer[1024];
    memset( buffer, 0, 1024 );
    sokB.recv( buffer, 1023 );
    sokB.close();
    ck_string response = buffer;
    UT_ASSERT( response.to_lower().contains("google") );
}

void ck_ssl_socket_test::test_close_warm_socket()
{
    int port = ut_next_port();

    thread t([&](){
        ck_ssl_socket serverSocket;

        serverSocket.use_pem_certificate_file( "ServerCRT1.crt" );
        serverSocket.use_pem_rsa_private_key_file( "PrivateKey1.key" );

        serverSocket.bind( port, "127.0.0.1" );
        serverSocket.listen();

        auto clientSocket = serverSocket.accept();

        uint32_t val = 0;
        clientSocket.recv( &val, 4 );

        val = val + 1;

        clientSocket.send( &val, 4 );
    });
    t.detach();

    ck_usleep( 250000 );
    ck_ssl_socket socket;

    // Self signed cert will not verify...
    UT_ASSERT_NO_THROW( socket.connect( "127.0.0.1", port ) );

    uint32_t val = 41;

    socket.send( &val, 4 );

    socket.recv( &val, 4 );

    UT_ASSERT( val == 42 );

    socket.close();

    //This is different than before.  After a close a new _ssl
    //context is created so the socket can be reuse.
    //_ssl is only NULL when destroyed...
    UT_ASSERT( socket._ssl == NULL );
}

void ck_ssl_socket_test::test_two_connects()
{
	try
	{
		int port = ut_next_port();

		thread t([&](){
			ck_ssl_socket serverSocket;

			serverSocket.use_pem_certificate_file("ServerCRT1.crt");
			serverSocket.use_pem_rsa_private_key_file("PrivateKey1.key");

			serverSocket.bind(port, "127.0.0.1");
			serverSocket.listen();

			auto clientSocket = serverSocket.accept();

			uint32_t val = 0;
			clientSocket.recv(&val, 4);

			val = val + 1;

			clientSocket.send(&val, 4);

			clientSocket = serverSocket.accept();

			val = 0;
			clientSocket.recv(&val, 4);

			val = val + 1;

			clientSocket.send(&val, 4);
		});
		t.detach();

		ck_sleep(2);
		ck_ssl_socket socket;

		// Self signed cert will not verify...
		UT_ASSERT_NO_THROW(socket.connect("127.0.0.1", port));

		uint32_t val = 99;

		socket.send(&val, 4);

		socket.recv(&val, 4);

		UT_ASSERT(val == 100);

		socket.close();

		// Self signed cert will not verify...
		UT_ASSERT_NO_THROW(socket.connect("127.0.0.1", port));

		uint32_t val2 = 99;

		socket.send(&val2, 4);

		socket.recv(&val2, 4);

		UT_ASSERT((val + val2) == 200);
	}
	catch (ck_exception& ex)
	{
		ck_string msg = ex.what();
		printf("msg = %s\n", msg.c_str());
	}
}

void ck_ssl_socket_test::test_encrypted_private_key()
{
    int port = ut_next_port();

    thread t([&](){
        auto serverSocket = make_shared<ck_ssl_socket>();

        serverSocket->use_pem_certificate_file( "ServerCRT2.crt" );
        serverSocket->use_pem_rsa_private_key_file( "PrivateKey2.key", "passphrase" );

        UT_ASSERT_NO_THROW( serverSocket->bind( port, "127.0.0.1" ) );
        serverSocket->listen();

        auto clientSocket = serverSocket->accept();

        uint32_t val = 0;
        clientSocket.recv( &val, 4 );

        val = val + 1;

        clientSocket.send( &val, 4 );
    });
    t.detach();

    ck_usleep( 250000 );
    auto socket = make_shared<ck_ssl_socket>();

    // Self signed cert will not verify...
    UT_ASSERT_NO_THROW( socket->connect( "127.0.0.1", port ) );

    uint32_t val = 41;

    socket->send( &val, 4 );

    socket->recv( &val, 4 );

    UT_ASSERT( val == 42 );
}

void ck_ssl_socket_test::test_verify()
{
    ck_ssl_socket socket;

    socket.use_ca_certificate_file( "CACert.pem" );
    socket.verify_connection( true );

    UT_ASSERT_NO_THROW( socket.connect( "www.google.com", 443 ) );

    ck_string msg = "GET /\r\n\r\n";
    socket.send( msg.c_str(), msg.length() );

    char buffer[1024];
    memset( buffer, 0, 1024 );
    socket.recv( buffer, 1023 );

    ck_string response = buffer;

    socket.close();

    UT_ASSERT( response.to_lower().contains("google") );
}

void ck_ssl_socket_test::test_verify_no_pem()
{
    ck_ssl_socket socket;
    socket.verify_connection( true );
    UT_ASSERT_NO_THROW( socket.connect( "www.google.com", 443 ) );
}

void ck_ssl_socket_test::test_buffered_ssl()
{
    ck_buffered_socket<ck_ssl_socket> socket;

    socket.inner().use_ca_certificate_file( "CACert.pem" );
    socket.inner().verify_connection( true );

    UT_ASSERT_NO_THROW( socket.connect( "www.google.com", 443 ) );

    ck_string msg = "GET /\r\n\r\n";
    socket.send( msg.c_str(), msg.length() );

    char buffer[1024];
    memset( buffer, 0, 1024 );

    if( socket.buffer_recv() )
        socket.recv( buffer, 1023 );

    ck_string response = buffer;

    socket.close();

    UT_ASSERT( response.to_lower().contains("google") );
}