
#include "framework.h"
#include "ck_sha1_test.h"
#include "cppkit/ck_sha1.h"

using namespace std;
using namespace cppkit;

REGISTER_TEST_FIXTURE(ck_sha1_test);

void ck_sha1_test::setup()
{
}

void ck_sha1_test::teardown()
{
}

void ck_sha1_test::test_hash_basic()
{
    ck_sha1 sha1;
    ck_string msg = "Beneath this mask there is an idea, Mr. Creedy, and ideas are bulletproof.";
    sha1.update( (uint8_t*)msg.c_str(), msg.length() );
    sha1.finalize();
    ck_string output = sha1.get_as_string();
    printf("%s\n",output.c_str());
    UT_ASSERT( output == "7a454aa45aa178935997fc89ff609ff374f9fff1" );
}
