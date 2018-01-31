
#include "framework.h"

#include "cppkit/ck_string.h"

class ck_socket_test : public test_fixture
{
public:

    TEST_SUITE(ck_socket_test);
      TEST(ck_socket_test::test_client_server);
      TEST(ck_socket_test::test_move_constructable);
      TEST(ck_socket_test::test_move_assignable);
      TEST(ck_socket_test::test_close_warm_socket);
      TEST(ck_socket_test::test_wont_block);
      TEST(ck_socket_test::test_buffered_socket);
      TEST(ck_socket_test::test_hardware_address);
    TEST_SUITE_END();

    virtual ~ck_socket_test() throw() {}

    virtual void setup();
    virtual void teardown();

    void test_client_server();
    void test_move_constructable();
    void test_move_assignable();
    void test_close_warm_socket();
    void test_wont_block();
    void test_buffered_socket();
    void test_hardware_address();
};
