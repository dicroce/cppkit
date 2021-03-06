
#ifndef cppkit_ck_uuid_utils_h
#define cppkit_ck_uuid_utils_h

#include <string>
#include <array>
#include <cstdint>

namespace cppkit
{

typedef std::array<uint8_t, 16> ck_uuid;

namespace ck_uuid_utils
{

ck_uuid generate();
void generate_in_place(uint8_t* uuid);
std::string generate_s();
std::string uuid_to_s(const ck_uuid& uuid);
std::string uuid_to_s(const uint8_t* uuid);
ck_uuid s_to_uuid(const std::string& uuidS);
void s_to_uuid(const std::string& uuidS, uint8_t* uuid);
int uuid_cmp(const uint8_t* uu1, const uint8_t* uu2);

}

}

#endif
