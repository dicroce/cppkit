
#ifndef cppkit_ck_exception_h
#define cppkit_ck_exception_h

#include "cppkit/ck_logger.h"
#include "cppkit/ck_macro.h"
#include <string>
#include <exception>
#include <assert.h>

namespace cppkit
{

class ck_exception : public std::exception
{
public:
    ck_exception();
    ck_exception(const std::string& msg);
    ck_exception(const char* msg, ...);
    virtual ~ck_exception() noexcept;

    void set_msg(const std::string& msg) { _msg = msg; }

    virtual const char* what() const noexcept;

protected:
    mutable std::string _msg;
    std::string _stack;
};

class ck_not_found_exception : public ck_exception
{
public:
    ck_not_found_exception();
    virtual ~ck_not_found_exception() noexcept {}
    ck_not_found_exception(const char* msg, ...);
};

class ck_invalid_argument_exception : public ck_exception
{
public:
    ck_invalid_argument_exception();
    virtual ~ck_invalid_argument_exception() noexcept {}
    ck_invalid_argument_exception(const char* msg, ...);
};

class ck_unauthorized_exception : public ck_exception
{
public:
    ck_unauthorized_exception();
    virtual ~ck_unauthorized_exception() noexcept {}
    ck_unauthorized_exception(const char* msg, ...);
};

class ck_not_implemented_exception : public ck_exception
{
public:
    ck_not_implemented_exception();
    virtual ~ck_not_implemented_exception() noexcept {}
    ck_not_implemented_exception(const char* msg, ...);
};

class ck_timeout_exception : public ck_exception
{
public:
    ck_timeout_exception();
    virtual ~ck_timeout_exception() noexcept {}
    ck_timeout_exception(const char* msg, ...);
};

class ck_io_exception : public ck_exception
{
public:
    ck_io_exception();
    virtual ~ck_io_exception() noexcept {}
    ck_io_exception(const char* msg, ...);
};

class ck_internal_exception : public ck_exception
{
public:
    ck_internal_exception();
    virtual ~ck_internal_exception() noexcept {}
    ck_internal_exception(const char* msg, ...);
};

class ck_coded_exception : public ck_exception
{
public:
    ck_coded_exception();
    virtual ~ck_coded_exception() noexcept {}
    ck_coded_exception(int code, const char* msg, ...);
    int get_code() const { return _code; }
private:
    int _code;
};

}

#define CK_THROW(ARGS) throw cppkit::ck_exception ARGS ;

#define CK_STHROW(EXTYPE, ARGS) throw EXTYPE ARGS ;

#define CK_LOG_EXCEPTION(E) \
CK_MACRO_BEGIN \
    auto parts = cppkit::ck_string_utils::split(std::string(E.what()), '\n'); \
    for( auto l : parts ) \
        CK_LOG_ERROR("%s",l.c_str()); \
CK_MACRO_END

#endif
