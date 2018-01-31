
#include "utils.h"
#include "cppkit/os/ck_large_files.h"

using namespace cppkit;
using namespace std;

ck_string get_directory_from_url( const ck_string& url )
{
    size_t searchEnd = url.size() - 1;
    if( url.ends_with( '/' ) )
        searchEnd--;

    ck_string dir = url.substr( url.rfind( '/', searchEnd ) + 1 );

    if( dir.ends_with( '/' ) )
        dir = dir.substr( 0, dir.length() - 1 );

    return dir;
}

ck_string exec_and_get_output( const ck_string& cmd )
{
    FILE* output = NULL;

#ifdef IS_WINDOWS
    output = _popen( cmd.c_str(), "rb" );
#else
    output = popen( cmd.c_str(), "r" );
#endif

    if( !output )
        CK_THROW(("Unable to popen() cmd = %s",cmd.c_str()));

    ck_string result;

    while( !feof( output ) )
    {
        char line[4096];
        if( fgets( &line[0], 4096, output ) != NULL )
            result += line;
    }

#ifdef IS_WINDOWS
    _pclose( output );
#else
    pclose( output );
#endif

    return result;
}

shared_ptr<ck_memory> read_file( const ck_string& path )
{
    struct ck_file_info fileInfo;
    if( ck_stat( path, &fileInfo ) < 0 )
        CK_THROW(("Unable to stat config."));

    FILE* inFile = fopen( path.c_str(), "r+b" );
    if( !inFile )
        CK_THROW(("Unable to open: %s", path.c_str()));

    uint32_t fileSize = (uint32_t)fileInfo.file_size;
    uint32_t blockSize = (uint32_t)fileInfo.optimal_block_size;

    size_t numBlocks = (fileSize > blockSize) ? fileSize / blockSize : 0;
    size_t remainder = (fileSize >= blockSize) ? (fileSize % blockSize) : fileSize;

    shared_ptr<ck_memory> result = make_shared<ck_memory>();
    uint8_t* dst = result->extend_data( fileSize ).get_ptr();

    while( numBlocks > 0 )
    {
        size_t itemsRead = fread( dst, blockSize, 1, inFile );
        if( itemsRead > 0 )
        {
            dst += (itemsRead * blockSize);
            numBlocks -= itemsRead;
        }
    }

    while( remainder > 0 )
    {
        size_t bytesRead = fread( dst, 1, remainder, inFile );
        if( bytesRead > 0 )
            remainder -= bytesRead;
    }

    return result;
}

ck_string read_file_as_string( const ck_string& path )
{
    shared_ptr<ck_memory> buffer = read_file( path );
    return ck_string( (char*)buffer->map().get_ptr(), buffer->size_data() );
}

void write_file_contents( const cppkit::ck_string& fileName, const cppkit::ck_string& fileContents )
{
    FILE* outFile = fopen( fileName.c_str(), "wb" );
    if( !outFile )
        CK_THROW(("Unable to open output file."));

    fwrite( fileContents.c_str(), 1, fileContents.length(), outFile );

    fclose( outFile );
}
