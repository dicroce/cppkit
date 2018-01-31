
#include "framework.h"

class ck_lru_cache_test : public test_fixture
{
public:
    TEST_SUITE(ck_lru_cache_test);
        TEST(ck_lru_cache_test::test_constructor);
        TEST(ck_lru_cache_test::test_get);
        TEST(ck_lru_cache_test::test_miss);
    TEST_SUITE_END();

    virtual ~ck_lru_cache_test() throw() {}

    void setup() {}
    void teardown() {}

    void test_constructor();
    void test_get();
    void test_miss();
};
