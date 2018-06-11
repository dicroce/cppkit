
#ifndef cppkit_ck_socket_io_h
#define cppkit_ck_socket_io_h

#include <stdlib.h>

namespace cppkit
{

class ck_socket_io
{
public:
    virtual int raw_send(const void* buf, size_t len) = 0;
    virtual int raw_recv(void* buf, size_t len) = 0;
};

}

#endif
