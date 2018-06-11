
#include "cppkit/ck_logger.h"
#include "cppkit/ck_string_utils.h"
#include <exception>

using namespace cppkit;
using namespace std;

void cppkit::ck_logger::write(LOG_LEVEL level,
                              int line,
                              const char* file,
                              const char* format,
                              ...)
{
    va_list args;
    va_start(args, format);
    ck_logger::write(level, line, nullptr, format, args);
    va_end(args);
}

void cppkit::ck_logger::write(LOG_LEVEL level,
                              int line,
                              const char* file,
                              const char* format,
                              va_list& args)
{
    int priority = LOG_USER;

    switch(level)
    {
        case LOG_LEVEL_CRITICAL: { priority |= LOG_CRIT; break; }
        case LOG_LEVEL_ERROR: { priority |= LOG_ERR; break; }
        case LOG_LEVEL_WARNING: { priority |= LOG_WARNING; break; }
        case LOG_LEVEL_NOTICE: { priority |= LOG_NOTICE; break; }
        case LOG_LEVEL_INFO: { priority |= LOG_INFO; break; }
        case LOG_LEVEL_TRACE: { priority |= LOG_INFO; break; }
        case LOG_LEVEL_DEBUG: { priority |= LOG_INFO; break; }
        default: break;
    };

    vsyslog(priority, format, args);
}

void cppkit_terminate()
{
    CK_LOG_CRITICAL( "cppkit terminate handler called!" );
    printf("cppkit terminate handler called!\n");

    std::exception_ptr p = std::current_exception();

    if( p )
    {
        try
        {
            std::rethrow_exception( p );
        }
        catch(std::exception& ex)
        {
            CK_LOG_CRITICAL("%s",ex.what());
            printf("%s\n",ex.what());
        }
        catch(...)
        {
            CK_LOG_CRITICAL("unknown exception in cppkit_terminate().");
        }
    }

    fflush(stdout);

    std::abort();
}

void cppkit::ck_logger::install_terminate()
{
    set_terminate( cppkit_terminate );
}
