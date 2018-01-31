
#include "framework.h"

class ck_md5_test : public test_fixture
{
public:
    TEST_SUITE(ck_md5_test);
        TEST(ck_md5_test::test_hash_basic);
    TEST_SUITE_END();

    virtual ~ck_md5_test() throw() {}

    virtual void setup();
    virtual void teardown();

    void test_hash_basic();
};
