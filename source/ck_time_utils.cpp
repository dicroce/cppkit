
/// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=--=-=-=-=-=-
/// cppkit - http://www.cppkit.org
/// Copyright (c) 2013, Tony Di Croce
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without modification, are permitted
/// provided that the following conditions are met:
///
/// 1. Redistributions of source code must retain the above copyright notice, this list of conditions and
///    the following disclaimer.
/// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
///    and the following disclaimer in the documentation and/or other materials provided with the
///    distribution.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
/// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
/// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
/// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
/// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
/// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
/// TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
/// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///
/// The views and conclusions contained in the software and documentation are those of the authors and
/// should not be interpreted as representing official policies, either expressed or implied, of the cppkit
/// Project.
/// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=--=-=-=-=-=-

#include "cppkit/os/ck_time_utils.h"
#include "cppkit/ck_string.h"
#include "cppkit/os/ck_sprintf.h"
#include <ctype.h>
#include <string.h>

using namespace cppkit;
using namespace std;
using namespace std::chrono;

#ifdef IS_WINDOWS

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
    #define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
    #define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

// The second argument is only here for compatibility. On Unix, the second argument is a now deprecated
// struct timezone*.
static int _ck_win32_gettimeofday( struct timeval* tv, void* obsolete )
{
    FILETIME ft;
    unsigned __int64 tmpres = 0;

    if( tv )
    {
        GetSystemTimeAsFileTime( &ft );

        tmpres |= ft.dwHighDateTime;
        tmpres <<= 32;
        tmpres |= ft.dwLowDateTime;

        tmpres /= 10;

        tmpres -= DELTA_EPOCH_IN_MICROSECS;

        tv->tv_sec = (long)(time_t)(tmpres / 1000000UL);
        tv->tv_usec = (int)(tmpres % 1000000UL);

        return 0;
    }

    return -1;
}

#endif

int cppkit::ck_gettimeofday(struct timeval* tv)
{
#ifdef IS_WINDOWS
    return _ck_win32_gettimeofday(tv, 0);
#else
    return gettimeofday(tv, 0);
#endif
}

void cppkit::ck_sleep(uint32_t seconds)
{
#ifdef IS_WINDOWS
    Sleep((DWORD)(seconds * 1000));
#else
    sleep(seconds);
#endif
}

void cppkit::ck_usleep(uint32_t usec)
{
#ifdef IS_WINDOWS
    if (usec >= 1000)
        Sleep((DWORD)(usec / 1000));
    else if (usec == 0)
        Sleep(0); // Support an explicit sleep time of 0.
    else
        Sleep(1);
#else
    usleep(usec);
#endif
}

#ifdef IS_WINDOWS
#define SSCANF sscanf_s
#else
#define SSCANF sscanf
#endif

chrono::system_clock::time_point cppkit::ck_iso_8601_to_time_point( const ck_string& str )
{
	const size_t tDex = str.find('T');

	if (tDex == string::npos)
        CK_THROW(("Invalid iso 8601 string: %s",str.c_str()));

	const size_t dotDex = str.find('.');
	const size_t zDex = str.find('Z');
	const size_t plusDex = str.find('+', 1);
	const size_t subDex = str.find('-', str.find('-', str.find('-') + 1) + 1);

	if (plusDex != string::npos && subDex != string::npos)
        CK_THROW(("Invalid iso 8601 string: %s",str.c_str()));

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
	SSCANF(dateStr.c_str(), "%4d-%2d-%2d", &yyyy, &mm, &dd);

	ttm.tm_year = yyyy - 1900;
	ttm.tm_mon = mm - 1; // Month since January 
	ttm.tm_mday = dd; // Day of the month [1-31]

	int HH = 0, MM = 0, SS = 0;
	SSCANF(timeStr.c_str(), "%2d:%2d:%2d", &HH, &MM, &SS);

	ttm.tm_hour = HH; // Hour of the day [00-23]
	ttm.tm_min = MM;
	ttm.tm_sec = SS;

	// We need to go from a broken down time (struct tm) to a time_t. BUT, we have to use the right function when converting
	// from struct tm. mktime() assumes the struct tm is in localtime. gmtime() assumes the struct tm is in UTC. If the incoming
	// iso 8601 string has a 'Z' then we need to use gmtime() (or _mkgmtime() on windows), else we can use mktime().
	time_t theTime = 0;
	if (zDex == string::npos) // input is local time
		theTime = mktime(&ttm);
	else // input is UTC
	{
#ifdef IS_WINDOWS
		theTime = _mkgmtime(&ttm);
#else
		theTime = timegm(&ttm);
#endif
	}

	system_clock::time_point time_point_result = std::chrono::system_clock::from_time_t(theTime);

	double fracSec = stod(fracSecStr);

	uint32_t numMillis = (uint32_t)(fracSec * 1000);

	time_point_result += std::chrono::milliseconds(numMillis);

	return time_point_result;
}

ck_string cppkit::ck_time_point_to_iso_8601( const chrono::system_clock::time_point& tp, bool UTC )
{
	auto utcTime = system_clock::to_time_t(tp);
	auto tpFromUTC = system_clock::from_time_t(utcTime);
	auto ms = duration_cast<milliseconds>(tp - tpFromUTC);

	struct tm bdtStorage;
	struct tm* bdt = &bdtStorage;

#ifdef IS_WINDOWS
	int err = 0;
	if (UTC)
		err = gmtime_s(&bdtStorage, &utcTime);
	else err = localtime_s(&bdtStorage, &utcTime);
	if (err != 0)
		bdt = NULL;
#else
	if (UTC)
		bdt = gmtime(&utcTime);
	else bdt = localtime(&utcTime);
#endif

	if(!bdt)
		CK_THROW(("failed to convert time point to broken down representation."));

	char date_time_format[] = "%Y-%m-%dT%H:%M:%S";

	char time_str[] = "yyyy-mm-ddTHH:MM:SS "; // XXX - dont delete trailing space!

	if( std::strftime(time_str, strlen(time_str), date_time_format, bdt) == 0 )
        CK_THROW(("Failed to write formatted iso 8601 time string!"));

	char frac[6] = { 0, 0, 0, 0, 0, 0 };
	auto numMillis = ms.count();
	double fracV = ((double)numMillis) / 1000.0;
	ck_snprintf(frac, 6, "%3f", fracV);

	string result(time_str);
	result.append(".");
	result.append(frac+2);

	if (UTC)
		result.append("Z");

	return result;
}
