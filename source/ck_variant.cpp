
#include "cppkit/ck_variant.h"

using namespace cppkit;

ck_variant::ck_variant() :
    _variant(),
    _text()
{
    _variant.type = VARTYPE_EMPTY;
}

ck_variant::ck_variant( const ck_variant& other ) :
    _variant( other._variant ),
    _text( other._text )
{
}

ck_variant::ck_variant( ck_variant&& other ) throw() :
    _variant( std::move( other._variant ) ),
    _text( std::move( other._text ) )
{
}

ck_variant& ck_variant::operator = ( const ck_variant& other )
{
    _variant = other._variant;
    _text = other._text;

    return *this;
}

bool ck_variant::is_empty() const
{
    return (_variant.type==VARTYPE_EMPTY) ? true : false;
}

ck_variant::VARIANT_TYPE ck_variant::get_type() const
{
    return _variant.type;
}

ck_variant ck_variant::from_bool( bool v )
{
    ck_variant var;
    var.set_bool( v );
    return var;
}

void ck_variant::set_bool( bool v )
{
    _variant.value.b = v;
    _variant.type = VARTYPE_BOOL;
}

bool ck_variant::to_bool() const
{
    if( _variant.type != VARTYPE_EMPTY &&
        _variant.type != VARTYPE_STRING )
        return _variant.value.b;

    if( _text.empty() )
        CK_THROW(( "Unable to convert empty variant." ));

    return (_text.to_uint64() > 0) ? true : false;
}

ck_variant ck_variant::from_int8( int8_t v )
{
    ck_variant var;
    var.set_int8( v );
    return var;
}

void ck_variant::set_int8( int8_t v )
{
    _variant.value.s8 = v;
    _variant.type = VARTYPE_INT8;
}

int8_t ck_variant::to_int8() const
{
    if( _variant.type != VARTYPE_EMPTY &&
        _variant.type != VARTYPE_STRING )
        return _variant.value.s8;

    if( _text.empty() )
        CK_THROW(( "Unable to convert empty variant." ));

    return _text.to_int8();
}

ck_variant ck_variant::from_uint8( uint8_t v )
{
    ck_variant var;
    var.set_uint8( v );
    return var;
}

void ck_variant::set_uint8( uint8_t v )
{
    _variant.value.u8 = v;
    _variant.type = VARTYPE_UINT8;
}

uint8_t ck_variant::to_uint8() const
{
    if( _variant.type != VARTYPE_EMPTY &&
        _variant.type != VARTYPE_STRING )
        return _variant.value.u8;

    if( _text.empty() )
        CK_THROW(( "Unable to convert empty variant." ));

    return _text.to_uint8();
}

ck_variant ck_variant::from_int16( int16_t v )
{
    ck_variant var;
    var.set_int16( v );
    return var;
}

void ck_variant::set_int16( int16_t v )
{
    _variant.value.s16 = v;
    _variant.type = VARTYPE_INT16;
}

int16_t ck_variant::to_int16() const
{
    if( _variant.type != VARTYPE_EMPTY &&
        _variant.type != VARTYPE_STRING )
        return _variant.value.s16;

    if( _text.empty() )
        CK_THROW(( "Unable to convert empty variant." ));

    return _text.to_int16();
}

ck_variant ck_variant::from_uint16( uint16_t v )
{
    ck_variant var;
    var.set_uint16( v );
    return var;
}

void ck_variant::set_uint16( uint16_t v )
{
    _variant.value.u16 = v;
    _variant.type = VARTYPE_UINT16;
}

uint16_t ck_variant::to_uint16() const
{
    if( _variant.type != VARTYPE_EMPTY &&
        _variant.type != VARTYPE_STRING )
        return _variant.value.u16;

    if( _text.empty() )
        CK_THROW(( "Unable to convert empty variant." ));

    return _text.to_uint16();
}

ck_variant ck_variant::from_int32( int32_t v )
{
    ck_variant var;
    var.set_int32( v );
    return var;
}

void ck_variant::set_int32( int32_t v )
{
    _variant.value.s32 = v;
    _variant.type = VARTYPE_INT32;
}

int32_t ck_variant::to_int32() const
{
    if( _variant.type != VARTYPE_EMPTY &&
        _variant.type != VARTYPE_STRING )
        return _variant.value.s32;

    if( _text.empty() )
        CK_THROW(( "Unable to convert empty variant." ));

    return _text.to_int32();
}

ck_variant ck_variant::from_uint32( uint32_t v )
{
    ck_variant var;
    var.set_uint32( v );
    return var;
}

void ck_variant::set_uint32( uint32_t v )
{
    _variant.value.u32 = v;
    _variant.type = VARTYPE_UINT32;
}

uint32_t ck_variant::to_uint32() const
{
    if( _variant.type != VARTYPE_EMPTY &&
        _variant.type != VARTYPE_STRING )
        return _variant.value.u32;

    if( _text.empty() )
        CK_THROW(( "Unable to convert empty variant." ));

    return _text.to_uint32();
}

ck_variant ck_variant::from_int64( int64_t v )
{
    ck_variant var;
    var.set_int64( v );
    return var;
}

void ck_variant::set_int64( int64_t v )
{
    _variant.value.s64 = v;
    _variant.type = VARTYPE_INT64;
}

int64_t ck_variant::to_int64() const
{
    if( _variant.type != VARTYPE_EMPTY &&
        _variant.type != VARTYPE_STRING )
        return _variant.value.s64;

    if( _text.empty() )
        CK_THROW(( "Unable to convert empty variant." ));

    return _text.to_int64();
}

ck_variant ck_variant::from_uint64( uint64_t v )
{
    ck_variant var;
    var.set_uint64( v );
    return var;
}

void ck_variant::set_uint64( uint64_t v )
{
    _variant.value.u64 = v;
    _variant.type = VARTYPE_UINT64;
}

uint64_t ck_variant::to_uint64() const
{
    if( _variant.type != VARTYPE_EMPTY &&
        _variant.type != VARTYPE_STRING )
        return _variant.value.u64;

    if( _text.empty() )
        CK_THROW(( "Unable to convert empty variant." ));

    return _text.to_uint64();
}

ck_variant ck_variant::from_float( float v )
{
    ck_variant var;
    var.set_float( v );
    return var;
}

void ck_variant::set_float( float v )
{
    _variant.value.f = v;
    _variant.type = VARTYPE_FLOAT;
}

float ck_variant::to_float() const
{
    if( _variant.type != VARTYPE_EMPTY &&
        _variant.type != VARTYPE_STRING )
        return _variant.value.f;

    if( _text.empty() )
        CK_THROW(( "Unable to convert empty variant." ));

    return _text.to_float();
}

ck_variant ck_variant::from_double( double v )
{
    ck_variant var;
    var.set_double( v );
    return var;
}

void ck_variant::set_double( double v )
{
    _variant.value.d = v;
    _variant.type = VARTYPE_DOUBLE;
}

double ck_variant::to_double() const
{
    if( _variant.type != VARTYPE_EMPTY &&
        _variant.type != VARTYPE_STRING )
        return _variant.value.d;

    if( _text.empty() )
        CK_THROW(( "Unable to convert empty variant." ));

    return _text.to_double();
}

ck_variant ck_variant::from_string( const ck_string& v )
{
    ck_variant var;
    var.set_string( v );
    return var;
}

void ck_variant::set_string( const ck_string& v )
{
    _variant.type = VARTYPE_STRING;
    _text = v;
}

ck_string ck_variant::to_string() const
{
    if( !_text.empty() )
        return _text;

    return ck_string::from_uint64( _variant.value.u64 );
}
