
#ifndef __cppkit_files_h
#define __cppkit_files_h

#include "cppkit/ck_string.h"

namespace cppkit
{
CK_API FILE* ck_fopen( const ck_string& path, const ck_string& mode );
}

#endif