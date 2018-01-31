
#include "framework.h"
#include "ck_md5_test.h"
#include "cppkit/ck_md5.h"

using namespace std;
using namespace cppkit;

REGISTER_TEST_FIXTURE(ck_md5_test);

void ck_md5_test::setup()
{
}

void ck_md5_test::teardown()
{
}

void ck_md5_test::test_hash_basic()
{
    ck_md5 md5;
    ck_string msg = "Beneath this mask there is an idea, Mr. Creedy, and ideas are bulletproof.";
    md5.update( (uint8_t*)msg.c_str(), msg.length() );
    md5.finalize();
    ck_string output = md5.get_as_string();
    UT_ASSERT( output == "68cc4c2cbf04714ffd2b4306376410b8" );
}
