
#include "cppkit/ck_file_lock.h"
#include "cppkit/os/ck_error_msg.h"

#ifdef IS_WINDOWS
#include <windows.h>
#include <io.h>
#else
#include <sys/file.h>
#endif

using namespace cppkit;
using namespace std;

ck_file_lock::ck_file_lock( int fd ) :
    _fd( fd )
{
}

ck_file_lock::ck_file_lock( ck_file_lock&& obj ) :
    _fd( std::move(obj._fd) )
{
}

ck_file_lock& ck_file_lock::operator = ( ck_file_lock&& obj )
{
    _fd = obj._fd;

    return *this;
}

void ck_file_lock::lock( bool exclusive )
{
#ifdef IS_WINDOWS
    HANDLE fileHandle = (HANDLE)_get_osfhandle( _fd );
    if( fileHandle == INVALID_HANDLE_VALUE )
        CK_THROW(("Unable to get OS handle."));

    OVERLAPPED overlapped;
    overlapped.Offset = 0;
    overlapped.OffsetHigh = 0;

    BOOL success = LockFileEx( fileHandle,
                               (exclusive)?LOCKFILE_EXCLUSIVE_LOCK:0,
                               0,
                               MAXDWORD,
                               MAXDWORD,
                               &overlapped );
    if( !success )
        CK_THROW(("Unable to acquire file lock: %s",ck_get_last_error_msg().c_str()));
#else
    int err = flock( _fd, (exclusive)?LOCK_EX:LOCK_SH );
    if( err < 0 )
        CK_THROW(("Unable to acquire file lock: %s",ck_get_last_error_msg().c_str()));
#endif
}

void ck_file_lock::unlock()
{
#ifdef IS_WINDOWS
    HANDLE fileHandle = (HANDLE)_get_osfhandle( _fd );
    if( fileHandle == INVALID_HANDLE_VALUE )
        CK_THROW(("Unable to get OS handle."));

    OVERLAPPED overlapped;
    overlapped.Offset = 0;
    overlapped.OffsetHigh = 0;

    BOOL success = UnlockFileEx( fileHandle,
                                 0,
                                 MAXDWORD,
                                 MAXDWORD,
                                 &overlapped );
    if( !success )
        CK_THROW(("Unable to release file lock: %s",ck_get_last_error_msg().c_str()));
#else
    int err = flock( _fd, LOCK_UN );
    if( err < 0 )
        CK_THROW(("Unable to release file lock: %s",ck_get_last_error_msg().c_str()));
#endif
}

ck_file_lock_guard::ck_file_lock_guard( ck_file_lock& lok, bool exclusive ) :
    _lok( lok )
{
    _lok.lock( exclusive );
}

ck_file_lock_guard::~ck_file_lock_guard() throw()
{
    _lok.unlock();
}
