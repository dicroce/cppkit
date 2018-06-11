
#include "framework.h"

class ck_server_threaded_test : public test_fixture
{
public:

    TEST_SUITE(ck_server_threaded_test);
      TEST(ck_server_threaded_test::test_basic);
    TEST_SUITE_END();

    virtual ~ck_server_threaded_test() throw() {}

    virtual void setup();
    virtual void teardown();

    void test_basic();
};
