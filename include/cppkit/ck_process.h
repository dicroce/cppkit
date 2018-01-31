
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

#ifndef cppkit_actor_h
#define cppkit_actor_h

#include "cppkit/ck_types.h"
#include "cppkit/ck_string.h"
#include "cppkit/ck_file.h"

#ifdef IS_POSIX
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#else
#include <Windows.h>
#endif

#include <list>
#include <chrono>

namespace cppkit
{

struct ck_pid
{
#ifdef IS_WINDOWS
    DWORD pid;
#else
    pid_t pid;
#endif

    ck_pid()
    {
#ifdef IS_WINDOWS
        pid = 0;
#else
        pid = -1;
#endif
    }

    ck_pid( const ck_pid& obj ) { pid = obj.pid; }

    ck_pid& operator = ( const ck_pid& obj ) { pid = obj.pid; return *this; }

    bool valid() const
    {
#ifdef IS_WINDOWS
        if( pid > 0 )
            return true;
#else
        if( pid >= 0 )
            return true;
#endif
        return false;
    }

    void clear()
    {
#ifdef IS_WINDOWS
        pid = 0;
#else
        pid = -1;
#endif
    }

    bool operator == ( const ck_pid& obj )
    {
        if( pid == obj.pid )
            return true;
        return false;
    }
};

enum ck_wait_status
{
    CK_PROCESS_EXITED,
    CK_PROCESS_WAIT_TIMEDOUT
};

class ck_process
{
public:
    CK_API ck_process( const cppkit::ck_string& cmd );
    CK_API virtual ~ck_process() throw();
    CK_API void start();
    CK_API ck_pid get_pid() const { return _pid; }
    CK_API ck_wait_status wait_for( int& code, std::chrono::milliseconds timeout );
    CK_API void kill();
    CK_API size_t stdout_read(void* ptr, size_t size, size_t nmemb);
    CK_API bool stdout_eof();

    CK_API static ck_pid get_current_pid();
    CK_API static std::list<ck_pid> get_child_processes( const ck_pid& parentPID );
    CK_API static std::list<ck_pid> get_processes_for_module( const cppkit::ck_string& moduleName );

private:
    ck_pid _pid;
    cppkit::ck_string _cmd;
#ifdef IS_WINDOWS
    PROCESS_INFORMATION _pi;
#else
    int _iopipe[2];
    FILE* _stdout;
#endif
};

}

#endif
