
#ifndef __buildy_option_h
#define __buildy_option_h

#include "cppkit/ck_string.h"
#include <list>

struct option
{
    cppkit::ck_string opt;
    cppkit::ck_string arg;
};

std::list<struct option> parse_options( int argc, char* argv[] );

bool check_option( const std::list<struct option>& options, const cppkit::ck_string& opt, cppkit::ck_string& arg );

bool check_option( const std::list<struct option>& options, const cppkit::ck_string& opt );

#endif
