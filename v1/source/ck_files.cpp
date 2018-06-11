#include "cppkit/os/ck_files.h"

using namespace cppkit;

FILE* cppkit::ck_fopen( const ck_string& path, const ck_string& mode )
{
    FILE* fp = NULL;
#ifdef IS_POSIX
    fp = fopen( path.c_str(), mode.c_str() );
    if ( !fp )
        CK_THROW(( "Unable to open file: %s\n", path.c_str() ));
#else
    errno_t err;

    if ( err = _wfopen_s( &fp, path.get_wide_string().c_str(), mode.get_wide_string().c_str() ) != 0 )
        CK_THROW(( "fopen_s returned error code: %d.\n", (int)err ));

    if ( !fp )
        CK_THROW(( "Unable to open file: %s\n", path.c_str() ));
#endif
    return fp;
}