
#include "framework.h"

class ck_sha256_test : public test_fixture
{
public:
    TEST_SUITE(ck_sha256_test);
        TEST(ck_sha256_test::test_hash_basic);
    TEST_SUITE_END();

    virtual ~ck_sha256_test() throw() {}

    virtual void setup();
    virtual void teardown();

    void test_hash_basic();
};
