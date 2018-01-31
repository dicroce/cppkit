
#include "cppkit/ck_file.h"
#include "cppkit/os/ck_large_files.h"

using namespace cppkit;
using namespace std;

ck_file::ck_file( ck_file&& obj ) :
    _f( std::move(obj._f) )
{
    obj._f = nullptr;
}

ck_file::~ck_file() throw()
{
    if( _f )
        fclose(_f);
}

ck_file& ck_file::operator = ( ck_file&& obj )
{
    _f = std::move( obj._f );
    obj._f = nullptr;
    return *this;
}

vector<uint8_t> ck_file::read_file(const ck_string& path)
{
    struct ck_file_info fi;
    ck_stat(path, &fi);

    uint32_t numBlocks = (fi.file_size > fi.optimal_block_size) ? fi.file_size / fi.optimal_block_size : 0;
    uint32_t remainder = (fi.file_size > fi.optimal_block_size) ? fi.file_size % fi.optimal_block_size : fi.file_size;

    vector<uint8_t> out(fi.file_size);
    uint8_t* writer = &out[0];

    auto f = ck_file::open(path, "rb");

    while(numBlocks > 0)
    {
        auto blocksRead = fread(writer, fi.optimal_block_size, numBlocks, f);
        writer += blocksRead * fi.optimal_block_size;
        numBlocks -= blocksRead;
    }

    if(remainder > 0)
        fread(writer, 1, remainder, f);

    return out;
}

void ck_file::write_file(const uint8_t* bytes, size_t len, const ck_string& path)
{
    auto f = ck_file::open(path, "w+b");
    struct ck_file_info fi;
    ck_stat(path, &fi);

    uint32_t numBlocks = (len > fi.optimal_block_size) ? len / fi.optimal_block_size : 0;
    uint32_t remainder = (len > fi.optimal_block_size) ? len % fi.optimal_block_size : len;

    while(numBlocks > 0)
    {
        auto blocksWritten = fwrite(bytes, fi.optimal_block_size, numBlocks, f);
        bytes += blocksWritten * fi.optimal_block_size;
        numBlocks -= blocksWritten;
    }

    if(remainder > 0)
        fwrite(bytes, 1, remainder, f);
}

void ck_file::atomic_rename_file(const ck_string& oldPath, const ck_string& newPath)
{
    if(rename(oldPath.c_str(), newPath.c_str()) < 0)
        CK_THROW(("Unable to rename %s to %s", oldPath.c_str(), newPath.c_str()));
}
