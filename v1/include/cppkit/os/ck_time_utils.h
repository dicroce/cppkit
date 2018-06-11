
#ifndef cppkit_time_utils_h
#define cppkit_time_utils_h

#include "cppkit/os/ck_platform.h"
#include "cppkit/ck_exception.h"
#include "cppkit/ck_string.h"
#include <ctime>
#include <chrono>

#ifdef IS_WINDOWS
  #include <winsock2.h>
#else
  #include <sys/time.h>
  #include <unistd.h>
#endif

namespace cppkit
{

CK_API int ck_gettimeofday(struct timeval* tv);

CK_API void ck_sleep(uint32_t seconds);

CK_API void ck_usleep(uint32_t usec);

CK_API std::chrono::system_clock::time_point ck_iso_8601_to_time_point( const ck_string& str );

CK_API ck_string ck_time_point_to_iso_8601( const std::chrono::system_clock::time_point& tp, bool UTC );

template<typename INTEGER>
CK_API INTEGER ck_convert_clock_freq(INTEGER ticks, INTEGER srcTicksPerSecond, INTEGER dstTicksPerSecond)
{
    return ticks / srcTicksPerSecond * dstTicksPerSecond +
           ticks % srcTicksPerSecond * dstTicksPerSecond / srcTicksPerSecond;
}

}

#ifdef IS_WINDOWS
  #define timeradd(a, b, result) \
    CK_MACRO_BEGIN \
      (result)->tv_sec = (a)->tv_sec + (b)->tv_sec; \
      (result)->tv_usec = (a)->tv_usec + (b)->tv_usec; \
      if ((result)->tv_usec >= 1000000) { \
        ++(result)->tv_sec; \
        (result)->tv_usec -= 1000000; \
      } \
    CK_MACRO_END

  #define timersub(a, b, result) \
    CK_MACRO_BEGIN \
      (result)->tv_sec = (a)->tv_sec - (b)->tv_sec; \
      (result)->tv_usec = (a)->tv_usec - (b)->tv_usec; \
      if ((result)->tv_usec < 0) { \
        --(result)->tv_sec; \
        (result)->tv_usec += 1000000; \
      } \
    CK_MACRO_END
#endif

#endif