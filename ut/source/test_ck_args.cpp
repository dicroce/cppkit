
#include "test_ck_args.h"
#include "cppkit/ck_args.h"

using namespace std;
using namespace cppkit;

REGISTER_TEST_FIXTURE(test_ck_args);

void test_ck_args::setup()
{
}

void test_ck_args::teardown()
{
}

void test_ck_args::test_basic()
{
    {
        char* argv[] = {(char*)"app", (char*)"--infile", (char*)"foo.mp4", (char*)"--outfile", (char*)"bar.mp4"};
        int argc = 5;

        auto arguments = args::parse_arguments(argc, argv);

        string val;
        args::check_argument(arguments, "--infile", val);
        RTF_ASSERT(val == "foo.mp4");

        args::check_argument(arguments, "--outfile", val);
        RTF_ASSERT(val == "bar.mp4");
    }

    {
        // check no argument option before argument option
        char* argv[] = {(char*)"app", (char*)"--quick", (char*)"--outfile", (char*)"bar.mp4"};
        int argc = 4;

        auto arguments = args::parse_arguments(argc, argv);
        
        RTF_ASSERT(args::check_argument(arguments, "--quick"));

        string val;
        args::check_argument(arguments, "--outfile", val);
        RTF_ASSERT(val == "bar.mp4");
    }

}

void test_ck_args::test_get_all()
{
    char* argv[] = {(char*)"app", (char*)"--file", (char*)"foo.mp4", (char*)"--file", (char*)"bar.mp4"};
    int argc = 5;

    auto arguments = args::parse_arguments(argc, argv);

    auto files = args::get_all(arguments, "--file");
    RTF_ASSERT(files.size() == 2);
    RTF_ASSERT(files[0] == "foo.mp4");
    RTF_ASSERT(files[1] == "bar.mp4");
}
