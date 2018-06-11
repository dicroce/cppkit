
#ifndef cppkit_ck_pollable_h
#define cppkit_ck_pollable_h

#include <cstdint>

namespace cppkit
{

class ck_pollable
{
public:
    virtual bool wait_till_recv_wont_block(uint64_t& millis) const = 0;
    virtual bool wait_till_send_wont_block(uint64_t& millis) const = 0;
};

}

#endif
