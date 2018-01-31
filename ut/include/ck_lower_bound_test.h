
#ifndef ck_lower_bound_test_h
#define ck_lower_bound_test_h

#include "framework.h"
#include <vector>

class ck_lower_bound_test : public test_fixture
{
public:
    TEST_SUITE(ck_lower_bound_test);
        TEST(ck_lower_bound_test::test_before);
        TEST(ck_lower_bound_test::test_first_exact);
        TEST(ck_lower_bound_test::test_middle);
        TEST(ck_lower_bound_test::test_middle_exact);
        TEST(ck_lower_bound_test::test_greater);
    TEST_SUITE_END();

    void setup();
    void teardown();

protected:
    void test_before();
    void test_first_exact();
    void test_middle();
    void test_middle_exact();
    void test_greater();

private:
    std::vector<int> _storage;
};

#endif
