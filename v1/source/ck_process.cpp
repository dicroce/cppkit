
#include "cppkit/ck_process.h"
#include "cppkit/ck_path.h"
#include "cppkit/os/ck_error_msg.h"
#include "cppkit/os/ck_time_utils.h"
#ifdef IS_WINDOWS
#include <tlhelp32.h>
#else
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#endif
#include <vector>

using namespace cppkit;
using namespace std;
using namespace std::chrono;

ck_process::ck_process( const ck_string& cmd ) :
    _pid(),
    _cmd( cmd ),
#ifdef IS_WINDOWS
    _pi()
#else
    _iopipe(),
    _stdout(NULL)
#endif
{
#ifdef IS_WINDOWS
    ZeroMemory(&_pi, sizeof(_pi));
#endif
}

ck_process::~ck_process() throw()
{
    if( _pid.valid() )
    {
#ifdef IS_LINUX
        int status;
        waitpid(_pid.pid, &status, 0);

		if (_stdout)
			fclose(_stdout);
#else
        WaitForSingleObject( _pi.hProcess, INFINITE );

	    DWORD exitCode = 0;
	    GetExitCodeProcess( _pi.hProcess, &exitCode );

	    CloseHandle( _pi.hProcess );
	    CloseHandle( _pi.hThread );
#endif
    }
}

void ck_process::start()
{
#ifdef IS_WINDOWS

    STARTUPINFO startupInfo;
    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);

    std::wstring wideFullPath = ck_string::convert_multi_byte_string_to_wide_string( _cmd );

    TCHAR lpszClientPath[500];
    memset( lpszClientPath, 0, 500 * sizeof(TCHAR) );
    memcpy( lpszClientPath, wideFullPath.c_str(), wideFullPath.size() * sizeof(wchar_t) );

    CreateProcess( NULL,
		   lpszClientPath,
		   NULL,
		   NULL,
		   FALSE,
		   0,
		   NULL,
		   NULL,
		   &startupInfo,
		   &_pi );

    _pid.pid = _pi.dwProcessId;

#else

    if( pipe(_iopipe) < 0 )
        CK_THROW(("Unable to open pipe."));

    _pid.pid = fork();
    if( _pid.pid < 0 )
        CK_THROW(("Unable to fork()."));

    if( _pid.pid == 0 ) // 0 is returned in child...
    {
        setpgid(0, 0); // 0 here is special case that means set pgid to pid of caller.

        dup2(_iopipe[1], STDOUT_FILENO);
        close(_iopipe[0]);
        close(_iopipe[1]);

        vector<ck_string> parts;

        // work left to right
        // if you see whitespace, make command part
        // if you see quote, find terminating quote and make command part

        size_t ps=0, pe=0, cmdLen = _cmd.length();
        while(ps < cmdLen)
        {
            bool inQuote = false;

            while(_cmd[ps] == ' ' && ps < cmdLen)
            {
                ++ps;
                pe=ps;
            }

            if(_cmd[ps] == '"')
                inQuote = true;

            while((inQuote || _cmd[pe] != ' ') && pe < cmdLen)
            {
                ++pe;
                if(_cmd[pe] == '"')
                {
                    if(inQuote)
                    {
                        // push command ps+1 -> pe
                        parts.push_back(ck_string(&_cmd[ps+1],pe-(ps+1)));
                        inQuote = false;
                        ps = pe+1;
                        break;
                    }
                    else
                    {
                        inQuote = true;
                    }
                }
                else if((!inQuote && _cmd[pe] == ' ') || pe == cmdLen)
                {
                    // push command ps -> pe
                    parts.push_back(ck_string(&_cmd[ps],pe-ps));
                    ps = pe;
                    break;
                }
            }
        }

        vector<const char*> partPtrs;
        for( auto& p : parts )
            partPtrs.push_back(p.c_str());
        partPtrs.push_back(NULL);
        execve( parts[0].c_str(), (char* const*)&partPtrs[0], NULL );
        CK_THROW(("Failed to execve()."));
    }
    else
    {
        close(_iopipe[1]);
        _stdout = fdopen(_iopipe[0], "rb");
        if(!_stdout)
            CK_THROW(("Unable to open process stdout."));
    }

#endif
}

ck_wait_status ck_process::wait_for( int& code, milliseconds timeout )
{
#ifdef IS_WINDOWS
    DWORD res = WaitForSingleObject( _pi.hProcess, timeout.count() );

    if( res == WAIT_OBJECT_0 )
    {
        DWORD exitCode;
        if( GetExitCodeProcess( _pi.hProcess, &exitCode ) == 0 )
            CK_THROW(("Unable to retrieve process exit code!"));

        code = exitCode;

	    CloseHandle( _pi.hProcess );
	    CloseHandle( _pi.hThread );

        _pid.clear();

        return CK_PROCESS_EXITED;
    }
    else if( res == WAIT_TIMEOUT )
        return CK_PROCESS_WAIT_TIMEDOUT;
    else CK_THROW(("Unable to WaitForSingleObject."));
#else
    auto remaining = timeout;
    while(duration_cast<milliseconds>(remaining).count() > 0)
    {
        auto before = steady_clock::now();

        int status;
        int res = waitpid(_pid.pid, &status, WNOHANG);
        if( res > 0 )
        {
            code = WEXITSTATUS(status);
            _pid.clear();
            return CK_PROCESS_EXITED;
        }

        if( res < 0 )
            CK_THROW(("Unable to waitpid()."));

        ck_usleep(250000);

        auto after = steady_clock::now();

        auto delta = duration_cast<milliseconds>(after - before);

        if( remaining > delta )
            remaining -= delta;
        else remaining = milliseconds::zero();
    }

    return CK_PROCESS_WAIT_TIMEDOUT;
#endif
}

void ck_process::kill()
{
#ifdef IS_WINDOWS
    TerminateProcess( _pi.hProcess, -1 );
#else
    ::kill(-_pid.pid, SIGKILL); //negated pid means kill entire process group id
#endif
}

size_t ck_process::stdout_read(void* ptr, size_t size, size_t nmemb)
{
#ifdef IS_WINDOWS
    CK_THROW(("ck_process::stdout_read() not implemented on windows yet."));
#else
    if(!_stdout)
        CK_THROW(("Cannot read from null stream."));
    return fread(ptr, size, nmemb, _stdout);
#endif
}

bool ck_process::stdout_eof()
{
#ifdef IS_WINDOWS
    CK_THROW(("ck_process::stdout_eof() not implemented on windows yet."));
#else
    return (feof(_stdout) > 0) ? true : false;
#endif
}

ck_pid ck_process::get_current_pid()
{
    ck_pid p;
#ifdef IS_WINDOWS
    p.pid = GetCurrentProcessId();
#else
    p.pid = getpid();
#endif
    return p;
}

list<ck_pid> ck_process::get_child_processes( const ck_pid& parentPID )
{
    list<ck_pid> output;

#ifdef IS_WINDOWS
    HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
    PROCESSENTRY32 pe32;

    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    if( hProcessSnap == INVALID_HANDLE_VALUE )
      CK_THROW(("Unable to take toolhelp snapshot."));

    try
    {
        pe32.dwSize = sizeof( PROCESSENTRY32 );

        if( !Process32First( hProcessSnap, &pe32 ) )
            CK_THROW(("Unable to example process."));

	    do
	    {
	        if( pe32.th32ParentProcessID == parentPID.pid )
    	    {
	            ck_pid pid;
		        pid.pid = pe32.th32ProcessID;
    		    output.push_back( pid );
	        }
	    } while( Process32Next( hProcessSnap, &pe32 ) );

	    CloseHandle( hProcessSnap );
    }
    catch(...)
    {
	    if( hProcessSnap != INVALID_HANDLE_VALUE )
	        CloseHandle( hProcessSnap );
        throw;
    }

#else

    ck_path path( "/proc" );

    ck_string fileName;
    while( path.read_dir( fileName ) )
    {
        if( fileName.is_integer() && ck_path::is_dir( "/proc/" + fileName ) )
        {
            ck_string statusPath = "/proc/" + fileName + "/status";
            if( ck_path::exists( statusPath ) )
            {
                FILE* inFile = fopen( statusPath.c_str(), "rb" );
		        if( !inFile )
		            CK_THROW(("Unable to open status file."));

        		char* lineBuffer = NULL;

        		try
        		{
  		            ssize_t bytesRead = 0;
		            do
		            {
		                lineBuffer = NULL;
			            size_t lineSize = 0;
			            bytesRead = getline( &lineBuffer, &lineSize, inFile );

			            if( bytesRead < 0 )
			                continue;

			            ck_string line = lineBuffer;

			            vector<ck_string> parts = line.split( ':' );

			            if( parts.size() == 2 )
			            {
			                if( parts[0] == "PPid" )
			                {
			                    ck_string foundPPID = parts[1].strip();

				                if( foundPPID.to_int() == parentPID.pid )
				                {
				                    ck_pid pid;
				                    pid.pid = fileName.to_int();
				                    output.push_back( pid );
				                }
			                }
			            }

			            if( lineBuffer )
			            {
			                free(lineBuffer);
			                lineBuffer = NULL;
			            }
                    }
		            while( bytesRead >= 0 );

		            fclose( inFile );
		        }
		        catch(...)
		        {
  		            if( inFile )
  		                fclose( inFile );
		            if( lineBuffer )
  		                free( lineBuffer );
		            throw;
		        }
            }
        }
    }

#endif

    return output;
}

list<ck_pid> ck_process::get_processes_for_module( const cppkit::ck_string& moduleName )
{
    list<ck_pid> output;

#ifdef IS_WINDOWS

    HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
    PROCESSENTRY32 pe32;

    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    if( hProcessSnap == INVALID_HANDLE_VALUE )
      CK_THROW(("Unable to take toolhelp snapshot."));

    try
    {
        pe32.dwSize = sizeof( PROCESSENTRY32 );

        if( !Process32First( hProcessSnap, &pe32 ) )
            CK_THROW(("Unable to example process."));

	    do
	    {
  	        ck_string module = pe32.szExeFile;

            if( module.contains( moduleName ) )
            {
		        ck_pid pid;
                pid.pid = pe32.th32ProcessID;
                output.push_back( pid );
            }
	    } while( Process32Next( hProcessSnap, &pe32 ) );

	    CloseHandle( hProcessSnap );
    }
    catch(...)
    {
	    if( hProcessSnap != INVALID_HANDLE_VALUE )
	        CloseHandle( hProcessSnap );
        throw;
    }

#else

    ck_path path( "/proc" );

    ck_string procName;
    while( path.read_dir( procName ) )
    {
        if( procName.is_integer() && ck_path::is_dir( "/proc/" + procName ) )
        {
            char exe[1024];
            memset(exe,0,1024);
            ck_string path = "/proc/" + procName + "/exe";
            int ret = readlink( path.c_str(), exe, sizeof(exe)-1 );
            ck_string module = exe;
            if( module.contains( moduleName ) )
            {
                ck_pid pid;
                pid.pid = procName.to_int();
                output.push_back( pid );
            }
        }
    }

#endif

    return output;
}

