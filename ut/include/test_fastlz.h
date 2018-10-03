
#include "framework.h"

class test_fastlz : public test_fixture
{
public:

    RTF_FIXTURE(test_fastlz);
      TEST(test_fastlz::test_compress_basic);
      TEST(test_fastlz::test_compress_bz2);
    RTF_FIXTURE_END();

    virtual ~test_fastlz() throw() {}

    virtual void setup();
    virtual void teardown();

    void test_compress_basic();
    void test_compress_bz2();

};
