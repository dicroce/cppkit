
#include "framework.h"

class test_ck_pool : public test_fixture
{
public:
    RTF_FIXTURE(test_ck_pool);
        TEST(test_ck_pool::test_constructor);
        TEST(test_ck_pool::test_get);
        TEST(test_ck_pool::test_deleter_transfer);
        TEST(test_ck_pool::test_empty);
    RTF_FIXTURE_END();

    virtual ~test_ck_pool() throw() {}

    void setup() {}
    void teardown() {}

    void test_constructor();
    void test_get();
    void test_deleter_transfer();
    void test_empty();
};
