
#include "framework.h"

class test_ck_string : public test_fixture
{
public:

    RTF_FIXTURE(test_ck_string);
      TEST(test_ck_string::test_split);
      TEST(test_ck_string::test_split_none);
      TEST(test_ck_string::test_split_char);
      TEST(test_ck_string::test_split_entire);
      TEST(test_ck_string::test_split_delim_leading_trailing);
      TEST(test_ck_string::test_basic_format);
      TEST(test_ck_string::test_basic_format_buffer);
      TEST(test_ck_string::test_uri_encode);
      TEST(test_ck_string::test_uri_decode);
      TEST(test_ck_string::test_numeric_conversions);
      TEST(test_ck_string::test_base64_encode);
      TEST(test_ck_string::test_base64_decode);
      TEST(test_ck_string::test_strip);
    RTF_FIXTURE_END();

    virtual ~test_ck_string() throw() {}

    virtual void setup();
    virtual void teardown();

    void test_split();
    void test_split_none();
    void test_split_char();
    void test_split_entire();
    void test_split_delim_leading_trailing();

    void test_basic_format();
    void test_basic_format_buffer();

    void test_uri_encode();
    void test_uri_decode();

    void test_numeric_conversions();

    void test_base64_encode();
    void test_base64_decode();

    void test_strip();
};
