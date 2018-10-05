
#ifndef cppkit_ck_compression_utils_h
#define cppkit_ck_compression_utils_h

#include <cstdint>
#include <functional>
#include <climits>

namespace cppkit
{

namespace ck_compression_utils
{

uint64_t compress_buffer(const uint8_t* src, uint64_t srcLen, uint8_t* dst, uint64_t dstLen, uint32_t blockSize = UINT_MAX, const std::function<void(uint64_t)>& cb = {});

uint64_t decompress_buffer(const uint8_t* src, uint64_t srcLen, uint8_t* dst, uint64_t dstLen, uint32_t blockSize = UINT_MAX, const std::function<void(uint64_t)>& cb = {});

}

}

#endif
