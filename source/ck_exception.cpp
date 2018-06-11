
#include "cppkit/ck_exception.h"
#include "cppkit/ck_string_utils.h"
#include "cppkit/ck_stack_trace.h"

using namespace std;
using namespace cppkit;

ck_exception::ck_exception() :
    exception(),
    _msg(),
    _stack(ck_stack_trace::get_stack())
{
}

ck_exception::ck_exception(const string& msg) :
    exception(),
    _msg(msg),
    _stack(ck_stack_trace::get_stack())
{
}

ck_exception::ck_exception(const char* msg, ...) : 
    exception(),
    _msg(),
    _stack(ck_stack_trace::get_stack())
{
    va_list args;
    va_start(args, msg);
    _msg = ck_string_utils::format(msg, args);
    va_end(args);
}

ck_exception::~ck_exception() noexcept
{
}

const char* ck_exception::what() const noexcept
{
    // XXX Note: Since this method returns a char*, we MUST assign our _msg
    // member to the new string to guarantee the lifetime of the pointer to
    // be as long as this ck_exception itself.
    _msg = ck_string_utils::format("%s\n%s", _msg.c_str(), _stack.c_str());
    return _msg.c_str();
}

ck_not_found_exception::ck_not_found_exception() :
    ck_exception()
{
}

ck_not_found_exception::ck_not_found_exception(const char* msg, ...) :
    ck_exception()
{
    va_list args;
    va_start(args, msg);
    set_msg(ck_string_utils::format(msg, args));
    va_end(args);
}

ck_invalid_argument_exception::ck_invalid_argument_exception() :
    ck_exception()
{
}

ck_invalid_argument_exception::ck_invalid_argument_exception(const char* msg, ...) :
    ck_exception()
{
    va_list args;
    va_start(args, msg);
    set_msg(ck_string_utils::format(msg, args));
    va_end(args);
}

ck_unauthorized_exception::ck_unauthorized_exception() :
    ck_exception()
{
}

ck_unauthorized_exception::ck_unauthorized_exception(const char* msg, ...) :
    ck_exception()
{
    va_list args;
    va_start(args, msg);
    set_msg(ck_string_utils::format(msg, args));
    va_end(args);
}

ck_not_implemented_exception::ck_not_implemented_exception() :
    ck_exception()
{
}

ck_not_implemented_exception::ck_not_implemented_exception(const char* msg, ...) :
    ck_exception()
{
    va_list args;
    va_start(args, msg);
    set_msg(ck_string_utils::format(msg, args));
    va_end(args);
}

ck_timeout_exception::ck_timeout_exception() :
    ck_exception()
{
}

ck_timeout_exception::ck_timeout_exception(const char* msg, ...) :
    ck_exception()
{
    va_list args;
    va_start(args, msg);
    set_msg(ck_string_utils::format(msg, args));
    va_end(args);
}

ck_io_exception::ck_io_exception() :
    ck_exception()
{
}

ck_io_exception::ck_io_exception(const char* msg, ...) :
    ck_exception()
{
    va_list args;
    va_start(args, msg);
    set_msg(ck_string_utils::format(msg, args));
    va_end(args);
}

ck_internal_exception::ck_internal_exception() :
    ck_exception()
{
}

ck_internal_exception::ck_internal_exception(const char* msg, ...) :
    ck_exception()
{
    va_list args;
    va_start(args, msg);
    set_msg(ck_string_utils::format(msg, args));
    va_end(args);
}
