
#ifndef cppkit_variant_h
#define cppkit_variant_h

#include "cppkit/ck_types.h"
#include "cppkit/os/ck_exports.h"
#include "cppkit/ck_exception.h"
#include "cppkit/ck_string.h"

namespace cppkit
{

class ck_variant
{
public:
    enum VARIANT_TYPE
    {
        VARTYPE_EMPTY = 0,
        VARTYPE_BOOL,
	    VARTYPE_INT8,
	    VARTYPE_UINT8,
	    VARTYPE_INT16,
	    VARTYPE_UINT16,
	    VARTYPE_INT32,
	    VARTYPE_UINT32,
	    VARTYPE_INT64,
	    VARTYPE_UINT64,
	    VARTYPE_FLOAT,
	    VARTYPE_DOUBLE,
	    VARTYPE_STRING
    };

    CK_API ck_variant();
    CK_API ck_variant( const ck_variant& other );
    CK_API ck_variant( ck_variant&& other ) throw();

    CK_API ck_variant& operator = ( const ck_variant& other );

    CK_API bool is_empty() const;
    CK_API VARIANT_TYPE get_type() const;

    CK_API static ck_variant from_bool( bool v );
    CK_API void set_bool( bool v );
    CK_API bool to_bool() const;

    CK_API static ck_variant from_int8( int8_t v );
    CK_API void set_int8( int8_t v );
    CK_API int8_t to_int8() const;

    CK_API static ck_variant from_uint8( uint8_t v );
    CK_API void set_uint8( uint8_t v );
    CK_API uint8_t to_uint8() const;

    CK_API static ck_variant from_int16( int16_t v );
    CK_API void set_int16( int16_t v );
    CK_API int16_t to_int16() const;

    CK_API static ck_variant from_uint16( uint16_t v );
    CK_API void set_uint16( uint16_t v );
    CK_API uint16_t to_uint16() const;

    CK_API static ck_variant from_int32( int32_t v );
    CK_API void set_int32( int32_t v );
    CK_API int32_t to_int32() const;

    CK_API static ck_variant from_uint32( uint32_t v );
    CK_API void set_uint32( uint32_t v );
    CK_API uint32_t to_uint32() const;

    CK_API static ck_variant from_int64( int64_t v );
    CK_API void set_int64( int64_t v );
    CK_API int64_t to_int64() const;

    CK_API static ck_variant from_uint64( uint64_t v );
    CK_API void set_uint64( uint64_t v );
    CK_API uint64_t to_uint64() const;

    CK_API static ck_variant from_float( float v );
    CK_API void set_float( float v );
    CK_API float to_float() const;

    CK_API static ck_variant from_double( double v );
    CK_API void set_double( double v );
    CK_API double to_double() const;

    CK_API static ck_variant from_string( const ck_string& v );
    CK_API void set_string( const ck_string& v );
    CK_API ck_string to_string() const;

private:
    struct CK_VARIANT
    {
        VARIANT_TYPE type;
        union
        {
            bool                 b;
 	    int8_t               s8;
	    uint8_t              u8;
	    int16_t              s16;
	    uint16_t             u16;
	    int32_t              s32;
	    uint32_t             u32;
	    int64_t              s64;
	    uint64_t             u64;
	    float                f;
	    double               d;
        } value;
    };

    CK_VARIANT _variant;
    ck_string _text;
};

}

#endif

