
#ifndef cppkit_ck_memory_map_h
#define cppkit_ck_memory_map_h

#include "cppkit/ck_exception.h"
#include <cstdint>
#include <unistd.h>

namespace cppkit
{

class ck_memory_map
{
public:
    enum flags
    {
        MM_TYPE_FILE = 0x01,
        MM_TYPE_ANON = 0x02,
        MM_SHARED = 0x04,
        MM_PRIVATE = 0x08,
        MM_FIXED = 0x10
    };

    enum protection
    {
        MM_PROT_NONE = 0x00,
        MM_PROT_READ = 0x01,
        MM_PROT_WRITE = 0x02,
        MM_PROT_EXEC = 0x04
    };

    enum advice
    {
        MM_ADVICE_NORMAL = 0x00,
        MM_ADVICE_RANDOM = 0x01,
        MM_ADVICE_SEQUENTIAL = 0x02,
        MM_ADVICE_WILLNEED = 0x04,
        MM_ADVICE_DONTNEED = 0x08
    };

    ck_memory_map();
    ck_memory_map(const ck_memory_map& ob ) = delete;
    ck_memory_map(ck_memory_map&& obj) noexcept;
    ck_memory_map(int fd, uint64_t offset, uint64_t len, uint32_t prot, uint32_t flags, uint64_t mapOffset=0);

    virtual ~ck_memory_map() noexcept;

    ck_memory_map& operator = (const ck_memory_map&) = delete;
    ck_memory_map& operator = (ck_memory_map&& obj) noexcept;

    inline uint8_t* map(bool ignoreOffset = false) const
    {
        if(_mem == nullptr)
            CK_THROW(("Unable to map default constructed memory map."));
        return (ignoreOffset)?((uint8_t*)_mem):((uint8_t*)_mem) + _mapOffset;
    }

    inline uint64_t size(bool ignoreOffset = false) const
    {
        return (ignoreOffset)?_length:_length - _mapOffset;
    }

    void advise(void* addr, size_t length, int advice) const;

private:
    void _close() noexcept;
    int _get_posix_prot_flags(int prot) const;
    int _get_posix_access_flags(int flags) const;
    int _get_posix_advice(int advice) const;

    void* _mem;
    uint64_t _length;
    uint64_t _mapOffset;
};

}

#endif
