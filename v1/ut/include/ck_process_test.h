
#include "framework.h"

class ck_process_test : public test_fixture
{
public:
    TEST_SUITE(ck_process_test);
        TEST(ck_process_test::test_basic_process);
        TEST(ck_process_test::test_enumerate_child_processes);
        TEST(ck_process_test::test_get_processes_for_module);
        TEST(ck_process_test::test_wait_fock_time_utilsout);
        TEST(ck_process_test::test_wait_for_exited);
        TEST(ck_process_test::test_kill);
        TEST(ck_process_test::test_read_stdout);
    TEST_SUITE_END();

    virtual ~ck_process_test() throw() {}

    virtual void setup();
    virtual void teardown();

    void test_basic_process();
    void test_enumerate_child_processes();
    void test_get_processes_for_module();
    void test_wait_fock_time_utilsout();
    void test_wait_for_exited();
    void test_kill();
    void test_read_stdout();
};
