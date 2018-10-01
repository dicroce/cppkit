
#include "framework.h"

class test_fastlz : public test_fixture
{
public:

    RTF_FIXTURE(test_fastlz);
      TEST(test_fastlz::test_compress_basic);
    RTF_FIXTURE_END();

    virtual ~test_fastlz() throw() {}

    virtual void setup();
    virtual void teardown();

    void test_compress_basic();
};
