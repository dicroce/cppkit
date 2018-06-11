
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
/// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=--=-=-=-=-=-

#include "cppkit/ck_types.h"
#include "cppkit/ck_stack_trace.h"
#include "cppkit/ck_logger.h"
#include "cppkit/os/ck_sprintf.h"

#include <iostream>
#include <map>
#include <cstdio>
#include <sstream>
#include <ctime>
#include <thread>

#ifdef IS_WINDOWS
#include <Windows.h>
#else
#include <signal.h>
#include <cstdlib>
#include <syslog.h>
#endif

#define CK_LOG_CHUNK_SIZE 1024

using namespace std;
using namespace cppkit;

CK_API std::string _log_level_prefix(cppkit::LOG_LEVEL level);
CK_API void _log_level_handler(int signal);

bool ck_log::log_enabled()
{
    lock_guard<recursive_mutex> guard( _log_lock );

    return _log_enabled;
}

void ck_log::set_log_enabled(bool enabled)
{
    lock_guard<recursive_mutex> guard( _log_lock );

    _log_enabled = enabled;
}

uint32_t ck_log::get_log_flags()
{
    lock_guard<recursive_mutex> guard( _log_lock );

    return _curr_log_flags;
}

void ck_log::set_log_flags(const uint32_t flags)
{
    lock_guard<recursive_mutex> guard( _log_lock );

    _curr_log_flags = flags;
}

LOG_LEVEL ck_log::get_log_level()
{
    lock_guard<recursive_mutex> guard( _log_lock );

    return _curr_log_level;
}

void ck_log::set_log_level(const LOG_LEVEL level)
{
    lock_guard<recursive_mutex> guard( _log_lock );

    _curr_log_level = level;
}

std::string ck_log::log_level_to_string(const LOG_LEVEL level)
{
    switch (level)
    {
    case LOGLEVEL_DEBUG:
        return "debug";
    case LOGLEVEL_TRACE:
        return "trace";
    case LOGLEVEL_INFO:
        return "info";
    case LOGLEVEL_NOTICE:
        return "notice";
    case LOGLEVEL_WARNING:
        return "warning";
    case LOGLEVEL_ERROR:
        return "error";
    case LOGLEVEL_CRITICAL:
        return "critical";
    default:
        CK_THROW(("ck_log: Unknown log level (%d)", level));
    };
}

LOG_LEVEL ck_log::string_to_log_level(const std::string& str)
{
    ck_string level = ck_string(str).to_lower();

    if (level == "debug")
        return LOGLEVEL_DEBUG;
    else if (level == "trace")
        return LOGLEVEL_TRACE;
    else if (level == "info")
        return LOGLEVEL_INFO;
    else if (level == "notice")
        return LOGLEVEL_NOTICE;
    else if (level == "warning")
        return LOGLEVEL_WARNING;
    else if (level == "error")
        return LOGLEVEL_ERROR;
    else if (level == "critical")
        return LOGLEVEL_CRITICAL;
    else
        CK_THROW(("ck_log: Unknown log level (%s)", level.c_str()));
}

bool ck_log::print_to_std_out_enabled()
{
    lock_guard<recursive_mutex> guard( _log_lock );

    return _print_to_std_out_enabled;
}

void ck_log::set_print_to_std_out_enabled(bool enabled)
{
    lock_guard<recursive_mutex> guard( _log_lock );

    _print_to_std_out_enabled = enabled;
}

bool ck_log::print_to_file_enabled()
{
    return _file.get() != 0;
}

void ck_log::disable_print_to_file()
{
    _file.reset(0);
}

void ck_log::enable_print_to_file(const std::string& filename, bool append)
{
    if(append)
        _file.reset(new ofstream(filename.c_str(), ios_base::app));
    else _file.reset(new ofstream(filename.c_str()));
}

void ck_log::install_log_level_sig_handler()
{
#ifndef IS_WINDOWS
    //Set up signal handler.
    if(signal(SIGUSR1, _log_level_handler) == SIG_ERR)
        CK_LOG_ERROR("SIGUSR1 failed to initialize");

    if(signal(SIGUSR2, _log_level_handler) == SIG_ERR)
        CK_LOG_ERROR("SIGUSR2 failed to initialize");
#endif
}

void cppkit_terminate()
{
    ck_log::set_print_to_std_out_enabled( true );

    CK_LOG_CRITICAL( "cppkit uncaught exception terminate handler called!" );

    std::exception_ptr p = std::current_exception();

    if( p )
    {
        try
        {
            std::rethrow_exception( p );
        }
        catch(ck_exception& ex)
        {
            CK_LOG_CRITICAL("%s",ex.what());
        }
        catch(std::exception& ex)
        {
            CK_LOG_CRITICAL("%s",ex.what());
        }
        catch(...)
        {
            CK_LOG_CRITICAL("cppkit caught an unknown exception in custom terminate handler.");
        }
    }

    std::abort();
}

void ck_log::install_terminate()
{
    set_terminate( cppkit_terminate );
}

bool ck_log::_should_log( const LOG_LEVEL level)
{
    lock_guard<recursive_mutex> guard( _log_lock );

    if(!_log_enabled)
        return false;

    if( _curr_log_level < level )
        return false;

    return true;
}

bool ck_log::write( const LOG_LEVEL level, const char* format, ...)
{
    if(!format)
        return false;

    lock_guard<recursive_mutex> guard( _log_lock );

    if ( !_should_log(level) ) return true;

    va_list args;
    va_start(args, format);
    const bool result = write(level, 0, NULL, format, args);
    va_end(args);

    return result;
}

bool ck_log::write( const LOG_LEVEL level, const int line, const char* file, const char* format, ...)
{
    if(!format)
        return false;

    lock_guard<recursive_mutex> guard( _log_lock );
    if ( !_should_log(level) ) return true;

    va_list args;
    va_start(args, format);
    const bool result = write(level, line, file, format, args);
    va_end(args);

    return result;
}

bool ck_log::write(const LOG_LEVEL level, const int line, const char* file, const char* format, va_list& args)
{
    if(!format)
        return false;

    lock_guard<recursive_mutex> guard( _log_lock );
    if ( !_should_log(level) ) return true;

    ck_string log_msg = ck_string::format( format, args );

#ifndef IS_WINDOWS
    //Note: must escape all of the % with %% so syslog can display them correctly
    log_msg = log_msg.replace_all( "%", "%%" );
#endif

    // Write the message to the log.
    bool result = false;
    if( ! log_msg.empty() )
    {
        stringstream ex_log_msg;

        // Create the extended information string if flags are enabled.
        if(_curr_log_flags)
        {
            // Prepend the time
            if(_curr_log_flags & kLogTimestamp)
            {
				char timeBuf[1024];

                time_t now = time(0);
#ifdef IS_WINDOWS
				ctime_s( timeBuf, 1024, &now );
				string t = timeBuf;
#else
                string t = ctime(&now);
#endif
                if(!t.empty())
                    t.erase(t.size()-1); //ctime appends a newline
                else
                    t = "ctime error";

                ex_log_msg << '[' << t << "] ";
            }

            // Prepend the thread Id
            if(_curr_log_flags & kLogThreadID)
                ex_log_msg << "Thread ID [" << this_thread::get_id() << "] ";

            // Prepend the file and line number.
            if((_curr_log_flags & kLogFileLine) && file)
                ex_log_msg << file << '(' << line << "): ";
        }

        // Append our loglevel prefix no matter what.
        ex_log_msg << _log_level_prefix(level);

        result = _write( level, ex_log_msg.str(), log_msg.c_str() );
    }

    return result;
}

std::string _log_level_prefix(cppkit::LOG_LEVEL level)
{
    std::string prefix;

    switch(level)
    {
        case LOGLEVEL_CRITICAL:
        {
            prefix = std::string( kLogCriticalPrefix );
            break;
        }
        case LOGLEVEL_ERROR:
        {
            prefix = std::string( kLogErrorPrefix );
            break;
        }
        case LOGLEVEL_WARNING:
        {
            prefix = std::string( kLogWarningPrefix );
            break;
        }
        case LOGLEVEL_NOTICE:
        {
            prefix = std::string( kLogNoticePrefix );
            break;
        }
        case LOGLEVEL_INFO:
        {
            prefix = std::string( kLogInfoPrefix );
            break;
        }
        case LOGLEVEL_TRACE:
        {
            prefix = std::string( kLogTracePrefix );
            break;
        }
        case LOGLEVEL_DEBUG:
        {
            prefix = std::string( kLogDebugPrefix );
            break;
        }
    }

    return prefix;
}

void _log_level_handler(int signal)
{
#ifndef IS_WINDOWS
    switch(signal)
    {
        case SIGUSR1:
        {
            const LOG_LEVEL level = ck_log::get_log_level();
            if(level < LOGLEVEL_DEBUG)
                ck_log::set_log_level(LOG_LEVEL(level + 1));
            else
                ck_log::set_print_to_std_out_enabled(!ck_log::print_to_std_out_enabled());
            break;
        }
        case SIGUSR2:
        {
            const LOG_LEVEL level = ck_log::get_log_level();
            if(level > LOGLEVEL_CRITICAL)
                ck_log::set_log_level(LOG_LEVEL(level - 1));
            break;
        }
        default:
        break;
    };
#endif
}

CK_API bool ck_log::_write( const LOG_LEVEL level, const std::string& prefix, const std::string& msg)
{
#ifdef IS_WINDOWS
    ck_string fullMessage = " ";
    fullMessage.append(prefix);
    fullMessage.append(msg);
    static const wchar_t NEW_LINE[] = L"\n";
    OutputDebugStringW(fullMessage.get_wide_string().data());
    OutputDebugStringW(NEW_LINE);
#else
    int priority = LOG_USER;

    switch(level)
    {
        case LOGLEVEL_CRITICAL: {priority |= LOG_CRIT;    break;}
        case LOGLEVEL_ERROR:    {priority |= LOG_ERR;     break;}
        case LOGLEVEL_WARNING:  {priority |= LOG_WARNING; break;}
        case LOGLEVEL_NOTICE:   {priority |= LOG_NOTICE;  break;}
        case LOGLEVEL_INFO:     {priority |= LOG_INFO;    break;}
        case LOGLEVEL_TRACE:    {priority |= LOG_INFO;    break;}
        case LOGLEVEL_DEBUG:    {priority |= LOG_INFO;    break;}
        default:
            return false;
    };

    // No need to chunk log, just write it all
    if(msg.size() <= CK_LOG_CHUNK_SIZE)
    {
        stringstream msgStream;
        msgStream << prefix << msg;
        syslog(priority, msgStream.str().c_str());
    }
    // Write chunks to syslog to avoid message length errors
    else
    {
        size_t msgSize = msg.size();

        // Get chunk count
        int chunkCount = (int)(msgSize / CK_LOG_CHUNK_SIZE);
        if ( (msgSize % CK_LOG_CHUNK_SIZE) != 0 )
            chunkCount++;

        size_t pos = 0;
        int i = 1;
        for(; pos < msgSize; pos += CK_LOG_CHUNK_SIZE, ++i)
        {
            size_t endpos = pos + CK_LOG_CHUNK_SIZE;
            stringstream msgChunk;
            msgChunk << prefix << "(" << i << "/" << chunkCount << ") ";

            if ( endpos < msgSize )
                msgChunk << msg.substr(pos, CK_LOG_CHUNK_SIZE);
            // If the end position is past the end of the string, just go to the end.
            else
                msgChunk << msg.substr(pos);

            syslog(priority, msgChunk.str().c_str());
        }
    }
#endif

    if(_print_to_std_out_enabled)
        cout << " " << msg << endl;

    if(_file.get())
        *_file << " " << msg << endl;

    return true;
}

#ifndef NDEBUG
uint32_t ck_log::_curr_log_flags = kLogFileLine | kLogThreadID | kLogTimestamp;
LOG_LEVEL ck_log::_curr_log_level = LOGLEVEL_DEBUG;
#else
uint32_t ck_log::_curr_log_flags = 0;
LOG_LEVEL ck_log::_curr_log_level = LOGLEVEL_WARNING;
#endif

bool ck_log::_log_enabled = true;
std::recursive_mutex ck_log::_log_lock;
bool ck_log::_print_to_std_out_enabled = false;
unique_ptr<ofstream> ck_log::_file;

ck_log::static_init::static_init()
{
#ifndef IS_WINDOWS
    //Set up syslog.
    openlog(0, 0, LOG_USER);

    //Set up signal handler.
    if(signal(SIGUSR1, _log_level_handler) == SIG_ERR)
    {
        //For some reason, if I use LOGLEVEL_ERROR directly instead of creating this
        //extraneous variable, the compiler complains about LOGLEVEL_ERROR not being
        //in this scope, but using the extraneous variable works, so it gets to be
        //there to get rid of the problem.
        const LOG_LEVEL error = LOGLEVEL_ERROR;
        syslog(error, "SIGUSR1 failed to initialize");
    }
    if(signal(SIGUSR2, _log_level_handler) == SIG_ERR)
    {
        const LOG_LEVEL error = LOGLEVEL_ERROR;
        syslog(error, "SIGUSR2 failed to initialize");
    }
#endif
}

const ck_log::static_init _static_init;
