
#include "framework.h"

class test_ck_uuid : public test_fixture
{
public:

    RTF_FIXTURE(test_ck_uuid);
      TEST(test_ck_uuid::test_generate);
      TEST(test_ck_uuid::test_uuid_to_s);
      TEST(test_ck_uuid::test_s_to_uuid);
    RTF_FIXTURE_END();

    virtual ~test_ck_uuid() throw() {}

    virtual void setup();
    virtual void teardown();

    void test_generate();
    void test_uuid_to_s();
    void test_s_to_uuid();
};
