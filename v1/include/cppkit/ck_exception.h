
/// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=--=-=-=-=-=-
/// cppkit - http://www.cppkit.org
/// Copyright (c) 2013, Tony Di Croce
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without modification, are permitted
/// provided that the following conditions are met:
///
/// 1. Redistributions of source code must retain the above copyright notice, this list of conditions and
///    the following disclaimer.
/// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
///    and the following disclaimer in the documentation and/or other materials provided with the
///    distribution.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
/// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
/// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
/// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
/// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
/// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
/// TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
/// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///
/// The views and conclusions contained in the software and documentation are those of the authors and
/// should not be interpreted as representing official policies, either expressed or implied, of the cppkit
/// Project.
/// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=--=-=-=-=-=-

#ifndef cppkit_exception_h
#define cppkit_exception_h

#include "cppkit/os/ck_exports.h"
#include "cppkit/os/ck_platform.h"
#include "cppkit/ck_logger.h"

#include <string>
#include <exception>
#include <assert.h>

namespace cppkit
{

class ck_exception : public std::exception
{
public:
    CK_API ck_exception();

    CK_API ck_exception(const std::string& msg);

    CK_API ck_exception(const char* msg, ...);

    CK_API virtual ~ck_exception() throw();

    CK_API virtual const char* what() const throw();

    /// This is used by the throwing macros so that derived exceptions can have their names used in
    /// their messages rather than ck_exception.
    CK_API void set_type_name(const std::string& typeName);
    CK_API const char* get_type_name() const;

    CK_API const char* get_msg() const;
    CK_API void set_msg(std::string msg);

    CK_API void set_throw_point(int line, const char* file);
    CK_API int get_line_num() const { return _line_num; }
    CK_API const char* get_src_file() const { return _src_file.c_str(); }

    CK_API const char* get_stack() const { return _stack.c_str(); }
    CK_API void set_stack( const std::string& stack ) { _stack = stack; }

private:
    std::string _type_name;
    std::string _msg;
    int _line_num;
    std::string _src_file;
    std::string _stack;
    mutable std::string _what_msg;
};

}

#define CK_THROW(PARAMS) \
CK_MACRO_BEGIN \
    cppkit::ck_exception __exception PARAMS ; \
    __exception.set_throw_point(__LINE__, __FILE__); \
    throw __exception; \
CK_MACRO_END

/// Puts double quotes around its argument.
/// ENQUOTE_EXPAND is required if you want to pass a macro to ENQUOTE.
#define ENQUOTE_EXPAND(tok) #tok
#define ENQUOTE(tok) ENQUOTE_EXPAND(tok)

#define CK_STHROW(ETYPE, PARAMS)                           \
CK_MACRO_BEGIN                                             \
    ETYPE _exception PARAMS ;                              \
    _exception.set_throw_point(__LINE__, __FILE__);        \
    _exception.set_type_name(ENQUOTE(ETYPE));              \
    throw _exception;                                      \
CK_MACRO_END

#define CK_LOG_CPPKIT_EXCEPTION(E) \
CK_MACRO_BEGIN \
    { \
        auto parts = cppkit::ck_string(E.what()).split('\n'); \
        for( auto l : parts ) \
            CK_LOG_ERROR("%s",l.c_str()); \
    } \
CK_MACRO_END

#define CK_LOG_STD_EXCEPTION(E) \
CK_MACRO_BEGIN \
    { \
        auto parts = cppkit::ck_string(E.what()).split('\n'); \
        for( auto l : parts ) \
            CK_LOG_ERROR("%s",l.c_str()); \
    } \
CK_MACRO_END

#endif
