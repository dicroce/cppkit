
#ifndef cppkit_ck_compression_utils_h
#define cppkit_ck_compression_utils_h

#include <cstdint>

namespace cppkit
{

namespace ck_compression_utils
{

uint64_t compress_buffer(const uint8_t* src, uint64_t srcLen, uint8_t* dst, uint64_t dstLen);

uint64_t decompress_buffer(const uint8_t* src, uint64_t srcLen, uint8_t* dst, uint64_t dstLen);

}

}

#endif
