
#ifndef _cppkit_ck_blob_tree_h
#define _cppkit_ck_blob_tree_h

#include "cppkit/ck_exception.h"

#include <map>
#include <string>
#include <vector>
#include <stdexcept>

class test_cppkit_ck_blob_tree;

namespace cppkit
{

class ck_blob_tree
{
    friend class ::test_cppkit_ck_blob_tree;

public:
    enum node_type
    {
        NT_OBJECT,
        NT_ARRAY,
        NT_LEAF
    };

    static std::vector<uint8_t> serialize(const ck_blob_tree& rt, uint32_t version);

    static ck_blob_tree deserialize(const uint8_t* p, size_t size, uint32_t& version);

    ck_blob_tree& operator[](const std::string& key)
    {
        if(!_childrenByIndex.empty())
            CK_STHROW(ck_internal_exception, ("ck_blob_tree node cannot be both an array and an object."));
        return _children[key];
    }

    ck_blob_tree& operator[](size_t index)
    {
        if(!_children.empty())
            CK_STHROW(ck_internal_exception, ("ck_blob_tree node cannot be both an object and an array."));
        if(_childrenByIndex.size() < (index+1))
            _childrenByIndex.resize(index+1);
        return _childrenByIndex[index];
    }

    size_t size()
    {
        if(!_children.empty())
            CK_STHROW(ck_internal_exception, ("ck_blob_tree node cannot be both an object and an array."));
        return _childrenByIndex.size();
    }

    ck_blob_tree& operator=(const std::pair<size_t, const uint8_t*>& payload)
    {
        _payload = payload;
        return *this;
    }

    inline std::pair<size_t, const uint8_t*> get() const
    {
        return _payload;
    }

private:
    static size_t _sizeof_treeb(const ck_blob_tree& rt);
    static size_t _write_treeb(const ck_blob_tree& rt, uint8_t* p, uint8_t* end);
    static size_t _read_treeb(const uint8_t* p, const uint8_t* end, ck_blob_tree& rt);
    inline static size_t _bytes_left(const uint8_t* p, const uint8_t* end) { return end - p; }

    std::map<std::string, ck_blob_tree> _children;
    std::vector<ck_blob_tree> _childrenByIndex;
    std::pair<size_t, const uint8_t*> _payload;
};

}

#endif