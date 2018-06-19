
#include "test_ck_time_utils.h"
#include "cppkit/ck_uuid_utils.h"
#include "cppkit/ck_time_utils.h"
#include <numeric>
#include <string.h>

using namespace cppkit;
using namespace cppkit::ck_time_utils;
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
    if(is_tz_utc())
    {
        RTF_ASSERT(system_clock::to_time_t(iso_8601_to_tp("2018-06-13T16:12:07.000Z")) == 1528906327);
    }
    else
    {
        // Note: This is a bad test because it assumes that your in a TZ that observes DST!

        string pts = "2018-03-10T20:00:00.000";
        auto ptp = iso_8601_to_tp(pts);

        string ats = "2018-03-11T20:00:00.000"; // remember, this time is sprung ahead...
        auto atp = iso_8601_to_tp(ats);

        int min = duration_cast<minutes>(atp-ptp).count();

        // not 1440 becuase that second time (being local) was "sprung ahead" and so the difference between them is less that 24 hours of minutes.
        RTF_ASSERT(min == 1380); // 1440 is for tz's that dont follow dst...
    }
}

void test_ck_time_utils::test_tp_to_8601()
{
    if(is_tz_utc())
    {
        RTF_ASSERT(tp_to_iso_8601(system_clock::from_time_t(1528906327), true) == "2018-06-13T16:12:07.000Z");
    }
    else
    {
        RTF_ASSERT(tp_to_iso_8601(system_clock::from_time_t(1520740800), false) == "2018-03-10T20:00:00.000");
        RTF_ASSERT(tp_to_iso_8601(system_clock::from_time_t(1520712000), true) == "2018-03-10T20:00:00.000Z");
        RTF_ASSERT(tp_to_iso_8601(system_clock::from_time_t(1520823600), false) == "2018-03-11T20:00:00.000");
        RTF_ASSERT(tp_to_iso_8601(system_clock::from_time_t(1520798400), true) == "2018-03-11T20:00:00.000Z");
    }
}

void test_ck_time_utils::test_8601_period_to_duration()
{
    {
        auto siny = milliseconds::zero() + hours(8760) + hours(720) + hours(168) + hours(24) + hours(1) + minutes(1) + seconds(1);

        RTF_ASSERT(siny == iso_8601_period_to_duration("P1Y1M1W1DT1H1M1S"));
    }

    {
        auto siny = hours(8760);
        auto dur = iso_8601_period_to_duration("P1Y");
        auto numMillis = dur.count();
        RTF_ASSERT(siny == iso_8601_period_to_duration("P1Y"));
    }

    {
        auto siny = hours(8760) + milliseconds(500);
        RTF_ASSERT(siny == iso_8601_period_to_duration("P1YT0.5S"));
    }

    {
        auto siny = hours(87600) + hours(288);
        RTF_ASSERT(siny == iso_8601_period_to_duration("P10Y12D"));
    }

    {
        auto siny = hours(100);
        RTF_ASSERT(siny == iso_8601_period_to_duration("PT100H"));
    }

    {
        auto dur = iso_8601_period_to_duration("PT100H") - iso_8601_period_to_duration("PT25H") + iso_8601_period_to_duration("P1D");
        RTF_ASSERT(duration_to_iso_8601_period(dur) == "P4DT3H");
    }

    RTF_ASSERT(iso_8601_period_to_duration("PT100H") > iso_8601_period_to_duration("P3D"));
    RTF_ASSERT(iso_8601_period_to_duration("P1W") > iso_8601_period_to_duration("PT72H"));
}

void test_ck_time_utils::test_duration_to_8601_period()
{
    {
        auto siny = duration_cast<milliseconds>(hours(8760) + hours(720) + hours(168) + hours(24) + hours(1) + minutes(1) + seconds(1));
        RTF_ASSERT(duration_to_iso_8601_period(siny) == "P1Y1M1W1DT1H1M1S");
    }

    {
        auto siny = hours(8760) + hours(720) + hours(168) + hours(24) + hours(1) + minutes(1) + seconds(1) + milliseconds(250);
        RTF_ASSERT(duration_to_iso_8601_period(siny) == "P1Y1M1W1DT1H1M1.25S");
    }

    {
        auto siny = duration_cast<milliseconds>(hours(8760 * 3) + minutes(5));
        RTF_ASSERT(duration_to_iso_8601_period(siny) == "P3YT5M");
    }

    {
        auto siny = duration_cast<milliseconds>(hours(8760 * 3) + milliseconds(250));
        RTF_ASSERT(duration_to_iso_8601_period(siny) == "P3YT0.25S");
    }

    {
        auto siny = duration_cast<milliseconds>(milliseconds(250));
        RTF_ASSERT(duration_to_iso_8601_period(siny) == "PT0.25S");
    }
}
