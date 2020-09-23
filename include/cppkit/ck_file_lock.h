#ifndef cppkit_ck_file_lock_h
#define cppkit_ck_file_lock_h

class test_cppkit_ck_file_lock;

namespace cppkit
{

class ck_file_lock
{
public:
    ck_file_lock(int fd=-1);
    ck_file_lock(const ck_file_lock&) = delete;
    ck_file_lock( ck_file_lock&& obj ) noexcept;

    ck_file_lock& operator=(const ck_file_lock&) = delete;
    ck_file_lock& operator=(ck_file_lock&& obj) noexcept;

    void lock(bool exclusive = true);
    void unlock();

private:
    int _fd;
};

class ck_file_lock_guard final
{
public:
    ck_file_lock_guard(ck_file_lock& lok, bool exclusive = true);
    ck_file_lock_guard(const ck_file_lock_guard&) = delete;
    ck_file_lock_guard(ck_file_lock&&) = delete;
    ~ck_file_lock_guard() noexcept;

    ck_file_lock_guard& operator=(const ck_file_lock_guard&) = delete;
    ck_file_lock_guard& operator=(ck_file_lock_guard&&) = delete;

private:
    ck_file_lock& _lok;
};

}

#endif
