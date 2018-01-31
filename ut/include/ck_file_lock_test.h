#ifndef ck_file_lock_test_h
#define ck_file_lock_test_h

#include "framework.h"
#include <stdio.h>

class ck_file_lock_test : public test_fixture
{
public:
    TEST_SUITE(ck_file_lock_test);
        TEST(ck_file_lock_test::test_constructor);
        TEST(ck_file_lock_test::test_exclusive);
        TEST(ck_file_lock_test::test_shared);
    TEST_SUITE_END();

    virtual ~ck_file_lock_test() throw()
    {}

    void setup();
    void teardown();

protected:

    void test_constructor();
    void test_exclusive();
    void test_shared();

private:
    FILE* lockFile;
};

#endif
