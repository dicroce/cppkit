
#ifndef cppkit_ck_uuid_utils_h
#define cppkit_ck_uuid_utils_h

#include <string>
#include <vector>
#include <uuid/uuid.h>

namespace cppkit
{

namespace ck_uuid_utils
{

void generate(uint8_t* uuid);
std::string generate();
std::string uuid_to_s(const uint8_t* uuid);
void s_to_uuid(const std::string& uuidS, uint8_t* uuid);

}

}

#endif
