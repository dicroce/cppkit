
#include "framework.h"

class test_ck_blob_tree : public test_fixture
{
public:

    RTF_FIXTURE(test_ck_blob_tree);
      TEST(test_ck_blob_tree::test_basic);
      TEST(test_ck_blob_tree::test_objects_in_array);
    RTF_FIXTURE_END();

    virtual ~test_ck_blob_tree() throw() {}

    virtual void setup();
    virtual void teardown();

    void test_basic();
    void test_objects_in_array();
};
