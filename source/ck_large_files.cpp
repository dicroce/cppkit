
#include "cppkit/os/ck_large_files.h"

using namespace cppkit;

#if defined(IS_POSIX)
#include <fcntl.h>
#include <unistd.h>
#include <sys/statvfs.h>
#endif

#if defined(IS_WINDOWS)
#include <io.h>
#endif

#ifdef IS_WINDOWS
#define FILENO _fileno
#else
#define FILENO fileno
#endif

#ifdef IS_WINDOWS

int cppkit::ck_stat( const ck_string& file_name, struct ck_file_info* file_info )
{
    // XXX - Needs wide character conversion

    ck_string path = (file_name.ends_with("\\")||file_name.ends_with("/"))?file_name.substr(0,file_name.length()-1):file_name;
    int result = -1;
    struct __stat64 sfi;

    if( _wstat64( path.get_wide_string().data(), &sfi ) == 0 )
    {
        file_info->file_name = path;
        file_info->file_size = sfi.st_size;
        file_info->file_type = (sfi.st_mode & _S_IFDIR) ? CK_DIRECTORY : CK_REGULAR;

        // Note: On Win32, we currently hard code this to a reasonable value.
        file_info->optimal_block_size = 4096;

        file_info->access_time = std::chrono::system_clock::from_time_t(sfi.st_atime);
        file_info->modification_time = std::chrono::system_clock::from_time_t(sfi.st_mtime);

        result = 0;;
    }
    return result;
}

int cppkit::ck_fseeko( FILE* file, uint64_t offset, int whence )
{
    return _fseeki64( file, offset, whence );
}

uint64_t cppkit::ck_ftello( FILE* file )
{
    return _ftelli64( file );
}

int cppkit::ck_filecommit( FILE* file )
{
    fflush(file);

    // Tell windows to commit all buffered file data to disk.
    int fd = _fileno(file);
    int err = _commit(fd);

    return err;
}

int cppkit::ck_fallocate( FILE* file, uint64_t size )
{
    return ( _chsize_s( FILENO( file ), size ) == 0) ? 0 : -1;
}

#else

int cppkit::ck_stat( const ck_string& file_name, struct ck_file_info* file_info )
{
    struct stat sfi;

    if( stat( file_name.c_str(), &sfi ) == 0 )
    {
        file_info->file_name = file_name;
        file_info->file_size = sfi.st_size;
        file_info->file_type = (sfi.st_mode & S_IFDIR) ? CK_DIRECTORY : CK_REGULAR;
        file_info->optimal_block_size = sfi.st_blksize;

        file_info->access_time = std::chrono::system_clock::from_time_t(sfi.st_atime);
        file_info->modification_time = std::chrono::system_clock::from_time_t(sfi.st_mtime);

        return 0;
    }

    return -1;
}

int cppkit::ck_fseeko( FILE* file, uint64_t offset, int whence )
{
    return fseeko( file, offset, whence );
}

uint64_t cppkit::ck_ftello( FILE* file )
{
    return ftello( file );
}

int cppkit::ck_filecommit( FILE* file )
{
    fflush(file);

    // sync all data associated with file stream to disk.
    int fd = fileno(file);

#if defined(IS_LINUX)
    fdatasync(fd);

    // advise kernel to dump cached data from memory.
    int err = posix_fadvise64(fd, 0, 0, POSIX_FADV_DONTNEED);

#else
    int err = fcntl( fd, F_FULLFSYNC );
#endif

    return err;
}

int cppkit::ck_fallocate( FILE* file, uint64_t size )
{
#if defined(IS_LINUX)
    return ( posix_fallocate64( fileno( file ), 0, size ) == 0 ) ? 0 : -1;
#else

    fstore_t store = {F_ALLOCATECONTIG, F_PEOFPOSMODE, 0, size};

    int ret = fcntl( fileno( file ), F_PREALLOCATE, &store );

    if( -1 == ret )
    {
        store.fst_flags = F_ALLOCATEALL;
	    ret = fcntl( fileno( file ), F_PREALLOCATE, &store );
    }

    return 0 == ftruncate( fileno( file ), size );

#endif
}

#endif

void cppkit::ck_disk_usage( const ck_string& path, uint64_t& size, uint64_t& free )
{
#ifdef IS_WINDOWS
    ULARGE_INTEGER winFree, winTotal;
    if( !GetDiskFreeSpaceExW(path.get_wide_string().data(), &winFree, &winTotal, 0) )
        CK_THROW(("Unable to query disk usage."));
    size = winTotal.QuadPart;
    free = winFree.QuadPart;
#else
    struct statvfs stat;
    if( statvfs(path.c_str(), &stat) != 0 )
        CK_THROW(("Unable to query disk usage."));
    size = (uint64_t)stat.f_blocks * (uint64_t)stat.f_frsize;
    free = (uint64_t)stat.f_bavail * (uint64_t)stat.f_bsize;
#endif
}

void cppkit::ck_mkdir( const ck_string& name )
{
    int ret;
#ifdef IS_POSIX
    ret = mkdir( name.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
#else
    ret = _mkdir( name.c_str() );
#endif
    if( ret < 0 )
        CK_THROW(("Unable to create directory: %s", name.c_str()));
}

void cppkit::ck_rmdir( const ck_string& name )
{
    int ret;
#ifdef IS_POSIX
    ret = rmdir( name.c_str() );
#else
    ret = _rmdir( name.c_str() );
#endif
    if( ret < 0 )
        CK_THROW(("Unable to remove directory: %s", name.c_str()));
}

void cppkit::ck_unlink( const ck_string& path )
{
    int ret;
#ifdef IS_POSIX
    ret = unlink( path.c_str() );
#else
    ret = _unlink( path.c_str() );
#endif
    if( ret < 0 )
        CK_THROW(("Unable to remove file: %s", path.c_str()));
}

ck_string cppkit::ck_current_directory()
{
  ck_string output;
#ifdef IS_POSIX
    char buffer[2048];
    if( getcwd( buffer, 2048 ) == NULL )
      CK_THROW(("Unable to get current directory."));
    output = buffer;
#else
    wchar_t buffer[2048];
    if( GetCurrentDirectory( 2048, buffer ) == 0 )
        CK_THROW(("Unable to get current directory."));
    output = ck_string::convert_wide_string_to_multi_byte_string( buffer );
#endif

    return output;
}
