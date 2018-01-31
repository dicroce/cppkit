
/// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=--=-=-=-=-=-
/// cppkit - http://www.cppkit.org
/// Copyright (c) 2013, Tony Di Croce
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without modification, are permitted
/// provided that the following conditions are met:
///
/// 1. Redistributions of source code must retain the above copyright notice, this list of conditions and
///    the following disclaimer.
/// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
///    and the following disclaimer in the documentation and/or other materials provided with the
///    distribution.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
/// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
/// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
/// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
/// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
/// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
/// TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
/// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///
/// The views and conclusions contained in the software and documentation are those of the authors and
/// should not be interpreted as representing official policies, either expressed or implied, of the cppkit
/// Project.
/// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=--=-=-=-=-=-

#ifndef cppkit_bitsy_h
#define cppkit_bitsy_h

#include "cppkit/os/ck_exports.h"
#include "cppkit/ck_exception.h"

class ck_bitsy_test;

namespace cppkit
{

/// ck_bitsy provides a simple way to read bits from a memory buffer.
/// One way to think about ck_bitsy is as a bit aware iterator that allows a reader
/// to read any number of bits (up to 32 at a time) from a buffer. ck_bitsy
/// internally keeps track of any pointers (and bitmasks) necessary. A user of
/// ck_bitsy need only write code like this:
///
/// ck_bitsy bitsy(foo, 128);
///
/// val = bitsy.GetBits(12);
/// val2 = bitsy.GetBits(16);
///
/// Where val and val2 were previously declared to be of type uint32_t.
template<typename iter>
class ck_bitsy
{
    friend class ::ck_bitsy_test;

public:
    ///
    CK_API ck_bitsy(iter begin, iter end);

    ///
    CK_API ck_bitsy(const ck_bitsy& obj);

    ///
    CK_API virtual ~ck_bitsy() {}

    ///
    CK_API ck_bitsy& operator=(const ck_bitsy& obj);

    ///
    CK_API void set_buffer(iter begin, iter end);

    ///
    CK_API void reset();

    /// Returns the next bitCount bits as uint32_t.
    ///
    /// If there aren't enough bits remaining, then all of the remaining
    /// bits are returned. If there are no bits remaining, then 0 is
    /// returned.
    CK_API uint32_t get_bits(int bitCount);

    /// Same as get_bits but throws if there aren't enough bits left.
    CK_API uint32_t get_exact_bits(int bitCount);

    /// Returns the number of bits remaining to be read.
    CK_API size_t get_bits_remaining() const;

    /// Returns the minimum number of bits needed to represent the value of dword
    CK_API int needed_bits(uint32_t dword);

    CK_API void skip_bits(int bitCount);

private:

    inline void _inc_pos( int inc = 1 );
    inline size_t _get_bytes_remaining() const;

    inline size_t _bits_remaining_in_mask( uint8_t mask ) const
    {
        switch( mask )
        {
        case 1:
            return 1;
        case 2:
            return 2;
        case 4:
            return 3;
        case 8:
            return 4;
        case 16:
            return 5;
        case 32:
            return 6;
        case 64:
            return 7;
        case 128:
            return 8;
        }

        CK_THROW(("Invalid mask."));        
    }

    iter _begin;
    iter _end;
    iter _pos;
    uint8_t _currentMask;
    mutable size_t _bytesRemaining;
};

template<typename iter>
ck_bitsy<iter>::ck_bitsy(iter begin, iter end)
    : _begin(begin),
      _end(end),
      _pos(begin),
      _currentMask(128),
      _bytesRemaining(-1)
{
}

template<typename iter>
ck_bitsy<iter>::ck_bitsy(const ck_bitsy& obj)
    : _begin(obj._begin),
      _end(obj._end),
      _pos(obj._pos),
      _currentMask(obj._currentMask),
      _bytesRemaining(obj._bytesRemaining)
{
}

template<typename iter>
ck_bitsy<iter>& ck_bitsy<iter>::operator =(const ck_bitsy& obj)
{
    _begin = obj._begin;
    _end = obj._end;
    _pos = obj._pos;
    _currentMask = obj._currentMask;
    _bytesRemaining = obj._bytesRemaining;
    return *this;
}

template<typename iter>
void ck_bitsy<iter>::set_buffer(iter begin, iter end)
{
    _begin = begin;
    _end = end;
    reset();
}

template<typename iter>
void ck_bitsy<iter>::reset()
{
    _pos = _begin;
    _currentMask = 128;
    _bytesRemaining = -1;
}

template<typename iter>
uint32_t ck_bitsy<iter>::get_bits(int bitCount)
{
    uint32_t outputBits = 0;

    for(int i = 0; i < bitCount && _pos < _end; ++i)
    {
        outputBits = outputBits << 1;

        if((*_pos) & _currentMask)
            outputBits |= 1;

        if(_currentMask == 1)
        {
            _currentMask = 128;
            _inc_pos();
        }
        else
            _currentMask = _currentMask >> 1;
    }

    return outputBits;
}

template<typename iter>
uint32_t ck_bitsy<iter>::get_exact_bits(int bitCount)
{
    const size_t remaining = get_bits_remaining();

    if(remaining < (size_t)bitCount)
        CK_THROW(("Not enough bits remaining."));

    return get_bits(bitCount);
}

template<typename iter>
size_t ck_bitsy<iter>::get_bits_remaining() const
{
    unsigned int bits = _bits_remaining_in_mask( _currentMask );
    return _get_bytes_remaining() * 8 - (8 - bits);
}

// Returns the minimum number of bits needed to represent the value of dword
template<typename iter>
int ck_bitsy<iter>::needed_bits(uint32_t dword)
{
    uint32_t mask = 1;
    uint32_t val = 0;
    int bitsRequired = 0;

    while(val < dword)
    {
        val = val << 1;
        val |= mask;
        ++bitsRequired;
    }

    return bitsRequired;
}

template<typename iter>
void ck_bitsy<iter>::skip_bits(int bitCount)
{
    int byteCount = bitCount / 8;
    int remainderBits = bitCount % 8;

    if( byteCount > 0 )
    {
        _inc_pos( byteCount );

        auto bitsRemainingInCurrent = _bits_remaining_in_mask( _currentMask );

        if( bitsRemainingInCurrent > remainderBits )
            _currentMask >> remainderBits;
        else
        {
            _inc_pos();
            _currentMask = 128 >> (remainderBits-bitsRemainingInCurrent);
        }
    }
    else
    {
        auto bitsRemainingInCurrent = _bits_remaining_in_mask( _currentMask );

        if( bitsRemainingInCurrent > remainderBits )
            _currentMask >> remainderBits;
        else
        {
            _inc_pos();
            _currentMask = 128 >> (remainderBits-bitsRemainingInCurrent);
        }        
    }
}

template<typename iter>
inline void ck_bitsy<iter>::_inc_pos( int inc )
{
    _pos += inc;
    if(_bytesRemaining == -1)
        _bytesRemaining = _get_bytes_remaining();
    else
        _bytesRemaining -= inc;
}

template<>
inline void ck_bitsy<const uint8_t*>::_inc_pos( int inc )
{
    _pos += inc;
}

template<typename iter>
inline size_t ck_bitsy<iter>::_get_bytes_remaining() const
{
    if(_bytesRemaining == -1)
    {
        _bytesRemaining = 0;
        iter temp = _pos;
        while(temp != _end) ++temp, ++_bytesRemaining;
    }

    return _bytesRemaining;
}

template<>
inline size_t ck_bitsy<const uint8_t*>::_get_bytes_remaining() const
{
    return _end - _pos;
}

};

#endif
