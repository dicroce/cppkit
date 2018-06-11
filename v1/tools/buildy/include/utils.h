
#ifndef __buildy_utils_h
#define __buildy_utils_h

#include "cppkit/ck_string.h"
#include "cppkit/ck_memory.h"
#include <memory>

cppkit::ck_string get_directory_from_url( const cppkit::ck_string& url );

cppkit::ck_string exec_and_get_output( const cppkit::ck_string& cmd );

std::shared_ptr<cppkit::ck_memory> read_file( const cppkit::ck_string& path );
cppkit::ck_string read_file_as_string( const cppkit::ck_string& path );

void write_file_contents( const cppkit::ck_string& fileName, const cppkit::ck_string& fileContents );

#endif
