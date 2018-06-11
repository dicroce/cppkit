
#include "cppkit/ck_uuid.h"

#ifdef IS_WINDOWS
#include "Objbase.h"
#else
#include <uuid/uuid.h>
#endif

using namespace std;
using namespace cppkit;

ck_string cppkit::ck_uuid_string_generate()
{
#ifdef IS_WINDOWS
    RPC_WSTR str = NULL;
    GUID g;
    if( CoCreateGuid(&g) != S_OK )
        CK_THROW(("Unable to create GUID."));
    if( UuidToStringW(&g, &str) != RPC_S_OK )
        CK_THROW(("Unable to convert GUID to string."));
    ck_string output( (wchar_t*)str );
    RpcStringFreeW( &str );
    return output;
#else
    char str[37];
    uuid_t id;
    uuid_generate_random( id );
    uuid_unparse( id, str );
    return str;
#endif
}

bool cppkit::ck_uuid_string_valid(const ck_string& uuid)
{
    // XXX Note: This is a fairly shitty uuid validator. It only checks for
    // length and that the dashes are in the right place. This could bite us, 
    // but I doubt it... hopefully I'm right. :)
    if(uuid.length() != 36)
        return false;
    if(uuid[8]!='-' || uuid[13]!='-' || uuid[18]!='-' || uuid[23]!='-')
        return false;

    return true;
}