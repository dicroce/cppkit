
#include "test_ck_time_utils.h"
#include "cppkit/ck_uuid_utils.h"
#include "cppkit/ck_time_utils.h"
#include <numeric>
#include <string.h>

using namespace cppkit;
using namespace std;
using namespace std::chrono;

REGISTER_TEST_FIXTURE(test_ck_time_utils);

void test_ck_time_utils::setup()
{
}

void test_ck_time_utils::teardown()
{
}

void test_ck_time_utils::test_8601_to_tp()
{
    if(ck_time_utils::is_tz_utc())
    {
        RTF_ASSERT(system_clock::to_time_t(ck_time_utils::iso_8601_to_tp("2018-06-13T16:12:07.000Z")) == 1528906327);
    }
    else
    {
        // Note: This is a bad test because it assumes that your in a TZ that observes DST!

        string pts = "2018-03-10T20:00:00.000";
        auto ptp = ck_time_utils::iso_8601_to_tp(pts);

        string ats = "2018-03-11T20:00:00.000"; // remember, this time is sprung ahead...
        auto atp = ck_time_utils::iso_8601_to_tp(ats);

        int min = duration_cast<minutes>(atp-ptp).count();

        // not 1440 becuase that second time (being local) was "sprung ahead" and so the difference between them is less that 24 hours of minutes.
        RTF_ASSERT(min == 1380); // 1440 is for tz's that dont follow dst...
    }
}

void test_ck_time_utils::test_tp_to_8601()
{
    if(ck_time_utils::is_tz_utc())
    {
        RTF_ASSERT(ck_time_utils::tp_to_iso_8601(system_clock::from_time_t(1528906327), true) == "2018-06-13T16:12:07.000Z");
    }
    else
    {
        RTF_ASSERT(ck_time_utils::tp_to_iso_8601(system_clock::from_time_t(1520740800), false) == "2018-03-10T20:00:00.000");
        RTF_ASSERT(ck_time_utils::tp_to_iso_8601(system_clock::from_time_t(1520712000), true) == "2018-03-10T20:00:00.000Z");
        RTF_ASSERT(ck_time_utils::tp_to_iso_8601(system_clock::from_time_t(1520823600), false) == "2018-03-11T20:00:00.000");
        RTF_ASSERT(ck_time_utils::tp_to_iso_8601(system_clock::from_time_t(1520798400), true) == "2018-03-11T20:00:00.000Z");
    }
}
