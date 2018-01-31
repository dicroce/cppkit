
#ifndef cppkit_lru_cache_h
#define cppkit_lru_cache_h

#include <list>
#include <map>
#include <memory>
#include <string>
#include "cppkit/ck_string.h"

namespace cppkit
{

/// lru_cache implements a very simple least recently used cache (discards least recently used
/// items first). When you create an lru_cache instance, you specify the maximum number of items
/// you'd like stored in the cache. Upon initial insertion an item is considered to be the
/// most recently used. As other elements of the cache are accessed (via get()) this item
/// becomes relatively older. Eventually, if the item is never accessed a new item being
/// added will cause this item to be destroyed and removed from the cache.
///

template<class T>
class ck_lru_cache
{
private:
    struct node
    {
        T data;
        typename std::list<std::shared_ptr<struct node> >::iterator pos;
        std::string key;
    };

public:
    CK_API ck_lru_cache( size_t cacheSize ) :
                         _cacheSize( cacheSize ),
                         _itemsInCache( 0 ),
                         _storage(),
                         _index()
    {
    }

    CK_API ~ck_lru_cache() throw()
    {
    }

    CK_API inline bool is_cached( const ck_string& key ) const
    {
        return (_index.find(std::string(key.c_str()))!=_index.end())?true:false;
    }

    CK_API T get( const ck_string& key )
    {
        if( !is_cached(key) )
            CK_THROW(("Key not found in cache: %s",key.c_str()));

        auto cn = _index.find( std::string(key.c_str()) )->second;

        _storage.erase( cn->pos );
        _storage.push_front( cn );
        _storage.front()->pos = _storage.begin();

        return cn->data;
    }

    CK_API void put( const ck_string& key, const T& data )
    {
        // If the item is in the cache already just update the data, otherwise create a new slot.

        if( is_cached( key ) )
        {
            auto cn = _index.find( std::string(key.c_str()) )->second;

            cn->data = data;

            _storage.erase( cn->pos );
            _storage.push_front( cn );
            _storage.front()->pos = _storage.begin();
        }
        else
        {
            if( _full() )
            {
                auto cn = _storage.back();
                _storage.pop_back();
                _index.erase( std::string(cn->key) );
                --_itemsInCache;
            }

            auto cn = std::make_shared<node>();
            cn->data = data;
            cn->key = std::string(key);

            _storage.push_front( cn );
            _storage.front()->pos = _storage.begin();

            _index.insert( std::make_pair(std::string(key),cn) );
            ++_itemsInCache;
        }
    }

    CK_API void drop( const ck_string& key )
    {
        if( !is_cached( key ) )
            CK_THROW(("Key not found in cache: %s",key.c_str()));

        auto cn = _index.find( std::string(key) )->second;

        _storage.erase( cn->pos );

        _index.erase( std::string(key) );
        --_itemsInCache;
    }

    CK_API void drop_oldest()
    {
        if( _storage.empty() )
            return;

        drop( _storage.back()->key );
    }

    CK_API size_t size() const
    {
        return _index.size();
    }

    CK_API bool empty() const
    {
        return _index.empty();
    }

    CK_API void clear()
    {
        _index.clear();
        _storage.clear();
        _itemsInCache = 0;
    }

private:

    inline bool _full() const { return (_itemsInCache >= _cacheSize) ? true : false; }

    ck_lru_cache( const ck_lru_cache& obj );
    ck_lru_cache& operator = ( const ck_lru_cache& obj );

    size_t _cacheSize;
    size_t _itemsInCache;
    std::list<std::shared_ptr<struct node>> _storage;
    std::map<std::string,std::shared_ptr<struct node>> _index;
};
#if 0
template<class T>
class XTSCache
{
public:
    CK_API XTSCache( size_t cacheSize ) :
        _lok(),
        _cache( cacheSize )
    {
    }

    CK_API ~XTSCache() throw()
    {
    }

    CK_API inline bool IsCached( const XSDK::XString& key ) const
    {
        XGuard g( _lok );
        return _cache.IsCached( key );
    }

    CK_API bool Get( const XSDK::XString& key, T& data )
    {
        XGuard g( _lok );
        return _cache.Get( key, data );
    }

    CK_API void Put( const XSDK::XString& key, const T& data )
    {
        XGuard g( _lok );
        _cache.Put( key, data );
    }

    CK_API void Drop( const XSDK::XString& key )
    {
        XGuard g( _lok );
        _cache.Drop( key );
    }

private:
    mutable XSDK::XMutex _lok;
    XSDK::ck_lru_cache<T> _cache;
};
#endif
}

#endif
