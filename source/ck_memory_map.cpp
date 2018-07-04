
#include "cppkit/ck_memory_map.h"
#include "cppkit/ck_exception.h"
#include "cppkit/ck_file.h"
#include <sys/mman.h>

using namespace cppkit;

ck_memory_map::ck_memory_map(ck_memory_map&& obj) noexcept :
    _mem(std::move(obj._mem)),
    _length(std::move(obj._length))
{
    obj._mem = NULL;
    obj._length = 0;
}

ck_memory_map::ck_memory_map(int fd, uint64_t offset, uint64_t len, uint32_t prot, uint32_t flags) :
    _mem(NULL),
    _length(len)
{
    if(fd <= 0)
        CK_THROW(("Attempting to memory map a bad file descriptor."));

    if(len == 0)
        CK_THROW(("Attempting to memory map 0 bytes is invalid."));

    if(!(flags & MM_TYPE_FILE) && !(flags & MM_TYPE_ANON))
        CK_THROW(("A mapping must be either a file mapping, or an anonymous mapping (neither was specified)."));

    if(flags & MM_FIXED)
        CK_THROW(("ck_memory_map does not support fixed mappings."));

#ifdef IS_LINUX
    _mem = mmap64(NULL, _length, _get_posix_prot_flags(prot), _get_posix_access_flags(flags), fd, offset);
#else
    _mem = mmap(NULL, _length, _get_posix_prot_flags(prot), _get_posix_access_flags(flags), fd, offset);
#endif
}

ck_memory_map::~ck_memory_map() noexcept
{
    _close();
}

ck_memory_map& ck_memory_map::operator = (ck_memory_map&& obj) noexcept
{
    _close();

    _mem = std::move(obj._mem);
    obj._mem = NULL;

    _length = std::move(obj._length);
    obj._length = 0;

	return *this;
}

void ck_memory_map::advise(void* addr, size_t length, int advice) const
{
    int posixAdvice = _get_posix_advice(advice);

    int err = madvise(addr, length, posixAdvice);

    if(err != 0)
        CK_THROW(("Unable to apply memory mapping advice."));
}

void ck_memory_map::_close() noexcept
{
    munmap(_mem, _length);
}

int ck_memory_map::_get_posix_prot_flags(int prot) const
{
    int osProtFlags = 0;

    if(prot & MM_PROT_READ)
        osProtFlags |= PROT_READ;
    if(prot & MM_PROT_WRITE)
        osProtFlags |= PROT_WRITE;
    if(prot & MM_PROT_EXEC)
        osProtFlags |= PROT_EXEC;

    return osProtFlags;
}

int ck_memory_map::_get_posix_access_flags(int flags) const
{
    int osFlags = 0;

    if(flags & MM_TYPE_FILE)
        osFlags |= MAP_FILE;
    if(flags & MM_TYPE_ANON)
    {
#ifdef IS_LINUX
        osFlags |= MAP_ANONYMOUS;
#else
        osFlags |= MAP_ANON;
#endif
    }
    if(flags & MM_SHARED)
        osFlags |= MAP_SHARED;
    if(flags & MM_PRIVATE)
        osFlags |= MAP_PRIVATE;
    if(flags & MM_FIXED)
        osFlags |= MAP_FIXED;

    return osFlags;
}

int ck_memory_map::_get_posix_advice(int advice) const
{
    int posixAdvice = 0;

    if(advice & MM_ADVICE_RANDOM)
        posixAdvice |= MADV_RANDOM;
    if(advice & MM_ADVICE_SEQUENTIAL)
        posixAdvice |= MADV_SEQUENTIAL;
    if(advice & MM_ADVICE_WILLNEED)
        posixAdvice |= MADV_WILLNEED;
    if(advice & MM_ADVICE_DONTNEED)
        posixAdvice |= MADV_DONTNEED;

    return posixAdvice;
}
