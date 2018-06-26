
#include "framework.h"

class test_ck_args : public test_fixture
{
public:

    RTF_FIXTURE(test_ck_args);
      TEST(test_ck_args::test_basic);
    RTF_FIXTURE_END();

    virtual ~test_ck_args() throw() {}

    virtual void setup();
    virtual void teardown();

    void test_basic();
};
