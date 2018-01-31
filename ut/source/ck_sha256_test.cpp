
#include "framework.h"
#include "ck_sha256_test.h"
#include "cppkit/ck_sha256.h"

using namespace std;
using namespace cppkit;

REGISTER_TEST_FIXTURE(ck_sha256_test);

void ck_sha256_test::setup()
{
}

void ck_sha256_test::teardown()
{
}

void ck_sha256_test::test_hash_basic()
{
    ck_sha256 sha256;
    ck_string msg = "Beneath this mask there is an idea, Mr. Creedy, and ideas are bulletproof.";
    sha256.update( (uint8_t*)msg.c_str(), msg.length() );
    sha256.finalize();
    ck_string output = sha256.get_as_string();
    UT_ASSERT( output == "ddeb9f4cbc687d0a49d0ae60a683e0b62d3e4e27d91404690d37690e890f1d5c" );
}
