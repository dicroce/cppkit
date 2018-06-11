
#ifndef cppkit_ck_logger_h
#define cppkit_ck_logger_h

#include <cstdarg>
#include <syslog.h>

namespace cppkit
{

namespace ck_logger
{

enum LOG_LEVEL
{
    LOG_LEVEL_CRITICAL = 1,
    LOG_LEVEL_ERROR = 3,
    LOG_LEVEL_WARNING = 4,
    LOG_LEVEL_NOTICE = 5,
    LOG_LEVEL_INFO = 6,
    LOG_LEVEL_TRACE = 7,
    LOG_LEVEL_DEBUG = 8
};

#define CK_LOG_CRITICAL(format, ...) cppkit::ck_logger::write(cppkit::ck_logger::LOG_LEVEL_CRITICAL, __LINE__, __FILE__, format,  ##__VA_ARGS__)
#define CK_LOG_ERROR(format, ...) cppkit::ck_logger::write(cppkit::ck_logger::LOG_LEVEL_ERROR, __LINE__, __FILE__, format,  ##__VA_ARGS__)
#define CK_LOG_WARNING(format, ...) cppkit::ck_logger::write(cppkit::ck_logger::LOG_LEVEL_WARNING, __LINE__, __FILE__, format,  ##__VA_ARGS__)
#define CK_LOG_NOTICE(format, ...) cppkit::ck_logger::write(cppkit::ck_logger::LOG_LEVEL_NOTICE, __LINE__, __FILE__, format,  ##__VA_ARGS__)
#define CK_LOG_INFO(format, ...) cppkit::ck_logger::write(cppkit::ck_logger::LOG_LEVEL_INFO, __LINE__, __FILE__, format,  ##__VA_ARGS__)
#define CK_LOG_TRACE(format, ...) cppkit::ck_logger::write(cppkit::ck_logger::LOG_LEVEL_TRACE, __LINE__, __FILE__, format,  ##__VA_ARGS__)
#define CK_LOG_DEBUG(format, ...) cppkit::ck_logger::write(cppkit::ck_logger::LOG_LEVEL_DEBUG, __LINE__, __FILE__, format,  ##__VA_ARGS__)

void write(LOG_LEVEL level, int line, const char* file, const char* format, ...);
void write(LOG_LEVEL level, int line, const char* file, const char* format, va_list& args);

void install_terminate();

}

}

#endif
