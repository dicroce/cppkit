
#ifndef cppkit_ck_sha_256_h
#define cppkit_ck_sha_256_h

#include <string>
#include <cstdint>

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

    // returns 32 byte sha 256
    void get( uint8_t* output );
    std::string get_as_string();

private:
    void _transform(const unsigned char *message, unsigned int block_nb);

    static const uint32_t sha256_k[64];
    static const unsigned int SHA224_256_BLOCK_SIZE = (512/8);
    static const unsigned int DIGEST_SIZE = ( 256 / 8);
    unsigned int _tot_len;
    unsigned int _len;
    unsigned char _block[2*SHA224_256_BLOCK_SIZE];
    uint32_t _h[8];
    uint8_t _digest[32];
    bool _finalized;
};

std::string sha_256(const uint8_t* p, size_t sz);

}

#endif
