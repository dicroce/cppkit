
#include "ck_bitsy_test.h"
#include "cppkit/ck_bitsy.h"
#include <vector>

using namespace std;
using namespace cppkit;

#ifdef IS_WINDOWS
#define RANDOM rand
#else
#define RANDOM random
#endif

REGISTER_TEST_FIXTURE(ck_bitsy_test);

void ck_bitsy_test::test_constructor()
{
    vector<uint8_t> buffer(12, 7);

    {
        const uint8_t* begin = &buffer.front();
        const uint8_t* end = &buffer.front() + buffer.size();
        ck_bitsy<const uint8_t*> bitsy(begin, end);
        UT_ASSERT(bitsy._begin == begin);
        UT_ASSERT(bitsy._end == end);
        UT_ASSERT(bitsy._pos == begin);
        UT_ASSERT(bitsy._currentMask == 128);

        bitsy.get_bits(19);
        ck_bitsy<const uint8_t*> copy = bitsy;
        UT_ASSERT(copy._begin == begin);
        UT_ASSERT(copy._end == end);
        UT_ASSERT(copy._pos == begin + 2);
        UT_ASSERT(bitsy._currentMask == 16);

        copy.get_bits(5);
        UT_ASSERT(bitsy._begin == begin);
        UT_ASSERT(bitsy._end == end);
        UT_ASSERT(bitsy._pos == begin + 2);
        UT_ASSERT(bitsy._currentMask == 16);

        bitsy = copy;
        UT_ASSERT(bitsy._begin == begin);
        UT_ASSERT(bitsy._end == end);
        UT_ASSERT(bitsy._pos == begin + 3);
        UT_ASSERT(bitsy._currentMask == 128);
    }

    {
        vector<uint8_t>::const_iterator begin = buffer.begin();
        vector<uint8_t>::const_iterator end = buffer.end();
        ck_bitsy<vector<uint8_t>::const_iterator> bitsy(begin, end);
        UT_ASSERT(bitsy._begin == begin);
        UT_ASSERT(bitsy._end == end);
        UT_ASSERT(bitsy._pos == begin);
        UT_ASSERT(bitsy._currentMask == 128);

        bitsy.get_bits(19);
        ck_bitsy<vector<uint8_t>::const_iterator> copy = bitsy;
        UT_ASSERT(copy._begin == begin);
        UT_ASSERT(copy._end == end);
        UT_ASSERT(copy._pos == begin + 2);
        UT_ASSERT(bitsy._currentMask == 16);

        copy.get_bits(5);
        UT_ASSERT(bitsy._begin == begin);
        UT_ASSERT(bitsy._end == end);
        UT_ASSERT(bitsy._pos == begin + 2);
        UT_ASSERT(bitsy._currentMask == 16);

        bitsy = copy;
        UT_ASSERT(bitsy._begin == begin);
        UT_ASSERT(bitsy._end == end);
        UT_ASSERT(bitsy._pos == begin + 3);
        UT_ASSERT(bitsy._currentMask == 128);
    }
}

void ck_bitsy_test::test_set_buffer()
{
    {
        vector<uint8_t> buffer(12, 7);
        ck_bitsy<const uint8_t*> bitsy(&buffer.front(), &buffer.front() + buffer.size());
        bitsy.get_bits(19);

        vector<uint8_t> other(5, 2);
        bitsy.set_buffer(&other.front(), &other.front() + other.size());
        UT_ASSERT(bitsy._begin == &other.front());
        UT_ASSERT(bitsy._end == &other.front() + other.size());
        UT_ASSERT(bitsy._pos == &other.front());
        UT_ASSERT(bitsy._currentMask == 128);
    }

    {
        vector<uint8_t> buffer(12, 7);
        ck_bitsy<vector<uint8_t>::const_iterator> bitsy(buffer.begin(), buffer.end());
        bitsy.get_bits(19);

        vector<uint8_t> other(5, 2);
        bitsy.set_buffer(other.begin(), other.end());
        UT_ASSERT(bitsy._begin == other.begin());
        UT_ASSERT(bitsy._end == other.end());
        UT_ASSERT(bitsy._pos == other.begin());
        UT_ASSERT(bitsy._currentMask == 128);
    }
}

void ck_bitsy_test::test_reset()
{
    vector<uint8_t> buffer(12, 7);

    {
        const uint8_t* begin = &buffer.front();
        const uint8_t* end = &buffer.front() + buffer.size();
        ck_bitsy<const uint8_t*> bitsy(begin, end);
        bitsy.get_bits(19);
        UT_ASSERT(bitsy._begin == begin);
        UT_ASSERT(bitsy._end == end);
        UT_ASSERT(bitsy._pos == begin + 2);
        UT_ASSERT(bitsy._currentMask == 16);

        bitsy.reset();
        UT_ASSERT(bitsy._begin == begin);
        UT_ASSERT(bitsy._end == end);
        UT_ASSERT(bitsy._pos == &buffer.front());
        UT_ASSERT(bitsy._currentMask == 128);
        UT_ASSERT(bitsy._bytesRemaining == -1);
    }

    {
        vector<uint8_t>::const_iterator begin = buffer.begin();
        vector<uint8_t>::const_iterator end = buffer.end();
        ck_bitsy<vector<uint8_t>::const_iterator> bitsy(begin, end);
        bitsy.get_bits(19);
        UT_ASSERT(bitsy._begin == begin);
        UT_ASSERT(bitsy._end == end);
        UT_ASSERT(bitsy._pos == begin + 2);
        UT_ASSERT(bitsy._currentMask == 16);

        bitsy.reset();
        UT_ASSERT(bitsy._begin == begin);
        UT_ASSERT(bitsy._end == end);
        UT_ASSERT(bitsy._pos == buffer.begin());
        UT_ASSERT(bitsy._currentMask == 128);
        UT_ASSERT(bitsy._bytesRemaining == -1);
    }
}

void ck_bitsy_test::test_get_bits()
{
    vector<uint8_t> buffer(14, 0);
    buffer[0] = 7;
    buffer[1] = 0;
    buffer[2] = 255;
    buffer[3] = 47;
    buffer[4] = 255;
    buffer[5] = 0;
    buffer[6] = 0;
    buffer[7] = 0;
    buffer[8] = 0;
    buffer[9] = 255;
    buffer[10] = 255;
    buffer[11] = 255;
    buffer[12] = 255;
    buffer[13] = 5;

    {
        ck_bitsy<const uint8_t*> bitsy(&buffer.front(), &buffer.front() + buffer.size());
        UT_ASSERT(bitsy.get_bits_remaining() == 112);
        UT_ASSERT(bitsy.get_bits(8) == 7);
        UT_ASSERT(bitsy.get_bits_remaining() == 104);
        UT_ASSERT(bitsy.get_bits(8) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 96);
        UT_ASSERT(bitsy.get_bits(8) == 255);
        UT_ASSERT(bitsy.get_bits_remaining() == 88);
        UT_ASSERT(bitsy.get_bits(8) == 47);
        UT_ASSERT(bitsy.get_bits_remaining() == 80);
        UT_ASSERT(bitsy.get_bits(8) == 255);
        UT_ASSERT(bitsy.get_bits_remaining() == 72);
        UT_ASSERT(bitsy.get_bits(8) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 64);
        UT_ASSERT(bitsy.get_bits(8) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 56);
        UT_ASSERT(bitsy.get_bits(8) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 48);
        UT_ASSERT(bitsy.get_bits(8) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 40);
        UT_ASSERT(bitsy.get_bits(8) == 255);
        UT_ASSERT(bitsy.get_bits_remaining() == 32);
        UT_ASSERT(bitsy.get_bits(8) == 255);
        UT_ASSERT(bitsy.get_bits_remaining() == 24);
        UT_ASSERT(bitsy.get_bits(8) == 255);
        UT_ASSERT(bitsy.get_bits_remaining() == 16);
        UT_ASSERT(bitsy.get_bits(8) == 255);
        UT_ASSERT(bitsy.get_bits_remaining() == 8);
        UT_ASSERT(bitsy.get_bits(8) == 5);
        UT_ASSERT(bitsy.get_bits_remaining() == 0);
    }

    {
        ck_bitsy<vector<uint8_t>::const_iterator> bitsy(buffer.begin(), buffer.end());
        UT_ASSERT(bitsy.get_bits_remaining() == 112);
        UT_ASSERT(bitsy.get_bits(8) == 7);
        UT_ASSERT(bitsy.get_bits_remaining() == 104);
        UT_ASSERT(bitsy.get_bits(8) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 96);
        UT_ASSERT(bitsy.get_bits(8) == 255);
        UT_ASSERT(bitsy.get_bits_remaining() == 88);
        UT_ASSERT(bitsy.get_bits(8) == 47);
        UT_ASSERT(bitsy.get_bits_remaining() == 80);
        UT_ASSERT(bitsy.get_bits(8) == 255);
        UT_ASSERT(bitsy.get_bits_remaining() == 72);
        UT_ASSERT(bitsy.get_bits(8) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 64);
        UT_ASSERT(bitsy.get_bits(8) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 56);
        UT_ASSERT(bitsy.get_bits(8) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 48);
        UT_ASSERT(bitsy.get_bits(8) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 40);
        UT_ASSERT(bitsy.get_bits(8) == 255);
        UT_ASSERT(bitsy.get_bits_remaining() == 32);
        UT_ASSERT(bitsy.get_bits(8) == 255);
        UT_ASSERT(bitsy.get_bits_remaining() == 24);
        UT_ASSERT(bitsy.get_bits(8) == 255);
        UT_ASSERT(bitsy.get_bits_remaining() == 16);
        UT_ASSERT(bitsy.get_bits(8) == 255);
        UT_ASSERT(bitsy.get_bits_remaining() == 8);
        UT_ASSERT(bitsy.get_bits(8) == 5);
        UT_ASSERT(bitsy.get_bits_remaining() == 0);
    }

    {
        ck_bitsy<const uint8_t*> bitsy(&buffer.front(), &buffer.front() + buffer.size());
        UT_ASSERT(bitsy.get_bits_remaining() == 112);
        UT_ASSERT(bitsy.get_bits(4) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 108);
        UT_ASSERT(bitsy.get_bits(5) == 14);
        UT_ASSERT(bitsy.get_bits_remaining() == 103);
        UT_ASSERT(bitsy.get_bits(18) == 2041);
        UT_ASSERT(bitsy.get_bits_remaining() == 85);
        UT_ASSERT(bitsy.get_bits(0) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 85);
        UT_ASSERT(bitsy.get_bits(32) == 2146959360);
        UT_ASSERT(bitsy.get_bits_remaining() == 53);
        UT_ASSERT(bitsy.get_bits(13) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 40);
        UT_ASSERT(bitsy.get_bits(32) == (uint32_t)-1);
        UT_ASSERT(bitsy.get_bits_remaining() == 8);
        UT_ASSERT(bitsy.get_bits(3) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 5);
        UT_ASSERT(bitsy.get_bits(8) == 5);
        UT_ASSERT(bitsy.get_bits_remaining() == 0);
        UT_ASSERT(bitsy.get_bits(8) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 0);
        UT_ASSERT(bitsy.get_bits(8) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 0);
    }

    {
        ck_bitsy<vector<uint8_t>::const_iterator> bitsy(buffer.begin(), buffer.end());
        UT_ASSERT(bitsy.get_bits_remaining() == 112);
        UT_ASSERT(bitsy.get_bits(4) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 108);
        UT_ASSERT(bitsy.get_bits(5) == 14);
        UT_ASSERT(bitsy.get_bits_remaining() == 103);
        UT_ASSERT(bitsy.get_bits(18) == 2041);
        UT_ASSERT(bitsy.get_bits_remaining() == 85);
        UT_ASSERT(bitsy.get_bits(0) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 85);
        UT_ASSERT(bitsy.get_bits(32) == 2146959360);
        UT_ASSERT(bitsy.get_bits_remaining() == 53);
        UT_ASSERT(bitsy.get_bits(13) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 40);
        UT_ASSERT(bitsy.get_bits(32) == (uint32_t)-1);
        UT_ASSERT(bitsy.get_bits_remaining() == 8);
        UT_ASSERT(bitsy.get_bits(3) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 5);
        UT_ASSERT(bitsy.get_bits(8) == 5);
        UT_ASSERT(bitsy.get_bits_remaining() == 0);
        UT_ASSERT(bitsy.get_bits(8) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 0);
        UT_ASSERT(bitsy.get_bits(8) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 0);
    }
}

void ck_bitsy_test::test_get_exact_bits()
{
    vector<uint8_t> buffer(14, 0);
    buffer[0] = 7;
    buffer[1] = 0;
    buffer[2] = 255;
    buffer[3] = 47;
    buffer[4] = 255;
    buffer[5] = 0;
    buffer[6] = 0;
    buffer[7] = 0;
    buffer[8] = 0;
    buffer[9] = 255;
    buffer[10] = 255;
    buffer[11] = 255;
    buffer[12] = 255;
    buffer[13] = 5;

    {
        ck_bitsy<const uint8_t*> bitsy(&buffer.front(), &buffer.front() + buffer.size());
        UT_ASSERT(bitsy.get_bits_remaining() == 112);
        UT_ASSERT(bitsy.get_exact_bits(8) == 7);
        UT_ASSERT(bitsy.get_bits_remaining() == 104);
        UT_ASSERT(bitsy.get_exact_bits(8) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 96);
        UT_ASSERT(bitsy.get_exact_bits(8) == 255);
        UT_ASSERT(bitsy.get_bits_remaining() == 88);
        UT_ASSERT(bitsy.get_exact_bits(8) == 47);
        UT_ASSERT(bitsy.get_bits_remaining() == 80);
        UT_ASSERT(bitsy.get_exact_bits(8) == 255);
        UT_ASSERT(bitsy.get_bits_remaining() == 72);
        UT_ASSERT(bitsy.get_exact_bits(8) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 64);
        UT_ASSERT(bitsy.get_exact_bits(8) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 56);
        UT_ASSERT(bitsy.get_exact_bits(8) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 48);
        UT_ASSERT(bitsy.get_exact_bits(8) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 40);
        UT_ASSERT(bitsy.get_exact_bits(8) == 255);
        UT_ASSERT(bitsy.get_bits_remaining() == 32);
        UT_ASSERT(bitsy.get_exact_bits(8) == 255);
        UT_ASSERT(bitsy.get_bits_remaining() == 24);
        UT_ASSERT(bitsy.get_exact_bits(8) == 255);
        UT_ASSERT(bitsy.get_bits_remaining() == 16);
        UT_ASSERT(bitsy.get_exact_bits(8) == 255);
        UT_ASSERT(bitsy.get_bits_remaining() == 8);
        UT_ASSERT(bitsy.get_exact_bits(8) == 5);
        UT_ASSERT(bitsy.get_bits_remaining() == 0);
    }

    {
        ck_bitsy<vector<uint8_t>::const_iterator> bitsy(buffer.begin(), buffer.end());
        UT_ASSERT(bitsy.get_bits_remaining() == 112);
        UT_ASSERT(bitsy.get_exact_bits(8) == 7);
        UT_ASSERT(bitsy.get_bits_remaining() == 104);
        UT_ASSERT(bitsy.get_exact_bits(8) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 96);
        UT_ASSERT(bitsy.get_exact_bits(8) == 255);
        UT_ASSERT(bitsy.get_bits_remaining() == 88);
        UT_ASSERT(bitsy.get_exact_bits(8) == 47);
        UT_ASSERT(bitsy.get_bits_remaining() == 80);
        UT_ASSERT(bitsy.get_exact_bits(8) == 255);
        UT_ASSERT(bitsy.get_bits_remaining() == 72);
        UT_ASSERT(bitsy.get_exact_bits(8) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 64);
        UT_ASSERT(bitsy.get_exact_bits(8) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 56);
        UT_ASSERT(bitsy.get_exact_bits(8) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 48);
        UT_ASSERT(bitsy.get_exact_bits(8) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 40);
        UT_ASSERT(bitsy.get_exact_bits(8) == 255);
        UT_ASSERT(bitsy.get_bits_remaining() == 32);
        UT_ASSERT(bitsy.get_exact_bits(8) == 255);
        UT_ASSERT(bitsy.get_bits_remaining() == 24);
        UT_ASSERT(bitsy.get_exact_bits(8) == 255);
        UT_ASSERT(bitsy.get_bits_remaining() == 16);
        UT_ASSERT(bitsy.get_exact_bits(8) == 255);
        UT_ASSERT(bitsy.get_bits_remaining() == 8);
        UT_ASSERT(bitsy.get_exact_bits(8) == 5);
        UT_ASSERT(bitsy.get_bits_remaining() == 0);
    }

    {
        ck_bitsy<const uint8_t*> bitsy(&buffer.front(), &buffer.front() + buffer.size());
        UT_ASSERT(bitsy.get_bits_remaining() == 112);
        UT_ASSERT(bitsy.get_exact_bits(4) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 108);
        UT_ASSERT(bitsy.get_exact_bits(5) == 14);
        UT_ASSERT(bitsy.get_bits_remaining() == 103);
        UT_ASSERT(bitsy.get_exact_bits(18) == 2041);
        UT_ASSERT(bitsy.get_bits_remaining() == 85);
        UT_ASSERT(bitsy.get_bits_remaining() == 85);
        UT_ASSERT(bitsy.get_exact_bits(32) == 2146959360);
        UT_ASSERT(bitsy.get_bits_remaining() == 53);
        UT_ASSERT(bitsy.get_exact_bits(13) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 40);
        UT_ASSERT(bitsy.get_exact_bits(32) == (uint32_t)-1);
        UT_ASSERT(bitsy.get_bits_remaining() == 8);
        UT_ASSERT(bitsy.get_exact_bits(5) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 3);
        UT_ASSERT_THROWS_MESSAGE("expected exception not thrown", bitsy.get_exact_bits(4), cppkit::ck_exception);
        UT_ASSERT(bitsy.get_bits_remaining() == 3);
        UT_ASSERT(bitsy.get_exact_bits(3) == 5);
        UT_ASSERT(bitsy.get_bits_remaining() == 0);
        UT_ASSERT_THROWS_MESSAGE("expected exception not thrown", bitsy.get_exact_bits(8), cppkit::ck_exception);
        UT_ASSERT(bitsy.get_bits_remaining() == 0);
    }

    {
        ck_bitsy<vector<uint8_t>::const_iterator> bitsy(buffer.begin(), buffer.end());
        UT_ASSERT(bitsy.get_bits_remaining() == 112);
        UT_ASSERT(bitsy.get_exact_bits(4) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 108);
        UT_ASSERT(bitsy.get_exact_bits(5) == 14);
        UT_ASSERT(bitsy.get_bits_remaining() == 103);
        UT_ASSERT(bitsy.get_exact_bits(18) == 2041);
        UT_ASSERT(bitsy.get_bits_remaining() == 85);
        UT_ASSERT(bitsy.get_bits_remaining() == 85);
        UT_ASSERT(bitsy.get_exact_bits(32) == 2146959360);
        UT_ASSERT(bitsy.get_bits_remaining() == 53);
        UT_ASSERT(bitsy.get_exact_bits(13) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 40);
        UT_ASSERT(bitsy.get_exact_bits(32) == (uint32_t)-1);
        UT_ASSERT(bitsy.get_bits_remaining() == 8);
        UT_ASSERT(bitsy.get_exact_bits(5) == 0);
        UT_ASSERT(bitsy.get_bits_remaining() == 3);
        UT_ASSERT_THROWS_MESSAGE("expected exception not thrown", bitsy.get_exact_bits(4), cppkit::ck_exception);
        UT_ASSERT(bitsy.get_bits_remaining() == 3);
        UT_ASSERT(bitsy.get_exact_bits(3) == 5);
        UT_ASSERT(bitsy.get_bits_remaining() == 0);
        UT_ASSERT_THROWS_MESSAGE("expected exception not thrown", bitsy.get_exact_bits(8), cppkit::ck_exception);
        UT_ASSERT(bitsy.get_bits_remaining() == 0);
    }
}

void ck_bitsy_test::test_heavy()
{
    vector<uint8_t> buffer(16,42);

    ck_bitsy<const uint8_t*> bitsy(&buffer.front(), &buffer.front() + buffer.size());

    for( int i = 0; i < 50000000; ++i )
    {
        bitsy.reset();
        bitsy.skip_bits( RANDOM() % 120 );
        uint32_t bits = bitsy.get_bits(8);
    }
}