
#ifndef cppkit_ck_time_utils_h
#define cppkit_ck_time_utils_h

#include <chrono>
#include <string>

namespace cppkit
{

namespace ck_time_utils
{

std::chrono::system_clock::time_point iso_8601_to_tp(const std::string& str);

std::string tp_to_iso_8601(const std::chrono::system_clock::time_point& tp, bool UTC);

uint64_t tp_to_epoch_millis(const std::chrono::system_clock::time_point& tp);

std::chrono::system_clock::time_point epoch_millis_to_tp(uint64_t t);

template<typename INT>
INT convert_clock_freq(INT ticks, INT srcTicksPerSecond, INT dstTicksPerSecond)
{
    return ticks / srcTicksPerSecond * dstTicksPerSecond +
           ticks % srcTicksPerSecond * dstTicksPerSecond / srcTicksPerSecond;
}

bool is_tz_utc();

}

}

#endif
