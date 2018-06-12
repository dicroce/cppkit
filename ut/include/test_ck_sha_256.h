
#include "framework.h"

class test_ck_sha_256 : public test_fixture
{
public:

    RTF_FIXTURE(test_ck_sha_256);
      TEST(test_ck_sha_256::test_hash_basic);
    RTF_FIXTURE_END();

    virtual ~test_ck_sha_256() throw() {}

    virtual void setup();
    virtual void teardown();

    void test_hash_basic();
};
