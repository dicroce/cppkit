
#include "cppkit/ck_uuid_utils.h"
#include "cppkit/ck_exception.h"
#include <string.h>

#if defined(IS_DARWIN) || defined(IS_IOS)
#include <CoreFoundation/CFUUID.h>
#else
#include <uuid/uuid.h>
#endif

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
#if defined(IS_IOS) || defined(IS_DARWIN)
	auto newId = CFUUIDCreate(NULL);
	auto bytes = CFUUIDGetUUIDBytes(newId);
	CFRelease(newId);

    *uuid = bytes.byte0;
    ++uuid;
    *uuid = bytes.byte1;
    ++uuid;
    *uuid = bytes.byte2;
    ++uuid;
    *uuid = bytes.byte3;
    ++uuid;
    *uuid = bytes.byte4;
    ++uuid;
    *uuid = bytes.byte5;
    ++uuid;
    *uuid = bytes.byte6;
    ++uuid;
    *uuid = bytes.byte7;
    ++uuid;
    *uuid = bytes.byte8;
    ++uuid;
    *uuid = bytes.byte9;
    ++uuid;
    *uuid = bytes.byte10;
    ++uuid;
    *uuid = bytes.byte11;
    ++uuid;
    *uuid = bytes.byte12;
    ++uuid;
    *uuid = bytes.byte13;
    ++uuid;
    *uuid = bytes.byte14;
    ++uuid;
    *uuid = bytes.byte15;
#else
	uuid_generate_random(uuid);
#endif
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

#if defined(IS_IOS) || defined(IS_DARWIN)
	char one[10], two[6], three[6], four[6], five[14];

	snprintf(one, 10, "%02x%02x%02x%02x",
		uuid[0], uuid[1], uuid[2], uuid[3]);
	snprintf(two, 6, "%02x%02x",
		uuid[4], uuid[5]);
	snprintf(three, 6, "%02x%02x",
		uuid[6], uuid[7]);
	snprintf(four, 6, "%02x%02x",
		uuid[8], uuid[9]);
	snprintf(five, 14, "%02x%02x%02x%02x%02x%02x",
		uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);
	const std::string sep("-");
	std::string out(one);

	out += sep + two;
	out += sep + three;
	out += sep + four;
	out += sep + five;

	return out;
#else
    uuid_unparse_lower(uuid, str);
#endif
    return str;

}

ck_uuid cppkit::ck_uuid_utils::s_to_uuid(const std::string& uuidS)
{
    ck_uuid output;
    ck_uuid_utils::s_to_uuid(uuidS, &output[0]);
    return output;
}

#if defined(IS_IOS) || defined(IS_DARWIN)
// converts a single hex char to a number (0 - 15)
static unsigned char _hexDigitToChar(char ch)
{
	// 0-9
	if (ch > 47 && ch < 58)
		return ch - 48;

	// a-f
	if (ch > 96 && ch < 103)
		return ch - 87;

	// A-F
	if (ch > 64 && ch < 71)
		return ch - 55;

	return 0;
}

static bool _isValidHexChar(char ch)
{
	// 0-9
	if (ch > 47 && ch < 58)
		return true;

	// a-f
	if (ch > 96 && ch < 103)
		return true;

	// A-F
	if (ch > 64 && ch < 71)
		return true;

	return false;
}

// converts the two hexadecimal characters to an unsigned char (a byte)
static unsigned char _hexPairToChar(char a, char b)
{
	return hexDigitToChar(a) * 16 + hexDigitToChar(b);
}
#endif

void cppkit::ck_uuid_utils::s_to_uuid(const string& uuidS, uint8_t* uuid)
{
#if defined(IS_IOS) || defined(IS_DARWIN)
	char charOne = '\0';
	char charTwo = '\0';
	bool lookingForFirstChar = true;
	unsigned nextByte = 0;

	for (const char &ch : fromString)
	{
		if (ch == '-')
			continue;

		if (nextByte >= 16 || !_isValidHexChar(ch))
            CK_STHROW(ck_invalid_argument_exception, ("Invalid uuid string (%s).", uuidS.c_str()));

		if (lookingForFirstChar)
		{
			charOne = ch;
			lookingForFirstChar = false;
		}
		else
		{
			charTwo = ch;
			auto byte = _hexPairToChar(charOne, charTwo);
			_bytes[nextByte++] = byte;
			lookingForFirstChar = true;
		}
	}

	// if there were fewer than 16 bytes in the string then guid is bad
	if (nextByte < 16)
        CK_STHROW(ck_invalid_argument_exception, ("Invalid uuid string (%s).", uuidS.c_str()));
#else
    if(uuid_parse(uuidS.c_str(), uuid) != 0)
        CK_STHROW(ck_invalid_argument_exception, ("Unable to parse uuid string (%s).", uuidS.c_str()));
#endif
}

int cppkit::ck_uuid_utils::uuid_cmp(const uint8_t* uu1, const uint8_t* uu2)
{
#if defined(IS_IOS) || defined(IS_DARWIN)
    return memcmp(uu1, uu2, 16);
#else
    return uuid_compare(uu1, uu2);
#endif
}