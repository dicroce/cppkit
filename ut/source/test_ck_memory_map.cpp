
#include "test_ck_memory_map.h"
#include "cppkit/ck_memory_map.h"
#include "cppkit/ck_file.h"
#include <cstring>

using namespace std;
using namespace cppkit;

static const uint32_t FILE_SIZE = (1024*1024);

REGISTER_TEST_FIXTURE(test_ck_memory_map);

#define UNLINK(a) unlink(a)

void test_ck_memory_map::setup()
{
    FILE* outFile = fopen("data", "w+b");

    uint8_t buffer[4096];

    uint32_t blocksToWrite = 256;
    while(blocksToWrite > 0)
    {
        memset(buffer, (uint8_t)(blocksToWrite-1), 4096);
        blocksToWrite -= fwrite(buffer, 4096, 1, outFile);
    }

    fclose(outFile);
}

void test_ck_memory_map::teardown()
{
    UNLINK("data");
}

void test_ck_memory_map::test_file_mapping()
{
    FILE* dataFile = fopen("data", "r+b");

    {
        ck_memory_map mm(fileno(dataFile),
                          0,
                          FILE_SIZE,
                          ck_memory_map::MM_PROT_READ | ck_memory_map::MM_PROT_WRITE,
                          ck_memory_map::MM_TYPE_FILE | ck_memory_map::MM_SHARED);

        auto val = mm.map();

        RTF_ASSERT(val != NULL);

        uint8_t expectedValue = 255;

        for(int i = 0; i < 256; i++)
        {
            for(int ii = 0; ii < 4096; ii++)
            {
                RTF_ASSERT(*val++ == expectedValue);
            }

            expectedValue--;
        }
    }

    fclose(dataFile);
}

void test_ck_memory_map::test_persist_to_disk()
{
    // this test maps the data file prepared in setup, and then sets all of its bytes to 0.
    // It then closes the mapping and the file.
    // It then re-opens the file and using fread() verifies that the file contains all 0 now.

    FILE* dataFile = fopen("data", "r+b");

    {
        ck_memory_map mm(fileno(dataFile),
                          0,
                          FILE_SIZE,
                          ck_memory_map::MM_PROT_READ | ck_memory_map::MM_PROT_WRITE,
                          ck_memory_map::MM_TYPE_FILE | ck_memory_map::MM_SHARED);

        auto val = mm.map();

        RTF_ASSERT(val != NULL);

        for(unsigned int i = 0; i < FILE_SIZE; i++)
            *val++ = 0;

    }

    fclose(dataFile);

    dataFile = fopen("data", "r+b");

    uint8_t buffer[4096];

    uint32_t blocksToRead = 256;

    while(blocksToRead > 0)
    {
        memset(buffer, 1, 4096);

        ck_fs::read_file(buffer, 4096, dataFile);
        --blocksToRead;

        for(uint32_t i = 0; i < 4096; i++)
            RTF_ASSERT(buffer[i] == 0);
    }

    fclose(dataFile);
}

void test_ck_memory_map::test_move()
{
    FILE* dataFile = fopen("data", "r+b");

    {
        ck_memory_map mm(fileno(dataFile),
                          0,
                          FILE_SIZE,
                          ck_memory_map::MM_PROT_READ | ck_memory_map::MM_PROT_WRITE,
                          ck_memory_map::MM_TYPE_FILE | ck_memory_map::MM_SHARED);

        ck_memory_map mm2 = std::move(mm);

        auto val = mm2.map();

        RTF_ASSERT(val != NULL);

        uint8_t expectedValue = 255;

        for(int i = 0; i < 256; i++)
        {
            for(int ii = 0; ii < 4096; ii++)
            {
                RTF_ASSERT(*val++ == expectedValue);
            }

            expectedValue--;
        }
    }

    fclose(dataFile);
}
