
#include "framework.h"

class ck_sha1_test : public test_fixture
{
public:
    TEST_SUITE(ck_sha1_test);
        TEST(ck_sha1_test::test_hash_basic);
    TEST_SUITE_END();

    virtual ~ck_sha1_test() throw() {}

    virtual void setup();
    virtual void teardown();

    void test_hash_basic();
};
