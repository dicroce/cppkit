
#include "framework.h"

class test_ck_time_utils : public test_fixture
{
public:

    RTF_FIXTURE(test_ck_time_utils);
      TEST(test_ck_time_utils::test_8601_to_tp);
      TEST(test_ck_time_utils::test_tp_to_8601);
      TEST(test_ck_time_utils::test_8601_period_to_duration);
      TEST(test_ck_time_utils::test_duration_to_8601_period);
    RTF_FIXTURE_END();

    virtual ~test_ck_time_utils() throw() {}

    virtual void setup();
    virtual void teardown();

    void test_8601_to_tp();
    void test_tp_to_8601();
    void test_8601_period_to_duration();
    void test_duration_to_8601_period();
};
