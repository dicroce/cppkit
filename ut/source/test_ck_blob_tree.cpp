
#include "test_ck_blob_tree.h"
#include "cppkit/ck_uuid_utils.h"
#include "cppkit/ck_blob_tree.h"
#include "cppkit/ck_sha_256.h"

using namespace std;
using namespace cppkit;

REGISTER_TEST_FIXTURE(test_ck_blob_tree);

void test_ck_blob_tree::setup()
{
}

void test_ck_blob_tree::teardown()
{
}

string hasher(const uint8_t* p, size_t size)
{
    ck_sha_256 hash;
    hash.update(p, size);
    hash.finalize();
    return hash.get_as_string();
}

void test_ck_blob_tree::test_basic()
{
    vector<uint8_t> blob_1 = {1, 2, 3, 4};
    auto blob_1_hash = hasher(&blob_1[0], blob_1.size());
    vector<uint8_t> blob_2 = {5, 6, 7, 8};
    auto blob_2_hash = hasher(&blob_2[0], blob_2.size());
    vector<uint8_t> blob_3 = {9, 10, 11, 12};
    auto blob_3_hash = hasher(&blob_3[0], blob_3.size());
    vector<uint8_t> blob_4 = {13, 14, 15, 16};
    auto blob_4_hash = hasher(&blob_4[0], blob_4.size());

    ck_blob_tree rt1;
    rt1["hello"]["my sweeties!"][0] = make_pair(blob_1.size(), &blob_1[0]);
    rt1["hello"]["my sweeties!"][1] = make_pair(blob_2.size(), &blob_2[0]);
    rt1["hello"]["my darlings!"][0] = make_pair(blob_3.size(), &blob_3[0]);
    rt1["hello"]["my darlings!"][1] = make_pair(blob_4.size(), &blob_4[0]);

    auto buffer = ck_blob_tree::serialize(rt1, 42);

    uint32_t version;
    auto rt2 = ck_blob_tree::deserialize(&buffer[0], buffer.size(), version);

    RTF_ASSERT(version == 42);

    RTF_ASSERT(rt2["hello"]["my sweeties!"].size() == 2);

    auto val = rt2["hello"]["my sweeties!"][0].get();
    RTF_ASSERT(hasher(val.second, val.first) == blob_1_hash);

    val = rt2["hello"]["my sweeties!"][1].get();
    RTF_ASSERT(hasher(val.second, val.first) == blob_2_hash);

    val = rt2["hello"]["my darlings!"][0].get();
    RTF_ASSERT(hasher(val.second, val.first) == blob_3_hash);

    val = rt2["hello"]["my darlings!"][1].get();
    RTF_ASSERT(hasher(val.second, val.first) == blob_4_hash);
}

void test_ck_blob_tree::test_objects_in_array()
{
    ck_blob_tree rt1;

    string b1 = "We need a nine bit byte.";
    string b2 = "What if a building has more doors than their are guids?";
    string b3 = "If java compiles, its bug free.";
    string b4 = "Microsoft COM is a good idea.";

    rt1[0]["obj_a"] = make_pair(b1.length(), (uint8_t*)b1.c_str());
    rt1[0]["obj_b"] = make_pair(b2.length(), (uint8_t*)b2.c_str());
    rt1[1]["obj_a"] = make_pair(b3.length(), (uint8_t*)b3.c_str());
    rt1[1]["obj_b"] = make_pair(b4.length(), (uint8_t*)b4.c_str());

    auto buffer = ck_blob_tree::serialize(rt1, 42);

    uint32_t version;
    auto rt2 = ck_blob_tree::deserialize(&buffer[0], buffer.size(), version);

    auto val = rt2[0]["obj_a"].get();
    string ab1((char*)val.second, val.first);

    val = rt2[0]["obj_b"].get();
    string ab2((char*)val.second, val.first);

    val = rt2[1]["obj_a"].get();
    string ab3((char*)val.second, val.first);

    val = rt2[1]["obj_b"].get();
    string ab4((char*)val.second, val.first);

    RTF_ASSERT(b1 == ab1);
    RTF_ASSERT(b2 == ab2);
    RTF_ASSERT(b3 == ab3);
    RTF_ASSERT(b4 == ab4);
}
