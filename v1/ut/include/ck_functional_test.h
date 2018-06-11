#ifndef ck_functional_test_h
#define ck_functional_test_h

#include "framework.h"

class ck_functional_test : public test_fixture
{
public:
    TEST_SUITE(ck_functional_test);
        TEST(ck_functional_test::test_basic_fmap);
        TEST(ck_functional_test::test_basic_filter);
        TEST(ck_functional_test::test_basic_reduce);
        TEST(ck_functional_test::test_combined);
        TEST(ck_functional_test::test_set_diff);
        TEST(ck_functional_test::test_erase_if);
        TEST(ck_functional_test::test_complicate_erase);
    TEST_SUITE_END();

    virtual ~ck_functional_test() throw() {}

    void setup() {}
    void teardown() {}

protected:
    void test_basic_fmap();
    void test_basic_filter();
    void test_basic_reduce();
    void test_combined();
    void test_set_diff();
    void test_erase_if();
    void test_complicate_erase();
};

#endif
