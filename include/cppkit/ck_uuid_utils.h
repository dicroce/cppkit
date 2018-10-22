
#ifndef cppkit_ck_uuid_utils_h
#define cppkit_ck_uuid_utils_h

#include <string>
#include <array>
#include <cstdint>
#include <uuid/uuid.h>

namespace cppkit
{

typedef std::array<uint8_t, 16> ck_uuid;

namespace ck_uuid_utils
{

void generate(uint8_t* uuid);
std::string generate();
std::string uuid_to_s(const uint8_t* uuid);
void s_to_uuid(const std::string& uuidS, uint8_t* uuid);
int uuid_cmp(const uint8_t* uu1, const uint8_t* uu2);

}

}

#endif
