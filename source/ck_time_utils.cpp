
#include "cppkit/ck_time_utils.h"
#include "cppkit/ck_string_utils.h"
#include "cppkit/ck_exception.h"
#include <string.h>

using namespace cppkit;
using namespace std;
using namespace std::chrono;

static bool _dst_in_effect(time_t t)
{
	tm* timeInfo = localtime(&t);
	return (bool)timeInfo->tm_isdst;
}

static time_t _utc_to_tz(time_t t)
{
    tm* timeInfo = localtime(&t);
	return t + timeInfo->tm_gmtoff;
}

static time_t _dst_offset()
{
    time_t currTimeT = time(0);

    tm janTM = *gmtime(&currTimeT);
    janTM.tm_mon = 1;
    janTM.tm_mday = 4;
    time_t janTimeT = mktime(&janTM);
	time_t adjJanTimeT = _utc_to_tz(janTimeT);
	time_t janDiff = adjJanTimeT - janTimeT;

    tm julTM = janTM;
    julTM.tm_mon = 7;
    time_t julTimeT = mktime(&julTM);
	time_t adjJulTimeT = _utc_to_tz(julTimeT);
	time_t julDiff = adjJulTimeT - julTimeT;

    return abs(julDiff - janDiff);
}

static time_t _parse_offset(const string& offset)
{
	auto colindex = offset.find(':');
	int hour = stoi(offset.substr(1, colindex)), minute = stoi(offset.substr(colindex+1));
	return (hour * 3600) + (minute * 60);
}

system_clock::time_point cppkit::ck_time_utils::iso_8601_to_tp(const string& str)
{
	// 1976-10-01T12:00:00.000+0:00   Interesting cases because time is essentially in UTC but
	// 1976-10-01T12:00:00.000-0:00   there is no trailing Z.
	//
	// 1976-10-01T12:00:00.000-7:00   Los Angeles
	//    local time is 12pm and that is behind utc by 7 hours
	//
	// 1976-10-01T12:00:00.000+3:00   Moscow
	//    local time is 12pm and that is ahead of utc by 3 hours

	auto tdex = str.find('T');

	if(tdex == string::npos)
		CK_THROW(("Invalid iso 8601 string"));

	auto dateStr = str.substr(0, tdex);

	int year, month, day;
	auto err = sscanf(dateStr.c_str(), "%4d-%2d-%2d", &year, &month, &day);
	if(err == EOF)
		CK_THROW(("iso 8601 parse error."));

	auto offsetdex = str.substr(tdex+1).rfind('+');

	if(offsetdex == string::npos)
		offsetdex = str.substr(tdex+1).rfind('-');
	
	bool hasOffset = (offsetdex != string::npos);

	auto timeStr = str.substr(tdex+1, offsetdex);

	int hour, minute, second;
	err = sscanf(timeStr.c_str(), "%2d:%2d:%2d", &hour, &minute, &second);

	auto pdex = timeStr.find(".");
	string frac = (pdex == string::npos)?"0":timeStr.substr(pdex);

	auto offsetStr = (offsetdex!=string::npos)?str.substr(offsetdex + tdex + 1):"+00:00";
	auto offset = _parse_offset(offsetStr);
	bool plus = (offsetStr[0] == '+');

	bool hasZ = (str.find('Z') != string::npos);

	auto ttm = tm();
	ttm.tm_year = year - 1900;
	ttm.tm_mon = month - 1;
	ttm.tm_mday = day;
	ttm.tm_hour = hour;
	ttm.tm_min = minute;
	ttm.tm_sec = second;

	time_t theTime;

	if(hasOffset)
	{
		theTime = timegm(&ttm);
		if(plus)
			theTime -= offset;
		else theTime += offset;
	}
	else
	{
		if(hasZ)
			theTime = timegm(&ttm);
		else
		{
			theTime = mktime(&ttm);
			if(_dst_in_effect(theTime))
				theTime -= _dst_offset();
		}
	}

	auto time_point_result = chrono::system_clock::from_time_t(theTime);

	auto fracSec = stod(frac);

	auto numMillis = (uint32_t)(fracSec * 1000);

	time_point_result += chrono::milliseconds(numMillis);

	return time_point_result;
}

string cppkit::ck_time_utils::tp_to_iso_8601(const system_clock::time_point& tp, bool UTC)
{
	auto utcTime = system_clock::to_time_t(tp);
	auto tpFromUTC = system_clock::from_time_t(utcTime);
	auto ms = duration_cast<milliseconds>(tp - tpFromUTC);

	struct tm bdtStorage;
	struct tm* bdt = &bdtStorage;

	if (UTC)
		bdt = gmtime(&utcTime);
	else bdt = localtime(&utcTime);

	if(!bdt)
		CK_STHROW(ck_internal_exception, ("failed to convert time point to broken down representation."));

	char date_time_format[] = "%Y-%m-%dT%H:%M:%S";

	char time_str[] = "yyyy-mm-ddTHH:MM:SS "; // XXX - dont delete trailing space!

	if( strftime(time_str, strlen(time_str), date_time_format, bdt) == 0 )
        CK_STHROW(ck_internal_exception, ("Failed to write formatted iso 8601 time string!"));

	char frac[6] = { 0, 0, 0, 0, 0, 0 };
	auto numMillis = ms.count();
	double fracV = ((double)numMillis) / 1000.0;
    ck_string_utils::format_buffer(frac, 6, "%3f", fracV);

	string result(time_str);
	result.append(".");
	result.append(frac+2);

	if (UTC)
		result.append("Z");

	return result;
}

milliseconds cppkit::ck_time_utils::iso_8601_period_to_duration(const string& str)
{
	auto dur = milliseconds::zero();

	auto numHours = duration_cast<hours>(dur).count();

	char designators[] = {'Y', 'M', 'W', 'D', 'T', 'H', 'M', 'S'};

	size_t idx = 0;

	auto prevDesig = 'P';

	bool parsedDate = false;

	for(size_t i = 0; i < 8; ++i)
	{
		numHours = duration_cast<hours>(dur).count();

		auto didx = str.find(designators[i], idx);
		idx = didx + 1;

		if(didx != string::npos)
		{
			//auto fieldStart = str.rfind_first_not_of("0123456789");
			auto fieldStart = str.rfind(prevDesig, didx) + 1;

			auto field = str.substr(fieldStart, (didx - fieldStart));

            auto val = ck_string_utils::s_to_size_t(field);

			if(!parsedDate)
			{
				switch(designators[i])
				{
					case 'Y'://YEARS
						dur += hours(val * 8760);
					break;
					case 'M'://MONTHS
						dur += hours(val * 720);
					break;
					case 'W'://WEEKS
						dur += hours(val * 168);
					break;
					case 'D'://DAYS
						dur += hours(val * 24);
					break;
					case 'T':
						parsedDate = true;
					break;
                	default:
                    	CK_THROW(("Unknown iso 8601 duration designator 1:"));
            	};
			}
			else
			{
				switch(designators[i])
				{
					case 'H'://HOURS
						dur += hours(val);
					break;
					case 'M'://MINUTES
						dur += minutes(val);
					break;
					case 'S'://SECONDS
					{
						if(field.find(".") != std::string::npos)
						{
							auto val = ck_string_utils::s_to_double(field);
							size_t wholeSeconds = (size_t)val;
							double fracSeconds = val - wholeSeconds;
							dur += seconds(wholeSeconds);
							size_t millis = (size_t)(fracSeconds * (double)1000);
							dur += milliseconds(millis);
						}
						else dur += seconds(val);
					}
					break;
					default:
						CK_THROW(("Unknown iso 8601 duration designator 2:"));
				};
			}

			prevDesig = designators[i];
		}
	}


	return dur;
}

string cppkit::ck_time_utils::duration_to_iso_8601_period(milliseconds d)
{
	string output = "P";

	auto numMillis = d.count();
	auto numHours = duration_cast<hours>(d).count();

    auto y = duration_cast<hours>(d).count() / 8760;
    d -= hours(y * 8760);
	if(y > 0)
		output += ck_string_utils::format("%dY", y);

    auto mo = duration_cast<hours>(d).count() / 720;
    d -= hours(mo * 720);
	if(mo > 0)
		output += ck_string_utils::format("%dM", mo);

    auto w = duration_cast<hours>(d).count() / 168;
    d -= hours(w * 168);
	if(w > 0)
		output += ck_string_utils::format("%dW", w);

    auto da = duration_cast<hours>(d).count() / 24;
    d -= hours(da * 24);
	if(da > 0)
		output += ck_string_utils::format("%dD", da);

    auto h = duration_cast<hours>(d).count();
    d -= hours(h);

    auto m = duration_cast<minutes>(d).count();
    d -= minutes(m);

    auto s = duration_cast<seconds>(d).count();
    d -= seconds(s);

    auto ms = duration_cast<milliseconds>(d).count();

	if(h > 0 || m > 0 || s > 0 || ms > 0)
		output += "T";

	if(h > 0)
		output += ck_string_utils::format("%dH", h);

	if(m > 0)
		output += ck_string_utils::format("%dM", m);

	if(s > 0)
		output += ck_string_utils::format("%lld", s);

    if(ms > 0)
    {
		if(s == 0)
			output += "0";

    	auto frac = ck_string_utils::double_to_s((double)ms / 1000.f).substr(2);
        frac.erase(frac.find_last_not_of('0') + 1, std::string::npos);
		output += ck_string_utils::format(".%sS",frac.c_str());
    }
    else
	{
		if(s > 0)
			output += "S";
	}

    return output;
}

uint64_t cppkit::ck_time_utils::tp_to_epoch_millis(const chrono::system_clock::time_point& tp)
{
	return duration_cast<milliseconds>(tp.time_since_epoch()).count();
}

chrono::system_clock::time_point cppkit::ck_time_utils::epoch_millis_to_tp(uint64_t t)
{
	return system_clock::time_point() + milliseconds(t);
}

bool cppkit::ck_time_utils::is_tz_utc()
{
	auto t = time(0);
	struct tm lt;
	localtime_r(&t, &lt);
	return (ck_string_utils::to_lower(string(lt.tm_zone)) == "utc") ? true : false;
}