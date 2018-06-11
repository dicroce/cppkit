
#include "cppkit/ck_stack_trace.h"

#include <stdlib.h>
#include <cxxabi.h>
#include <execinfo.h>

using namespace cppkit;
using namespace std;

string cppkit::ck_stack_trace::get_stack(char sep)
{
    void* trace[256];
    int traceSize = ::backtrace(trace, 256);
    char** buffer = ::backtrace_symbols(trace, traceSize);

    string stack;
    for( int i = 1; buffer && i < traceSize-1; i++ )
    {
        stack += "[#" + to_string((traceSize-1)-i) + "] ";
        string s = buffer[i];
        string mang = s.substr(s.find('(') + 1, s.find('+') - s.find('(') - 1);

        if( char* demangled = abi::__cxa_demangle(mang.c_str(), 0, 0, 0) )
        {
            string dm = demangled;
            free( demangled );
            // remove "std::" from names...
            //for( size_t i = dm.find("std::"); i != string::npos; i = dm.find("std::") )
            //    dm.erase(i, 5);

            stack += dm;
        } else stack += mang;

        stack += sep;
    }

    if(buffer)
        free(buffer);

    return stack;
}

vector<string> cppkit::ck_stack_trace::get_stack_frame_names()
{
    vector<string> stack;

    const int size = 256;
    char** buffer = nullptr;
    int traceSize = 0;
    void* trace[size];

    traceSize = backtrace(trace, size);
    buffer = backtrace_symbols(trace, traceSize);

    for( int i = 0; i < traceSize; i++ )
    {
        string stackFrame = buffer[i];

        size_t plusIndex = stackFrame.find( "+" );
        size_t nameStart = stackFrame.rfind( ' ', plusIndex - 2 ) + 1;

        string name = stackFrame.substr( nameStart, (plusIndex - 1) - nameStart );

        size_t mangledStart = name.find( "_Z" );
        if( mangledStart != std::string::npos )
        {
            int status = 0;
            char* demangled = abi::__cxa_demangle(name.c_str(), nullptr, nullptr, &status);

            if( demangled )
            {
                string dm = demangled;
                free( demangled );
                // remove "std::" from names...
                for( size_t i = dm.find("std::"); i != string::npos; i = dm.find("std::") )
                    dm.erase(i, 5);

                stack.push_back(dm);
            }
            else stack.push_back( string( name ) );
        }
        else stack.push_back( string( name ) );
    }

    free(buffer);

    return stack;
}
