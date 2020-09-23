#include "cppkit/ck_file_lock.h"
#include "cppkit/ck_exception.h"
#include <sys/file.h>
#include <utility>

using namespace cppkit;
using namespace std;

ck_file_lock::ck_file_lock(int fd) :
    _fd(fd)
{
}

ck_file_lock::ck_file_lock(ck_file_lock&& obj) noexcept :
    _fd(std::move(obj._fd))
{
}

ck_file_lock& ck_file_lock::operator=(ck_file_lock&& obj) noexcept
{
    _fd = std::move(obj._fd);
    obj._fd = -1;

    return *this;
}

void ck_file_lock::lock(bool exclusive)
{
    if(flock(_fd, (exclusive)?LOCK_EX:LOCK_SH) < 0)
        CK_STHROW(ck_internal_exception, ("Unable to flock() file."));
}

void ck_file_lock::unlock()
{
    if(flock(_fd, LOCK_UN) < 0)
        CK_STHROW(ck_internal_exception, ("Unable to un-flock()!"));
}

ck_file_lock_guard::ck_file_lock_guard(ck_file_lock& lok, bool exclusive) :
    _lok(lok)
{
    _lok.lock(exclusive);
}

ck_file_lock_guard::~ck_file_lock_guard() noexcept
{
    _lok.unlock();
}
