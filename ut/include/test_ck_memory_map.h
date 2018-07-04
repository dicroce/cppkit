
#ifndef _test_ck_memory_map_H_
#define _test_ck_memory_map_H_

#include "framework.h"

class test_ck_memory_map : public test_fixture
{
public:
    RTF_FIXTURE(test_ck_memory_map);
        TEST(test_ck_memory_map::test_file_mapping);
        TEST(test_ck_memory_map::test_persist_to_disk);
        TEST(test_ck_memory_map::test_move);
    RTF_FIXTURE_END();

    void setup();
    void teardown();

protected:

    void test_file_mapping();
    void test_persist_to_disk();
    void test_move();
};

#endif
