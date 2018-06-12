
#include "framework.h"

class test_ck_file : public test_fixture
{
public:

    RTF_FIXTURE(test_ck_file);
      TEST(test_ck_file::test_conversion_op);
      TEST(test_ck_file::test_stat);
      TEST(test_ck_file::test_path_ctor);
      TEST(test_ck_file::test_open_path);
      TEST(test_ck_file::test_path_iter);
      TEST(test_ck_file::test_file_size);
    RTF_FIXTURE_END();

    virtual ~test_ck_file() throw() {}

    virtual void setup();
    virtual void teardown();

    void test_conversion_op();
    void test_stat();
    void test_path_ctor();
    void test_open_path();
    void test_path_iter();
    void test_file_size();
};
