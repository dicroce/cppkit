
#ifndef cppkit_ck_file_h
#define cppkit_ck_file_h

#define _LARGE_FILE_API
#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include "cppkit/ck_string_utils.h"
#include "cppkit/ck_exception.h"
#include <stdlib.h>
#include <memory>
#include <vector>
#include <future>

class test_cppkit_ck_file;

namespace cppkit
{

class ck_file final
{
    friend class ::test_cppkit_ck_file;
public:
    ck_file() : _f(nullptr) {}
    ck_file(const ck_file&) = delete;
	ck_file(ck_file&& obj) noexcept;
	~ck_file() noexcept;

    ck_file& operator = (ck_file&) = delete;
	ck_file& operator = (ck_file&& obj) noexcept;

	operator FILE*() const { return _f; }

	static ck_file open(const std::string& path, const std::string& mode)
    {
        ck_file obj;
        obj._f = fopen(path.c_str(), mode.c_str());
        if(!obj._f)
            CK_STHROW(ck_not_found_exception, ("Unable to open: %s",path.c_str()));
        return obj;
    }

	void close() { fclose(_f); _f = nullptr; }

private:
    FILE* _f;
};

class ck_path final
{
    struct path_parts
    {
        std::string path;
        std::string glob;
    };

 public:
    ck_path(const std::string& glob);
    ck_path(const ck_path&) = delete;
    ck_path(ck_path&& obj) noexcept;

    ~ck_path() noexcept;

    ck_path& operator=(const ck_path&) = delete;
    ck_path& operator=(ck_path&& obj) noexcept;

    void open_dir(const std::string& glob);
    bool read_dir(std::string& fileName);

 private:
    void _clear() noexcept;
    path_parts _get_path_and_glob(const std::string& glob) const;

    path_parts _pathParts;
    bool _done;
    DIR* _d;
};

namespace ck_fs
{

extern const std::string PATH_SLASH;

enum ck_file_type
{
    CK_REGULAR,
    CK_DIRECTORY
};

struct ck_file_info
{
    std::string file_name;
    uint64_t file_size;
    ck_file_type file_type;
    uint32_t optimal_block_size;
    std::chrono::system_clock::time_point access_time;
    std::chrono::system_clock::time_point modification_time;
};

int stat(const std::string& fileName, struct ck_file_info* fileInfo, bool throwOnError = true);
std::vector<uint8_t> read_file(const std::string& path);
void write_file(const uint8_t* bytes, size_t len, const std::string& path);
void atomic_rename_file(const std::string& oldPath, const std::string& newPath);
bool file_exists(const std::string& path);
bool is_reg(const std::string& path);
bool is_dir(const std::string& path);
int fallocate(FILE* file, uint64_t size);
void break_path(const std::string& path, std::string& dir, std::string& fileName);
std::string temp_file_name(const std::string& dir, const std::string& baseName = std::string());
void get_fs_usage(const std::string& path, uint64_t& size, uint64_t& free);
uint64_t file_size(const std::string& path);
void mkdir(const std::string& path);

// GLIBC fwrite() attempts to write the requested number of bytes and returns the number of items
// successfully written. If GLIBC fwrite() is unable to write any bytes at all the error flag is
// set on the stream. NOTE: It looks like fwrite() can return 0 items written but have actually written
// some data to the file!
template<typename T, typename F>
void write_file(const T* p, size_t size, F& f, size_t blockSize = 4096)
{
    size_t blocks = (size >= blockSize)?size / blockSize:0;
    size_t remainder = (size >= blockSize)?size % blockSize:size;
    const uint8_t* writeHead = (const uint8_t*)p;

    while(blocks > 0)
    {
        auto blocksWritten = fwrite((void*)writeHead, blockSize, blocks, f);
        if(blocksWritten == 0)
            CK_THROW(("Unable to write to file."));
        blocks -= blocksWritten;
        writeHead += (blocksWritten * blockSize);
    }

    while(remainder > 0)
    {
        auto bytesWritten = fwrite((void*)writeHead, 1, remainder, f);
        if(bytesWritten == 0)
            CK_THROW(("Could not write to file."))
        remainder -= bytesWritten;
        writeHead += bytesWritten;
    }
}

// GLIBC fread() loops until it has read the requested number of bytes OR the lower level __read
// returns 0. If the lower level __read returns 0 the EOF flag is set on the stream.
template<typename T, typename F>
void read_file(T* p, size_t size, F& f, size_t blockSize = 4096)
{
    size_t blocks = (size >= blockSize)?size / blockSize:0;
    size_t remainder = (size >= blockSize)?size % blockSize:size;
    uint8_t* readHead = (uint8_t*)p;

    if(blocks > 0)
    {
        auto blocksRead = fread((void*)readHead, blockSize, blocks, f);
        if(blocksRead != blocks)
            CK_THROW(("Short fread(). Implies feof()."));
        readHead += (blocksRead * blockSize);
    }

    if(remainder > 0)
    {
        auto bytesRead = fread((void*)readHead, 1, remainder, f);
        if(bytesRead != remainder)
            CK_THROW(("Short fread(). Indicates feof()"));
    }
}

}

}

#endif
