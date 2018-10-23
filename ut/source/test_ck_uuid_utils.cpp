
#include "test_ck_uuid_utils.h"
#include "cppkit/ck_uuid_utils.h"
#include "cppkit/ck_string_utils.h"
#include <numeric>
#include <string.h>

using namespace std;
using namespace cppkit;

REGISTER_TEST_FIXTURE(test_ck_uuid);

void test_ck_uuid::setup()
{
}

void test_ck_uuid::teardown()
{
}

void test_ck_uuid::test_generate()
{
    ck_uuid buffer;
    memset(&buffer[0], 0, 16);
    buffer = ck_uuid_utils::generate();
    
    RTF_ASSERT(std::accumulate(&buffer[0], &buffer[16], 0) != 0);

    RTF_ASSERT(ck_uuid_utils::generate_s().length() == 36);
}

void test_ck_uuid::test_uuid_to_s()
{
    uint8_t buffer[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    auto h = ck_uuid_utils::uuid_to_s(&buffer[0]);
    RTF_ASSERT(ck_string_utils::to_lower(ck_uuid_utils::uuid_to_s(&buffer[0])) == "00010203-0405-0607-0809-0a0b0c0d0e0f");
}

void test_ck_uuid::test_s_to_uuid()
{
    uint8_t buffer[16];
    memset(&buffer[0], 0, 16);
    ck_uuid_utils::s_to_uuid("e158d3a3-bacd-45cb-92ea-4db749fe026c", buffer);
    RTF_ASSERT(buffer[0] == 0xe1);
    RTF_ASSERT(buffer[1] == 0x58);
    RTF_ASSERT(buffer[2] == 0xd3);
    RTF_ASSERT(buffer[3] == 0xa3);
    RTF_ASSERT(buffer[4] == 0xba);
    RTF_ASSERT(buffer[5] == 0xcd);
    RTF_ASSERT(buffer[6] == 0x45);
    RTF_ASSERT(buffer[7] == 0xcb);
    RTF_ASSERT(buffer[8] == 0x92);
    RTF_ASSERT(buffer[9] == 0xea);
    RTF_ASSERT(buffer[10] == 0x4d);
    RTF_ASSERT(buffer[11] == 0xb7);
    RTF_ASSERT(buffer[12] == 0x49);
    RTF_ASSERT(buffer[13] == 0xfe);
    RTF_ASSERT(buffer[14] == 0x02);
    RTF_ASSERT(buffer[15] == 0x6c);    
}

void test_ck_uuid::test_cmp()
{
    {
        uint8_t bufferA[16];
        memset(&bufferA[0], 0, 16);
        ck_uuid_utils::s_to_uuid("22b11978-dd19-4fa7-864c-84bd7ca402d2", bufferA);

        uint8_t bufferB[16];
        memset(&bufferB[0], 0, 16);
        ck_uuid_utils::s_to_uuid("feac6e8c-187b-4520-b410-b19269797fc7", bufferB);

        RTF_ASSERT(ck_uuid_utils::uuid_cmp(&bufferA[0], &bufferB[0]) == -1);
    }

    {
        uint8_t bufferA[16];
        memset(&bufferA[0], 0, 16);
        ck_uuid_utils::s_to_uuid("22b11978-dd19-4fa7-864c-84bd7ca402d2", bufferA);

        uint8_t bufferB[16];
        memset(&bufferB[0], 0, 16);
        ck_uuid_utils::s_to_uuid("22b11978-dd19-4fa7-864c-84bd7ca402d2", bufferB);

        RTF_ASSERT(ck_uuid_utils::uuid_cmp(&bufferA[0], &bufferB[0]) == 0);
    }

    {
        uint8_t bufferA[16];
        memset(&bufferA[0], 0, 16);
        ck_uuid_utils::s_to_uuid("feac6e8c-187b-4520-b410-b19269797fc7", bufferA);

        uint8_t bufferB[16];
        memset(&bufferB[0], 0, 16);
        ck_uuid_utils::s_to_uuid("22b11978-dd19-4fa7-864c-84bd7ca402d2", bufferB);

        RTF_ASSERT(ck_uuid_utils::uuid_cmp(&bufferA[0], &bufferB[0]) == 1);
    }
}