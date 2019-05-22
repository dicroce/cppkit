
#include "cppkit/ck_sha_256.h"
#include "cppkit/ck_exception.h"
#include "cppkit/ck_string_utils.h"
#include <string.h>

using namespace cppkit;
using namespace std;

#define SHA2_SHFR(x, n) (x >> n)
#define SHA2_ROTR(x, n) ((x >> n) | (x << ((sizeof(x) << 3) - n)))
#define SHA2_ROTL(x, n) ((x << n) | (x >> ((sizeof(x) << 3) - n)))
#define SHA2_CH(x, y, z) ((x & y) ^ (~x & z))
#define SHA2_MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))
#define SHA256_F1(x) (SHA2_ROTR(x,  2) ^ SHA2_ROTR(x, 13) ^ SHA2_ROTR(x, 22))
#define SHA256_F2(x) (SHA2_ROTR(x,  6) ^ SHA2_ROTR(x, 11) ^ SHA2_ROTR(x, 25))
#define SHA256_F3(x) (SHA2_ROTR(x,  7) ^ SHA2_ROTR(x, 18) ^ SHA2_SHFR(x,  3))
#define SHA256_F4(x) (SHA2_ROTR(x, 17) ^ SHA2_ROTR(x, 19) ^ SHA2_SHFR(x, 10))

#define SHA2_UNPACK32(x, str)                   \
{                                               \
    *((str) + 3) = (uint8_t) ((x)      );       \
    *((str) + 2) = (uint8_t) ((x) >>  8);       \
    *((str) + 1) = (uint8_t) ((x) >> 16);       \
    *((str) + 0) = (uint8_t) ((x) >> 24);       \
}

#define SHA2_PACK32(str, x)                     \
{                                               \
    *(x) =   ((uint32_t) *((str) + 3)      )    \
           | ((uint32_t) *((str) + 2) <<  8)    \
           | ((uint32_t) *((str) + 1) << 16)    \
           | ((uint32_t) *((str) + 0) << 24);   \
}

const uint32_t ck_sha_256::sha256_k[64] = 
{
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

ck_sha_256::ck_sha_256() :
    _tot_len(0),
    _len(0),
    _finalized(false)
{
    _h[0] = 0x6a09e667;
    _h[1] = 0xbb67ae85;
    _h[2] = 0x3c6ef372;
    _h[3] = 0xa54ff53a;
    _h[4] = 0x510e527f;
    _h[5] = 0x9b05688c;
    _h[6] = 0x1f83d9ab;
    _h[7] = 0x5be0cd19;

    memset(&_digest[0], 0, 32);
}

ck_sha_256::~ck_sha_256() noexcept
{
}

void ck_sha_256::update( const uint8_t* src, size_t size )
{
    unsigned int tmp_len = SHA224_256_BLOCK_SIZE - _len;
    unsigned int re_len = size < tmp_len ? size : tmp_len;

    memcpy(&_block[_len], src, re_len);

    if (_len + size < SHA224_256_BLOCK_SIZE) {
        _len += size;
        return;
    }

    unsigned int new_len = size - re_len;
    unsigned int block_nb = new_len / SHA224_256_BLOCK_SIZE;
    const unsigned char* shifted_message = src + re_len;

    _transform(_block, 1);
    _transform(shifted_message, block_nb);

    re_len = new_len % SHA224_256_BLOCK_SIZE;
    memcpy(_block, &shifted_message[block_nb << 6], re_len);

    _len = re_len;
    _tot_len += (block_nb + 1) << 6;
}

void ck_sha_256::finalize()
{
    unsigned int block_nb = (1 + ((SHA224_256_BLOCK_SIZE - 9) < (_len % SHA224_256_BLOCK_SIZE)));
    unsigned int len_b = (_tot_len + _len) << 3;
    unsigned int p_len = block_nb << 6;

    memset(_block + _len, 0, p_len - _len);

    _block[_len] = 0x80;

    SHA2_UNPACK32(len_b, _block + p_len - 4);

    _transform(_block, block_nb);

    for (int i = 0 ; i < 8; i++) {
        SHA2_UNPACK32(_h[i], &_digest[i << 2]);
    }

    _finalized = true;
}

void ck_sha_256::get( uint8_t* output ) // returns 32 byte sha256
{
    if(!_finalized)
        CK_THROW(("Finalized your ck_sha_256 hash before attempting to call get()."))

    memcpy(output, _digest, 32);
}

string ck_sha_256::get_as_string()
{
    if(!_finalized)
        CK_THROW(("Finalized your ck_sha_256 hash before attempting to call get()."))

    return ck_string_utils::format( "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
                                    _digest[0], _digest[1],_digest[2], _digest[3],
                                    _digest[4], _digest[5], _digest[6], _digest[7],
                                    _digest[8], _digest[9], _digest[10], _digest[11],
                                    _digest[12], _digest[13], _digest[14], _digest[15],
                                    _digest[16], _digest[17],_digest[18], _digest[19],
                                    _digest[20], _digest[21], _digest[22], _digest[23],
                                    _digest[24], _digest[25], _digest[26], _digest[27],
                                    _digest[28], _digest[29], _digest[30], _digest[31] );
}


void ck_sha_256::_transform(const unsigned char *message, unsigned int block_nb)
{
    uint32_t w[64];
    uint32_t wv[8];
    uint32_t t1, t2;

    for (int i = 0; i < (int) block_nb; i++)
    {
        const unsigned char* sub_block = message + (i << 6);

        for (int j = 0; j < 16; j++)
            SHA2_PACK32(&sub_block[j << 2], &w[j]);

        for (int j = 16; j < 64; j++)
            w[j] =  SHA256_F4(w[j -  2]) + w[j -  7] + SHA256_F3(w[j - 15]) + w[j - 16];

        for (int j = 0; j < 8; j++)
            wv[j] = _h[j];

        for (int j = 0; j < 64; j++) 
        {
            t1 = wv[7] + SHA256_F2(wv[4]) + SHA2_CH(wv[4], wv[5], wv[6]) + sha256_k[j] + w[j];
            t2 = SHA256_F1(wv[0]) + SHA2_MAJ(wv[0], wv[1], wv[2]);
            wv[7] = wv[6];
            wv[6] = wv[5];
            wv[5] = wv[4];
            wv[4] = wv[3] + t1;
            wv[3] = wv[2];
            wv[2] = wv[1];
            wv[1] = wv[0];
            wv[0] = t1 + t2;
        }

        for (int j = 0; j < 8; j++)
            _h[j] += wv[j];
    }
}

string cppkit::sha_256(const uint8_t* p, size_t sz)
{
    ck_sha_256 h;
    h.update(p, sz);
    h.finalize();
    return h.get_as_string();
}
