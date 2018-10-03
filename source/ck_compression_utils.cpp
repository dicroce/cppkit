
#include "cppkit/ck_compression_utils.h"
#include "cppkit/ck_exception.h"
#include <string.h>
#include <bzlib.h>
#include <climits>

using namespace cppkit;
using namespace std;

#if 0
typedef 
   struct {
      char *next_in;
      unsigned int avail_in;
      unsigned int total_in_lo32;
      unsigned int total_in_hi32;

      char *next_out;
      unsigned int avail_out;
      unsigned int total_out_lo32;
      unsigned int total_out_hi32;

      void *state;

      void *(*bzalloc)(void *,int,int);
      void (*bzfree)(void *,void *);
      void *opaque;
   } 
   bz_stream;
#endif

uint64_t cppkit::ck_compression_utils::compress_buffer(const uint8_t* src, uint64_t srcLen, uint8_t* dst, uint64_t dstLen)
{
    bz_stream bzs;
    memset(&bzs, 0, sizeof(bz_stream));

    BZ2_bzCompressInit(&bzs, 9, 0, 30);

    // Before each call to BZ2_bzCompress, next_in should point at the data to be compressed, and avail_in should indicate how many bytes the library may read. BZ2_bzCompress updates 
    // next_in, avail_in and total_in to reflect the number of bytes it has read.

    auto s = const_cast<uint8_t*>(src);
    auto d = dst;

    uint64_t totalIn = 0;
    uint64_t totalOut = 0;

    int ret;
    do
    {
        bzs.next_in = (char*)(s + totalIn);
        bzs.avail_in = (srcLen >= UINT_MAX)?UINT_MAX:srcLen;
        bzs.next_out = (char*)(d + totalOut);
        bzs.avail_out = (dstLen >= UINT_MAX)?UINT_MAX:dstLen;

        ret = BZ2_bzCompress(&bzs, (srcLen >= UINT_MAX)?BZ_RUN:BZ_FINISH);

        totalIn = (((uint64_t)bzs.total_in_hi32) << 32) + bzs.total_in_lo32;
        srcLen -= totalIn;
        totalOut = (((uint64_t)bzs.total_out_hi32) << 32) + bzs.total_out_lo32;
        dstLen -= totalOut;

        if(ret < 0)
        {
            BZ2_bzCompressEnd(&bzs);
            CK_THROW(("bzip2 error: %d", ret));
        }
    }
    while(ret != BZ_STREAM_END);

    BZ2_bzCompressEnd(&bzs);

    return totalOut;
}

uint64_t cppkit::ck_compression_utils::decompress_buffer(const uint8_t* src, uint64_t srcLen, uint8_t* dst, uint64_t dstLen)
{
    bz_stream bzs;
    memset(&bzs, 0, sizeof(bz_stream));

    BZ2_bzDecompressInit(&bzs, 0, 0);

    // Before each call to BZ2_bzCompress, next_in should point at the data to be compressed, and avail_in should indicate how many bytes the library may read. BZ2_bzCompress updates 
    // next_in, avail_in and total_in to reflect the number of bytes it has read.

    auto s = const_cast<uint8_t*>(src);
    auto d = dst;

    uint64_t totalIn = 0;
    uint64_t totalOut = 0;

    int ret;
    do
    {
        bzs.next_in = (char*)(s + totalIn);
        bzs.avail_in = (srcLen >= UINT_MAX)?UINT_MAX:srcLen;
        bzs.next_out = (char*)(d + totalOut);
        bzs.avail_out = (dstLen >= UINT_MAX)?UINT_MAX:dstLen;

        ret = BZ2_bzDecompress(&bzs);

        totalIn = (((uint64_t)bzs.total_in_hi32) << 32) + bzs.total_in_lo32;
        srcLen -= totalIn;
        totalOut = (((uint64_t)bzs.total_out_hi32) << 32) + bzs.total_out_lo32;
        dstLen -= totalOut;

        if(ret < 0)
        {
            BZ2_bzDecompressEnd(&bzs);
            CK_THROW(("bzip2 error: %d", ret));
        }
    }
    while(ret != BZ_STREAM_END);

    BZ2_bzDecompressEnd(&bzs);

    return totalOut;
}


#if 0

    BZ2_bzCompressInit()
    BZ2_bzCompress()
    BZ2_bzCompressEnd()

    BZ2_bzDecompressInit()
    BZ2_bzDecompress()
    BZ2_bzDecompressEnd()





 /* 1 - 9; 9 gives the best compression but uses the most runtime memory*/
    int blockSize = 9;
    /*1 - 4; 4 gives the most diagnostic info*/
    int verbosity = 0;
    /*30 is suggested; see docs for bzip2 for full info*/
    int workFactor = 30;
    int headerOffset;

BZ_EXTERN int BZ_API(BZ2_bzBuffToBuffCompress) ( 
      char*         dest, 
      unsigned int* destLen,
      char*         source, 
      unsigned int  sourceLen,
      int           blockSize100k, 
      int           verbosity, 
      int           workFactor 
   );

BZ_EXTERN int BZ_API(BZ2_bzBuffToBuffDecompress) ( 
      char*         dest, 
      unsigned int* destLen,
      char*         source, 
      unsigned int  sourceLen,
      int           small, 
      int           verbosity 
   );
#endif
