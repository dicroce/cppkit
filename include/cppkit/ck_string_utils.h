
#ifndef cppkit_ck_string_utils_h
#define cppkit_ck_string_utils_h

#include <string>
#include <vector>
#include <cstdint>

namespace cppkit
{

namespace ck_string_utils
{

std::vector<std::string> split(const std::string& str, char delim);
std::vector<std::string> split(const std::string& str, const std::string& delim);

std::string format(const char* fmt, ...);
std::string format(const char* fmt, va_list& args);

// Returns true if output was truncated (didn't fit in buf).
// Note: these methods are for specific performance use cases, if thats not
// what you need use the above format() method.
bool format_buffer(char* buf, size_t size, const char* fmt, ...);
bool format_buffer(char* buf, size_t size, const char* fmt, va_list& args);

bool contains(const std::string& str, const std::string& target);

std::string erase_all(const std::string& str, char delim);
std::string erase_all(const std::string& str, const std::string& delim);
std::string replace_all(const std::string& str, char toBeReplaced, char toReplaceWidth);
std::string replace_all(const std::string& str, const std::string& toBeReplaced, const std::string& toReplaceWith);

std::string to_lower(const std::string& str);
std::string to_upper(const std::string& str);

std::string uri_encode(const std::string& str);
std::string uri_decode(const std::string& str);

std::string to_base64( const void* source, size_t length );
std::vector<uint8_t> from_base64(const std::string str);

inline bool is_space(char a) { return (a == ' ' || a == '\n' || a == '\t' || a == '\r'); }

bool is_integer(const std::string& str, bool canHaveSign=true);

std::string lstrip(const std::string& str);
std::string rstrip(const std::string& str);
std::string strip(const std::string& str);
std::string strip_eol(const std::string& str);

bool starts_with(const std::string& str, const std::string& other);
bool ends_with(const std::string& str, const std::string& other);

int s_to_int(const std::string& s);
unsigned int s_to_uint(const std::string& s);
uint8_t s_to_uint8(const std::string& s);
int8_t s_to_int8(const std::string& s);
uint16_t s_to_uint16(const std::string& s);
int16_t s_to_int16(const std::string& s);
uint32_t s_to_uint32(const std::string& s);
int32_t s_to_int32(const std::string& s);
uint64_t s_to_uint64(const std::string& s);
int64_t s_to_int64(const std::string& s);
double s_to_double(const std::string& s);
size_t s_to_size_t(const std::string& s);

std::string int_to_s(int val);
std::string uint_to_s(unsigned int val);
std::string uint8_to_s(uint8_t val);
std::string int8_to_s(int8_t val);
std::string uint16_to_s(uint16_t val);
std::string int16_to_s(int16_t val);
std::string uint32_to_s(uint32_t val);
std::string int32_to_s(int32_t val);
std::string uint64_to_s(uint64_t val);
std::string int64_to_s(int64_t val);
std::string double_to_s(double val);
std::string size_t_to_s(size_t val);

}

}

#endif
