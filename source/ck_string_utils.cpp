
#include "cppkit/ck_string_utils.h"
#include "cppkit/ck_exception.h"
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <array>
#include <algorithm>

using namespace cppkit;
using namespace std;

vector<string> cppkit::ck_string_utils::split(const string& str, char delim)
{
    return split(str, string(&delim, 1));
}

vector<string> cppkit::ck_string_utils::split(const string& str, const string& delim)
{
    vector<string> parts;

    size_t begin = 0;
    size_t end = 0;

    auto delimLen = delim.length();

    while(true)
    {
        end = str.find(delim, begin);

        if(end == string::npos)
        {
            if(str.begin()+begin != str.end())
                parts.emplace_back(str.begin()+begin, str.end());
            break;
        }

        if(end != begin)
            parts.emplace_back(str.begin()+begin, str.begin()+end);

        begin = end + delimLen;
    }

    return parts;
}

string cppkit::ck_string_utils::format(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const string result = format(fmt, args);
    va_end(args);
    return result;
}

string cppkit::ck_string_utils::format(const char* fmt, va_list& args)
{
    va_list newargs;
    va_copy(newargs, args);

    int chars_written = vsnprintf(nullptr, 0, fmt, newargs);
    int len = chars_written + 1;

    vector<char> str(len);

    va_copy(newargs, args);
    vsnprintf(&str[0], len, fmt, newargs);

    return string(&str[0]);
}

bool cppkit::ck_string_utils::format_buffer(char* buf, size_t size, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    auto result = format_buffer(buf, size, fmt, args);
    va_end(args);
    return result;
}

bool cppkit::ck_string_utils::format_buffer(char* buf, size_t size, const char* fmt, va_list& args)
{
    auto result = vsnprintf(buf, size, fmt, args);
    if(result < 0)
        CK_STHROW(ck_internal_exception, ("Unable to format string buffer."));
    return ((size_t)result >= size);
}

bool cppkit::ck_string_utils::contains(const string& str, const string& target)
{
    return (str.find(target) != string::npos) ? true : false;
}

string cppkit::ck_string_utils::erase_all(const string& str, char delim)
{
    return erase_all(str, string(1, delim));
}

string cppkit::ck_string_utils::erase_all(const string& str, const string& delim)
{
    auto splitList = cppkit::ck_string_utils::split( str, delim );

    string output;
    for(auto& p : splitList)
        output += p;

    return output;
}

string cppkit::ck_string_utils::replace_all(const string& str, char toBeReplaced, char toReplaceWith)
{
    return replace_all(str, string(1, toBeReplaced), string(1, toReplaceWith));
}

string cppkit::ck_string_utils::replace_all(const string& str, const string& toBeReplaced, const string& toReplaceWith)
{
    size_t found = str.find(toBeReplaced);

    if(found == string::npos)
        return str;

    string retval = str.substr(0, found);
    retval.append(toReplaceWith);

    while(1)
    {
        const size_t start = found + toBeReplaced.size();
        found = str.find(toBeReplaced, start);

        if(found != string::npos)
        {
            retval.append(str.substr(start, found - start));
            retval.append(toReplaceWith);
        }
        else
        {
            retval.append(str.substr(start));
            break;
        }
    }

    return retval;
}

string cppkit::ck_string_utils::to_lower(const string& str)
{
    string retval = str;
    transform(retval.begin(), retval.end(), retval.begin(), ::tolower);
    return retval;
}

string cppkit::ck_string_utils::to_upper(const string& str)
{
    string retval = str;
    transform(retval.begin(), retval.end(), retval.begin(), ::toupper);
    return retval;
}

static bool _is_valid_uri_character(char c)
{
    return isalnum(c) || c == '-' || c == '.' || c == '_' || c == '~';
}

string cppkit::ck_string_utils::uri_encode(const string& str)
{
    string retval;

    if(!str.empty())
    {
        char c;

        for(size_t i = 0, size = str.size(); i < size; ++i)
        {
            c = str[i];

            if(_is_valid_uri_character(c))
                retval += c;
            else retval += format("%%%02X", c);
        }
    }

    return retval;
}

string cppkit::ck_string_utils::uri_decode(const string& str)
{
    string retval;

    if(!str.empty())
    {
        char c;

        for(size_t i = 0, size = str.size(); i < size; ++i)
        {
            c = str[i];

            // spaces
            // The current spec (RFC 3986) does not permit encoding spaces with
            // +, but older versions of the spec do, so we decode + as space but
            // do not encode space as +.
            if(c == '+')
                retval += ' ';
            // unsafe characters
            else if(c == '%')
            {
                // hex to char conversion of next 2 characters
                if(i + 2 < size)
                {
                    //Initial contents of hexStr are irrelevant. It just needs to be the right length.
                    string hexStr = "XX";
                    for(size_t j = 0; j < 2; ++j)
                    {
                        if(isxdigit(str[i+j+1]))
                            hexStr[j] = str[i+j+1];
                        else
                            CK_STHROW(ck_invalid_argument_exception, ("malformed url"));
                    }

                    unsigned int val;
                    sscanf(hexStr.c_str(), "%x", &val);
                    retval += ck_string_utils::format("%c", (char)val);
                    i += 2;
                }
                else
                    CK_STHROW(ck_invalid_argument_exception, ("malformed url"));
            }
            else
                retval += c;
        }
    }

    return retval;
}

static const char base64_encoding_table[64] =
{
    'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
    'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
    'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
    'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
};

string cppkit::ck_string_utils::to_base64( const void* source, size_t length )
{
    size_t srcLen = length;
    const size_t bufferSize = (4 * ((srcLen + 2 - ((srcLen + 2) % 3)) / 3));

    vector<char> destBuffer(bufferSize);

    size_t encodeLen = 0;
    uint8_t* pData = (uint8_t*)&destBuffer[0];
    uint8_t* src = (uint8_t*)source;

    // Encode
    if( (src != nullptr) && (srcLen > 0))
    {
        for(size_t i = 0;  i < (srcLen / 3); i++, src += 3)
        {
            pData[encodeLen++] = base64_encoding_table[src[0]>>2];
            pData[encodeLen++] = base64_encoding_table[((src[0]& 0x3)<<4) | (src[1]>>4)];
            pData[encodeLen++] = base64_encoding_table[((src[1]& 0xF)<<2) | (src[2]>>6)];
            pData[encodeLen++] = base64_encoding_table[(src[2]& 0x3F)];
        }

        // Add padding '=' if necessary
        switch (srcLen % 3)
        {
        case 1:
            pData[encodeLen++] = base64_encoding_table[src[0]>>2];
            pData[encodeLen++] = base64_encoding_table[(src[0]& 0x3)<<4];
            pData[encodeLen++] = '=';
            pData[encodeLen++] = '=';
            break;

        case 2:
            pData[encodeLen++] = base64_encoding_table[src[0]>>2];
            pData[encodeLen++] = base64_encoding_table[((src[0]& 0x3)<<4) | (src[1]>>4)];
            pData[encodeLen++] = base64_encoding_table[((src[1]& 0xF)<<2)];
            pData[encodeLen++] = '=';
            break;
        }
    }
    else
    {
        // no data to encode. return an empty string.
        return string();
    }

    return string(&destBuffer[0], destBuffer.size());
}

enum CHAR_CLASS
{
    LOWER,
    UPPER,
    DIGIT,
    PLUS,
    EQUAL,
    SLASH,
    NEWLINE,
    CR,
    OTHER
};

static CHAR_CLASS _get_char_class(char c)
{
    if (isupper(c))
        return UPPER;
    else if (isdigit(c))
        return DIGIT;
    else if (islower(c))
        return LOWER;
    else if (c == '/')
        return SLASH;
    else if (c == '+')
        return PLUS;
    else if (c == '=')
        return EQUAL;
    else if (c == '\n')
        return NEWLINE;
    else if (c == '\r')
        return CR;
    else
        return OTHER;
}

vector<uint8_t> cppkit::ck_string_utils::from_base64(const std::string str)
{
    if (str.size() < 2)
        return vector<uint8_t>();

    // This buffer size is an upper bound.
    // This value can be: N, N+1 or N+2,
    // where N is the length of the raw data.
    size_t bufferSize = ((3 * str.size()) / 4);

    // Allocate some memory
    vector<uint8_t> destBuffer(bufferSize);

    uint8_t* pData = &destBuffer[0];
    uint8_t* src = (uint8_t*)str.c_str();
    size_t cursor = 0;
    size_t decodeLen = 0;
    uint8_t byteNumber = 0;
    uint8_t c;
    bool done = false;

    while (cursor < str.size())
    {
        c = *src++;
        cursor++;

        switch (_get_char_class(c))
        {
        case UPPER:
            c = c - 'A';
            break;
        case DIGIT:
            c -= '0' - 52;
            break;
        case LOWER:
            c -= 'a' - 26;
            break;
        case SLASH:
            c = 63;
            break;
        case PLUS:
            c = 62;
            break;
        case NEWLINE:
            continue;  // Just skip any new lines (Base64 sometimes has \n's)
        case CR:
            continue;  // Just skip any carriage returns (Base64 sometimes has CR's)
        case EQUAL:
            done = true;
            break;
        default:
            // This should never happen. Return an empty object.
            return vector<uint8_t>();
        }

        // If we haven't hit an '=' sign keep going
        if (!done)
        {
            switch(byteNumber++)
            {
            case 0:
                pData[decodeLen] = c << 2;
                break;
            case 1:
                pData[decodeLen++] |= c >> 4;
                pData[decodeLen] = c << 4;
                break;
            case 2:
                pData[decodeLen++] |= c >> 2;
                pData[decodeLen] = c << 6;
                break;
            case 3:
                pData[decodeLen++] |= c;
                byteNumber = 0;
                break;
            default:
                break;
            }
        }
    }

    // Actual raw data was less than our upper bound.
    // Copy the memory into a smaller buffer so we know its actual size.
    if ( decodeLen < destBuffer.size() )
    {
        vector<uint8_t> tempBuffer(decodeLen);
        memcpy(&tempBuffer[0], &destBuffer[0], decodeLen);
        destBuffer = tempBuffer;
    }

    return destBuffer;
}

static bool verify_digit(char c){ return isdigit(c) != 0; }

bool cppkit::ck_string_utils::is_integer(const string& str, bool canHaveSign)
{
    const size_t first = str.find_first_not_of(' ');

    if(first == string::npos || (!canHaveSign && str[first] == '-'))
        return false;

    const size_t strippedFront = str[first] == '-' ? first + 1 : first;
    const size_t last = str.find_last_not_of(' ');
    const size_t strippedBack = last == string::npos ? 0 : (str.size() - 1) - last;
    const int numSize = (int)(str.size() - (strippedFront + strippedBack));

    if(numSize == 0)
        return false;

    return count_if(str.begin() + strippedFront,
                    str.end() - strippedBack,
                    verify_digit) == numSize;
}

string cppkit::ck_string_utils::lstrip(const string& str)
{
    string retval = str;
    size_t pos = 0;
    while(pos < retval.size() && cppkit::ck_string_utils::is_space(retval[pos])) pos++;
    retval.erase(0, pos);
    return retval;
}

string cppkit::ck_string_utils::rstrip(const string& str)
{
    string retval = str;
    size_t pos = retval.size();
    while(pos > 0 && cppkit::ck_string_utils::is_space(retval[pos - 1])) pos--;
    retval.erase(pos);
    return retval;
}

string cppkit::ck_string_utils::strip(const string& str)
{
    auto retval = cppkit::ck_string_utils::rstrip(str);
    return cppkit::ck_string_utils::lstrip(retval);
}

string cppkit::ck_string_utils::strip_eol(const string& str)
{    
    if(cppkit::ck_string_utils::ends_with(str, "\r\n"))
        return str.substr(0, str.size() - 2);
    if(cppkit::ck_string_utils::ends_with(str, "\n"))
        return str.substr(0, str.size() - 1);
    return str;
}

bool cppkit::ck_string_utils::starts_with(const std::string& str, const std::string& other)
{
    const size_t otherSize = other.size();
    return otherSize <= str.size() && str.compare(0, otherSize, other) == 0;
}

bool cppkit::ck_string_utils::ends_with(const std::string& str, const std::string& other)
{
    const size_t otherSize = other.size();
    const size_t thisSize = str.size();
    return (otherSize <= thisSize) &&
        (str.compare(thisSize - otherSize, otherSize, other) == 0);
}

int cppkit::ck_string_utils::s_to_int(const string& s)
{
    return stoi(s);
}

unsigned int cppkit::ck_string_utils::s_to_uint(const std::string& s)
{
    // int's are 32 bit on linux... so...
    uint32_t val;
    sscanf(s.c_str(), "%u", &val);
    return val;
}

uint8_t cppkit::ck_string_utils::s_to_uint8(const string& s)
{
    uint8_t val;
    sscanf(s.c_str(), "%hhu", &val);
    return val;
}

int8_t cppkit::ck_string_utils::s_to_int8(const string& s)
{
    int8_t val;
    sscanf(s.c_str(), "%hhd", &val);
    return val;
}

uint16_t cppkit::ck_string_utils::s_to_uint16(const string& s)
{
    uint16_t val;
    sscanf(s.c_str(), "%hu", &val);
    return val;
}

int16_t cppkit::ck_string_utils::s_to_int16(const string& s)
{
    int16_t val;
    sscanf(s.c_str(), "%hd", &val);
    return val;
}

uint32_t cppkit::ck_string_utils::s_to_uint32(const string& s)
{
    uint32_t val;
    sscanf(s.c_str(), "%u", &val);
    return val;
}

int32_t cppkit::ck_string_utils::s_to_int32(const string& s)
{
    int32_t val;
    sscanf(s.c_str(), "%d", &val);
    return val;
}

uint64_t cppkit::ck_string_utils::s_to_uint64(const string& s)
{
    uint64_t val;
    sscanf(s.c_str(), "%llu", (unsigned long long*)&val);
    return val;
}

int64_t cppkit::ck_string_utils::s_to_int64(const string& s)
{
    int64_t val;
    sscanf(s.c_str(), "%lld", (long long*)&val);
    return val;
}

double cppkit::ck_string_utils::s_to_double(const string& s)
{
    double val;
    sscanf(s.c_str(), "%lf", &val);
    return val;
}

string cppkit::ck_string_utils::int_to_s(int val)
{
    return to_string(val);
}

std::string cppkit::ck_string_utils::uint_to_s(unsigned int val)
{
    return ck_string_utils::format("%u", val);
}

string cppkit::ck_string_utils::int8_to_s(int8_t val)
{
    return ck_string_utils::format("%d", val);
}

string cppkit::ck_string_utils::uint8_to_s(uint8_t val)
{
    return ck_string_utils::format("%u", val);
}

string cppkit::ck_string_utils::int16_to_s(int16_t val)
{
    return ck_string_utils::format("%d", val);
}

string cppkit::ck_string_utils::uint16_to_s(uint16_t val)
{
    return ck_string_utils::format("%u", val);
}

string cppkit::ck_string_utils::int32_to_s(int32_t val)
{
    return ck_string_utils::format("%d", val);
}

string cppkit::ck_string_utils::uint32_to_s(uint32_t val)
{
    return ck_string_utils::format("%u", val);
}

string cppkit::ck_string_utils::int64_to_s(int64_t val)
{
    return ck_string_utils::format("%ld", val);
}

string cppkit::ck_string_utils::uint64_to_s(uint64_t val)
{
    return ck_string_utils::format("%lu", val);
}

string cppkit::ck_string_utils::double_to_s(double val)
{
    return ck_string_utils::format("%lf", val);
}