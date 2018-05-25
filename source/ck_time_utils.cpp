
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

system_clock::time_point cppkit::ck_time_utils::iso_8601_to_tp(const string& str)
{
	const size_t tDex = str.find('T');

	if (tDex == string::npos)
        CK_STHROW(ck_invalid_argument_exception, ("Invalid iso 8601 string: %s",str.c_str()));

	const size_t dotDex = str.find('.');
	const size_t zDex = str.find('Z');
	const size_t plusDex = str.find('+', 1);
	const size_t subDex = str.find('-', str.find('-', str.find('-') + 1) + 1);

	if (plusDex != string::npos && subDex != string::npos)
        CK_STHROW(ck_invalid_argument_exception, ("Invalid iso 8601 string: %s",str.c_str()));

	size_t dtEnd = dotDex;

	if (dtEnd == string::npos || dtEnd > zDex)
		dtEnd = zDex;

	if (dtEnd == string::npos || dtEnd > plusDex)
		dtEnd = plusDex;

	if (dtEnd == string::npos || dtEnd > subDex)
		dtEnd = subDex;

	const string dateStr = str.substr(0, tDex);
	const string timeStr = str.substr(tDex + 1, (dtEnd - tDex) - 1);
	string fracSecStr;

	if (dotDex != string::npos)
	{
		size_t fsEnd = zDex;

		if (fsEnd == string::npos || fsEnd > plusDex)
			fsEnd = plusDex;

		if (fsEnd == string::npos || fsEnd > subDex)
			fsEnd = subDex;

		const size_t fsStart = dateStr.size() + 1 + timeStr.size();
		const size_t fsLen = fsEnd == string::npos ? string::npos : fsEnd - fsStart;

		fracSecStr = str.substr(fsStart, fsLen);
	}

	const size_t zoneDex = dateStr.size() + 1 + timeStr.size() + fracSecStr.size();
	const string zoneStr = zDex == str.size() ? "" : str.substr(zoneDex);

	tm ttm = tm();

	int yyyy = 0, mm = 0, dd = 0;
	sscanf(dateStr.c_str(), "%4d-%2d-%2d", &yyyy, &mm, &dd);

	ttm.tm_year = yyyy - 1900;
	ttm.tm_mon = mm - 1; // Month since January 
	ttm.tm_mday = dd; // Day of the month [1-31]

	int HH = 0, MM = 0, SS = 0;
	sscanf(timeStr.c_str(), "%2d:%2d:%2d", &HH, &MM, &SS);

	ttm.tm_hour = HH; // Hour of the day [00-23]
	ttm.tm_min = MM;
	ttm.tm_sec = SS;

	// We need to go from a broken down time (struct tm) to a time_t. BUT, we have to use the right function when converting
	// from struct tm. mktime() assumes the struct tm is in localtime. gmtime() assumes the struct tm is in UTC. If the incoming
	// iso 8601 string has a 'Z' then we need to use gmtime() (or _mkgmtime() on windows), else we can use mktime().
	time_t theTime = 0;
	if (zDex == string::npos) // input is local time
	{
		theTime = mktime(&ttm);
		if(_dst_in_effect(theTime))
			theTime -= _dst_offset();
	}
	else // input is UTC
	{
		theTime = timegm(&ttm);
	}

	system_clock::time_point time_point_result = chrono::system_clock::from_time_t(theTime);

	double fracSec = stod(fracSecStr);

	uint32_t numMillis = (uint32_t)(fracSec * 1000);

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