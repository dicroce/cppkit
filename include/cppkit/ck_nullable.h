
#ifndef cppkit_ck_nullable_h
#define cppkit_ck_nullable_h

#include "cppkit/ck_exception.h"

namespace cppkit
{

/// Allows for a nullable value on the stack.
template<typename T>
class ck_nullable
{
public:
    ck_nullable() :
        _value(),
        _is_null( true )
    {
    }

    ck_nullable(const ck_nullable& obj) :
        _value(obj._value),
        _is_null(obj._is_null)
    {
    }

    ck_nullable(ck_nullable&& obj) noexcept :
        _value(std::move(obj._value)),
        _is_null(std::move(obj._is_null))
    {
        obj._value = T();
        obj._is_null = true;
    }

    ck_nullable( T value ) :
        _value( value ),
        _is_null( false )
    {
    }

    ~ck_nullable() noexcept {}

    ck_nullable& operator = ( const ck_nullable& rhs )
    {
        this->_value = rhs._value;
        this->_is_null = rhs._is_null;
        return *this;
    }

    ck_nullable& operator = (ck_nullable&& rhs) noexcept
    {
        _value = std::move(rhs._value);
        _is_null = std::move(rhs._is_null);
        rhs._value = T();
        rhs._is_null = true;
        return *this;
    }

    ck_nullable& operator = ( const T& rhs )
    {
        this->_value = rhs;
        this->_is_null = false;
        return *this;
    }

    operator bool() const
    {
        return !_is_null;
    }

    T value() const
    {
        return _value;
    }

    void set_value( T value )
    {
        _value = value;
        _is_null = false;
    }

    bool is_null() const
    {
        return _is_null;
    }

    void clear()
    {
        _value = T();
        _is_null = true;
    }

    friend bool operator == ( const ck_nullable& lhs, const ck_nullable& rhs )
    {
        return( lhs._is_null && rhs._is_null ) || (lhs._value == rhs._value);
    }

    friend bool operator == ( const ck_nullable& lhs, const T& rhs )
    {
        return !lhs._is_null && lhs._value == rhs;
    }

    friend bool operator == ( const T& lhs, const ck_nullable& rhs )
    {
        return rhs == lhs;
    }

    friend bool operator != ( const ck_nullable& lhs, const ck_nullable& rhs )
    {
        return !(lhs == rhs);
    }

    friend bool operator != ( const ck_nullable& lhs, const T& rhs )
    {
        return !(lhs == rhs);
    }

    friend bool operator != ( const T& lhs, const ck_nullable& rhs )
    {
        return !(lhs == rhs);
    }

private:
    T _value;
    bool _is_null;
};

template<typename T>
bool operator == ( const T& lhs, const ck_nullable<T>& rhs )
{
    return rhs == lhs;
}

template<typename T>
bool operator != ( const T& lhs, const ck_nullable<T>& rhs )
{
    return rhs != lhs;
}

}

#endif
