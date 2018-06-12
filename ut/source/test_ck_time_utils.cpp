
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
    string pts = "2018-03-10T20:00:00.000";
    auto ptp = ck_time_utils::iso_8601_to_tp(pts);

    string ats = "2018-03-11T20:00:00.000"; // remember, this time is sprung ahead...
    auto atp = ck_time_utils::iso_8601_to_tp(ats);

    int min = duration_cast<minutes>(atp-ptp).count();

    // not 1440 becuase that second time (being local) was "sprung ahead" and so the difference between them is less that 24 hours of minutes.
    RTF_ASSERT(min == 1380);
}

void test_ck_time_utils::test_tp_to_8601()
{
    RTF_ASSERT(ck_time_utils::tp_to_iso_8601(system_clock::from_time_t(1520740800), false) == "2018-03-10T20:00:00.000");
    RTF_ASSERT(ck_time_utils::tp_to_iso_8601(system_clock::from_time_t(1520712000), true) == "2018-03-10T20:00:00.000Z");
    RTF_ASSERT(ck_time_utils::tp_to_iso_8601(system_clock::from_time_t(1520823600), false) == "2018-03-11T20:00:00.000");
    RTF_ASSERT(ck_time_utils::tp_to_iso_8601(system_clock::from_time_t(1520798400), true) == "2018-03-11T20:00:00.000Z");
}
