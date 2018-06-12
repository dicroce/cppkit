
#include "test_ck_string_utils.h"
#include "cppkit/ck_string_utils.h"
#include "cppkit/ck_exception.h"
#include <limits.h>
#include <string.h>

using namespace std;
using namespace cppkit;
using namespace cppkit::ck_string_utils;

REGISTER_TEST_FIXTURE(test_ck_string);

void test_ck_string::setup()
{
}

void test_ck_string::teardown()
{
}

void test_ck_string::test_split()
{
    auto parts = ck_string_utils::split("This is a test string.", "test");
    RTF_ASSERT(parts.size() == 2);
    RTF_ASSERT(parts[0] == "This is a ");
    RTF_ASSERT(parts[1] == " string.");
}

void test_ck_string::test_split_none()
{
    auto parts = ck_string_utils::split("This is a test string.", "BOOM");
    RTF_ASSERT(parts.size() == 1);
    RTF_ASSERT(parts[0] == "This is a test string.");
}

void test_ck_string::test_split_char()
{
    auto parts = ck_string_utils::split("This is a test string.", ' ');
    RTF_ASSERT(parts.size() == 5);
    RTF_ASSERT(parts[0] == "This");
    RTF_ASSERT(parts[1] == "is");
    RTF_ASSERT(parts[2] == "a");
    RTF_ASSERT(parts[3] == "test");
    RTF_ASSERT(parts[4] == "string.");
}

void test_ck_string::test_split_entire()
{
    auto parts = ck_string_utils::split("foo-bar-baz", "foo-bar-baz");
    RTF_ASSERT(parts.size() == 0);
}

void test_ck_string::test_split_delim_leading_trailing()
{
    auto parts = ck_string_utils::split("---This---is---a---test---string.", "---");
    RTF_ASSERT(parts.size() == 5);
    RTF_ASSERT(parts[0] == "This");
    RTF_ASSERT(parts[1] == "is");
    RTF_ASSERT(parts[2] == "a");
    RTF_ASSERT(parts[3] == "test");
    RTF_ASSERT(parts[4] == "string.");

    parts = ck_string_utils::split("This---is---a---test---string.---", "---");
    RTF_ASSERT(parts.size() == 5);
    RTF_ASSERT(parts[0] == "This");
    RTF_ASSERT(parts[1] == "is");
    RTF_ASSERT(parts[2] == "a");
    RTF_ASSERT(parts[3] == "test");
    RTF_ASSERT(parts[4] == "string.");

    parts = ck_string_utils::split("---This---is---a---test---string.---", "---");
    RTF_ASSERT(parts.size() == 5);
    RTF_ASSERT(parts[0] == "This");
    RTF_ASSERT(parts[1] == "is");
    RTF_ASSERT(parts[2] == "a");
    RTF_ASSERT(parts[3] == "test");
    RTF_ASSERT(parts[4] == "string.");
}

void test_ck_string::test_basic_format()
{
    auto str = ck_string_utils::format( "I am %d years old and my name is %s.", 41, "Tony");
    RTF_ASSERT(str == "I am 41 years old and my name is Tony.");
}

void test_ck_string::test_basic_format_buffer()
{
    char buffer[4096];
    auto truncated = ck_string_utils::format_buffer(buffer, 4096, "I am %d years old and my name is %s.", 41, "Tony");
    RTF_ASSERT(truncated == false);
    RTF_ASSERT(string(buffer) == "I am 41 years old and my name is Tony.");
}

void test_ck_string::test_uri_encode()
{
    {
        const string inputStr = "a & Z+ 26";
        const string encoded = uri_encode(inputStr);
        RTF_ASSERT(encoded == "a%20%26%20Z%2B%2026");
    }

    RTF_ASSERT(uri_encode(string("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-._~")) == string("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-._~"));

    for(char i = 1; i < '-'; ++i)
        RTF_ASSERT(uri_encode(format("%c", i)) == format("%%%02X", i));

    // + 1 is '.'
    for(char i = '-' + 2 ; i < '0'; ++i)
        RTF_ASSERT(uri_encode(format("%c", i)) == format("%%%02X", i));

    for(char i = '9' + 1 + 1; i < 'A'; ++i)
        RTF_ASSERT(uri_encode(format("%c", i)) == format("%%%02X", i));

    for(char i = 'Z' + 1; i < '_'; ++i)
        RTF_ASSERT(uri_encode(format("%c", i)) == format("%%%02X", i));

    for(char i = '_' + 1; i < 'a'; ++i)
        RTF_ASSERT(uri_encode(format("%c", i)) == format("%%%02X", i));

    for(char i = 'z' + 1; i < '~'; ++i)
        RTF_ASSERT(uri_encode(format("%c", i)) == format("%%%02X", i));

    RTF_ASSERT(uri_encode(format("%c", 127)) == "%7F");
}

void test_ck_string::test_uri_decode()
{
    {
        const string input_str = "aBcdEfGh+%26%20Z%2B+26%26";
        const string decoded = uri_decode(input_str);
        RTF_ASSERT(decoded == "aBcdEfGh & Z+ 26&");

        string malformed_str = "a+%26+Z%2+26%26";
        RTF_ASSERT_THROWS(uri_decode(malformed_str), cppkit::ck_exception);
    }
    {
        string input_str = "aBcdEfGh+%26%20Z%2B+26%26";
        input_str = uri_decode(input_str);
        RTF_ASSERT(input_str == "aBcdEfGh & Z+ 26&");

        string malformed_str = "a+%26+Z%2+26%26";
        RTF_ASSERT_THROWS(uri_decode(malformed_str), cppkit::ck_exception);
    }
    {
        string input_str = "%2F%2f%2d%2D";
        input_str = uri_decode(input_str);
        RTF_ASSERT(input_str == "//--");
    }

    RTF_ASSERT(uri_decode(string("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-._~")) ==
               string("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-._~"));

    //Wraps to -128 after incrementing past 127.
    for(char i = 1; i > 0; ++i)
        RTF_ASSERT(uri_decode(format("%%%02X", i)) == format("%c", i));

    for(char i = 1; i > 0; ++i)
        RTF_ASSERT(uri_decode(format("%%%02x", i)) == format("%c", i));
}

void test_ck_string::test_numeric_conversions()
{
    {
        // int
        RTF_ASSERT(ck_string_utils::s_to_int("-32768") == -32768);
        RTF_ASSERT(ck_string_utils::int_to_s(-32768) == "-32768");
        RTF_ASSERT(ck_string_utils::s_to_int("32767") == 32767);
        RTF_ASSERT(ck_string_utils::int_to_s(32767) == "32767");
    }

    {
        // 8 bit
        RTF_ASSERT(ck_string_utils::s_to_uint8("0") == 0);
        RTF_ASSERT(ck_string_utils::uint8_to_s(0) == "0");
        RTF_ASSERT(ck_string_utils::s_to_uint8("255") == 255);
        RTF_ASSERT(ck_string_utils::uint8_to_s(255) == "255");
        RTF_ASSERT(ck_string_utils::s_to_int8("-128") == -128);
        RTF_ASSERT(ck_string_utils::int8_to_s(-128) == "-128");
        RTF_ASSERT(ck_string_utils::s_to_int8("127") == 127);
        RTF_ASSERT(ck_string_utils::int8_to_s(127) == "127");
    }

    {
        // 16 bit
        RTF_ASSERT(ck_string_utils::s_to_uint16("0") == 0);
        RTF_ASSERT(ck_string_utils::uint16_to_s(0) == "0");
        RTF_ASSERT(ck_string_utils::s_to_uint16("65535") == 65535);
        RTF_ASSERT(ck_string_utils::uint16_to_s(65535) == "65535");
        RTF_ASSERT(ck_string_utils::s_to_int16("-32768") == -32768);
        RTF_ASSERT(ck_string_utils::int16_to_s(-32768) == "-32768");
        RTF_ASSERT(ck_string_utils::s_to_int16("32767") == 32767);
        RTF_ASSERT(ck_string_utils::int16_to_s(32767) == "32767");
    }

    {
        // 32 bit
        RTF_ASSERT(ck_string_utils::s_to_uint32("0") == 0);
        RTF_ASSERT(ck_string_utils::uint32_to_s(0) == "0");
        RTF_ASSERT(ck_string_utils::s_to_uint32("4294967295") == 4294967295);
        RTF_ASSERT(ck_string_utils::uint32_to_s(4294967295) == "4294967295");
        RTF_ASSERT(ck_string_utils::s_to_int32("-2147483648") == -2147483648);
        RTF_ASSERT(ck_string_utils::int32_to_s(-2147483648) == "-2147483648");
        RTF_ASSERT(ck_string_utils::s_to_int32("2147483647") == 2147483647);
        RTF_ASSERT(ck_string_utils::int32_to_s(2147483647) == "2147483647");
    }

    {
        // 64 bit
        RTF_ASSERT(ck_string_utils::s_to_uint64("0") == 0);
        RTF_ASSERT(ck_string_utils::uint64_to_s(0) == "0");
        RTF_ASSERT(ck_string_utils::s_to_uint64("18446744073709551615") == 18446744073709551615LLU);
        RTF_ASSERT(ck_string_utils::uint64_to_s(18446744073709551615LLU) == "18446744073709551615");
        RTF_ASSERT(ck_string_utils::s_to_int64("-9223372036854775808") == LONG_LONG_MIN);
        RTF_ASSERT(ck_string_utils::int64_to_s(LONG_LONG_MIN) == "-9223372036854775808");
        RTF_ASSERT(ck_string_utils::s_to_int64("9223372036854775807") == LONG_LONG_MAX);
        RTF_ASSERT(ck_string_utils::int64_to_s(LONG_LONG_MAX) == "9223372036854775807");
    }
}

void test_ck_string::test_base64_encode()
{
    uint8_t buf[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    const string encodedData = ck_string_utils::to_base64(&buf[0], sizeof(buf));

    RTF_ASSERT(encodedData == "AQIDBAUGBwgJCg==");
}

void test_ck_string::test_base64_decode()
{
    uint8_t buf[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    const string encodedData = "AQIDBAUGBwgJCg==";
    auto decoded = ck_string_utils::from_base64(encodedData);

    RTF_ASSERT(memcmp(buf, &decoded[0], 10) == 0);
}
