
#include "cppkit/ck_uuid_utils.h"
#include "cppkit/ck_exception.h"
#include <string.h>

using namespace cppkit;
using namespace std;

ck_uuid cppkit::ck_uuid_utils::generate()
{
    ck_uuid output;
    ck_uuid_utils::generate_in_place(&output[0]);
    return output;
}

void cppkit::ck_uuid_utils::generate_in_place(uint8_t* uuid)
{
    uuid_generate_random(uuid);
}

string cppkit::ck_uuid_utils::generate_s()
{
    return uuid_to_s(ck_uuid_utils::generate());
}

std::string cppkit::ck_uuid_utils::uuid_to_s(const ck_uuid& uuid)
{
    return ck_uuid_utils::uuid_to_s(&uuid[0]);
}

string cppkit::ck_uuid_utils::uuid_to_s(const uint8_t* uuid)
{
    char str[37];
    uuid_unparse_lower(uuid, str);
    return str;
}

ck_uuid cppkit::ck_uuid_utils::s_to_uuid(const std::string& uuidS)
{
    ck_uuid output;
    ck_uuid_utils::s_to_uuid(uuidS, &output[0]);
    return output;
}

void cppkit::ck_uuid_utils::s_to_uuid(const string& uuidS, uint8_t* uuid)
{
    if(uuid_parse(uuidS.c_str(), uuid) != 0)
        CK_STHROW(ck_invalid_argument_exception, ("Unable to parse uuid string (%s).", uuidS.c_str()));
}

int cppkit::ck_uuid_utils::uuid_cmp(const uint8_t* uu1, const uint8_t* uu2)
{
    return uuid_compare(uu1, uu2);
}