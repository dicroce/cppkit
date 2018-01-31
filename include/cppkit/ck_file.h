

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

#ifndef cppkit_file_h
#define cppkit_file_h

#include "cppkit/ck_string.h"
#include <stdlib.h>
#include <memory>
#include <vector>

namespace cppkit
{

class ck_file final
{
public:
    CK_API ck_file() : _f( nullptr ) {}
	CK_API ck_file( ck_file&& obj );
	CK_API ~ck_file() throw();

	CK_API ck_file& operator = ( ck_file&& obj );

	CK_API operator FILE*() const { return _f; }

	CK_API static ck_file open(const ck_string& path, const ck_string& mode)
    {
        ck_file obj;
        obj._f = fopen(path.c_str(), mode.c_str());
        if( !obj._f )
            CK_THROW(("Unable to open: %s",path.c_str()));
        return obj;
    }

	CK_API void close() { fclose(_f); _f = nullptr; }

	CK_API static std::vector<uint8_t> read_file(const ck_string& path);
	CK_API static void write_file(const uint8_t* bytes, size_t len, const ck_string& path);
	CK_API static void atomic_rename_file(const ck_string& oldPath, const ck_string& newPath);

private:
    ck_file( const ck_file& ) = delete;
    ck_file& operator = ( const ck_file& ) = delete;

    FILE* _f;
};

}

#endif

