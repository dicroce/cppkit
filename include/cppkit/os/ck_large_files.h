
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

#ifndef cppkit_large_files_h
#define cppkit_large_files_h

#define _LARGE_FILE_API
#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "cppkit/os/ck_exports.h"
#include "cppkit/ck_types.h"
#include "cppkit/ck_string.h"

#ifdef IS_POSIX
#include <unistd.h>
#else
#include <direct.h>
#endif

#include <future>

namespace cppkit
{

enum ck_file_type
{
    CK_REGULAR,
    CK_DIRECTORY
};

struct ck_file_info
{
    ck_string file_name;
    uint64_t file_size;
    ck_file_type file_type;
    uint32_t optimal_block_size;
    std::chrono::system_clock::time_point access_time;
    std::chrono::system_clock::time_point modification_time;
};

CK_API int ck_stat( const ck_string& fileName, struct ck_file_info* fileInfo );

CK_API int ck_fseeko( FILE* file, uint64_t offset, int whence );

CK_API uint64_t ck_ftello( FILE* file );

CK_API int ck_filecommit( FILE* file );

CK_API int ck_fallocate( FILE* file, uint64_t size );

/// Returns true if the given file exists, otherwise returns false
CK_API bool ck_exists( const ck_string& fileName );

/// Returns a directory for putting temporary files.
CK_API ck_string ck_temp_dir();

CK_API void ck_disk_usage( const ck_string& path, uint64_t& size, uint64_t& free );

CK_API void ck_mkdir( const ck_string& name );

CK_API void ck_rmdir( const ck_string& name );

CK_API void ck_unlink( const ck_string& path );

CK_API ck_string ck_current_directory();

}

#endif
