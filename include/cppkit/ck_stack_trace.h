
#ifndef cppkit_ck_stack_trace_h
#define cppkit_ck_stack_trace_h

#include <string>
#include <vector>

namespace cppkit
{

namespace ck_stack_trace
{

std::string get_stack(char sep = '\n');
std::vector<std::string> get_stack_frame_names();

}

}

#endif
