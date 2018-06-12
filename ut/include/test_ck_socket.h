
#include "framework.h"

class test_ck_socket : public test_fixture
{
public:
    RTF_FIXTURE(test_ck_socket);
      TEST(test_ck_socket::test_client_server);
      TEST(test_ck_socket::test_move_constructable);
      TEST(test_ck_socket::test_move_assignable);
      TEST(test_ck_socket::test_close_warm_socket);
      TEST(test_ck_socket::test_wont_block);
      TEST(test_ck_socket::test_buffered_socket);
      TEST(test_ck_socket::test_hardware_address);
    RTF_FIXTURE_END();

    virtual ~test_ck_socket() throw() {}

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
