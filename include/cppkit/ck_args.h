
#ifndef cppkit_ck_args_h
#define cppkit_ck_args_h

#include "cppkit/ck_nullable.h"
#include <string>
#include <vector>

namespace cppkit
{

namespace args
{

struct argument
{
    std::string opt;
    std::string arg;
};

std::vector<argument> parse_arguments(int argc, char* argv[] );

std::string get_required_argument(const std::vector<argument>& arguments, const std::string& opt, const std::string& msg = "");
ck_nullable<std::string> get_optional_argument(const std::vector<argument>& arguments, const std::string& opt);

bool check_argument(const std::vector<argument>& arguments, const std::string& opt, std::string& arg);
bool check_argument(const std::vector<argument>& arguments, const std::string& opt);

}

}

#endif
