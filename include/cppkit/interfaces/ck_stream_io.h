
#ifndef cppkit_ck_stream_io_h
#define cppkit_ck_stream_io_h

#include <stdlib.h>

namespace cppkit
{

class ck_stream_io
{
public:
    virtual bool valid() const = 0;
    virtual void send(const void* buf, size_t len) = 0;
    virtual void recv(void* buf, size_t len) = 0;
};

}

#endif
