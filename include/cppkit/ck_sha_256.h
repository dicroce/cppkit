
#ifndef cppkit_ck_sha_256_h
#define cppkit_ck_sha_256_h

#include <string>
#include "openssl/sha.h"

namespace cppkit
{

class ck_sha_256 final
{
public:
    ck_sha_256();
    ck_sha_256(const ck_sha_256&) = delete;
    ck_sha_256(ck_sha_256&&) = delete;
    ~ck_sha_256() noexcept;

    ck_sha_256& operator=(const ck_sha_256&) = delete;
    ck_sha_256& operator=(ck_sha_256&&) = delete;

    void update( const uint8_t* src, size_t size );
    void finalize();

    // returns 32 byte sha256
    void get( uint8_t* output );
    std::string get_as_string();

private:
    uint8_t _digest[32];
    bool _finalized;
    SHA256_CTX _sha256;
};

std::string sha_256(const uint8_t* p, size_t sz);

}

#endif
