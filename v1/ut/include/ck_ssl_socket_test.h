
#include "framework.h"

class ck_ssl_socket_test : public test_fixture
{
public:

    TEST_SUITE(ck_ssl_socket_test);
        TEST(ck_ssl_socket_test::test_client_server);
        TEST(ck_ssl_socket_test::test_move_constructable);
        TEST(ck_ssl_socket_test::test_move_assignable);
        TEST(ck_ssl_socket_test::test_close_warm_socket);
        TEST(ck_ssl_socket_test::test_two_connects);
        TEST(ck_ssl_socket_test::test_encrypted_private_key);
        TEST(ck_ssl_socket_test::test_verify);
        TEST(ck_ssl_socket_test::test_verify_no_pem);
        TEST(ck_ssl_socket_test::test_buffered_ssl);
    TEST_SUITE_END();

    virtual ~ck_ssl_socket_test() throw() {}

    virtual void setup();
    virtual void teardown();

    void test_client_server();
    void test_move_constructable();
    void test_move_assignable();
    void test_close_warm_socket();
    void test_two_connects();
    void test_encrypted_private_key();
    void test_verify();
    void test_verify_no_pem();
    void test_buffered_ssl();
};
