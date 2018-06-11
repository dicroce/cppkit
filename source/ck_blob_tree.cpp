
#include "cppkit/ck_blob_tree.h"
#include "cppkit/ck_socket.h"

using namespace cppkit;
using namespace std;

vector<uint8_t> ck_blob_tree::serialize(const ck_blob_tree& rt, uint32_t version)
{
    auto size = sizeof(uint32_t) + _sizeof_treeb(rt);
    vector<uint8_t> buffer(size);
    uint8_t* p = &buffer[0];
    uint32_t word = ck_networking::ck_htonl(version);
    *(uint32_t*)p = word;
    p+=sizeof(uint32_t);
    // &buffer[size] is our sentinel (1 past the end)
    _write_treeb(rt, p, &buffer[size]);
    return buffer;
}

ck_blob_tree ck_blob_tree::deserialize(const uint8_t* p, size_t size, uint32_t& version)
{
    auto end = p + size;
    uint32_t word = *(uint32_t*)p;
    p+=sizeof(uint32_t);
    version = ck_networking::ck_ntohl(word);
    ck_blob_tree obj; 
    _read_treeb(p, end, obj);
    return obj;
}

size_t ck_blob_tree::_sizeof_treeb(const ck_blob_tree& rt)
{
    size_t sum = 1 + sizeof(uint32_t); // type & num children

    if(!rt._children.empty())
    {
        for(auto& cp : rt._children)
            sum += sizeof(uint16_t) + cp.first.length() + _sizeof_treeb(cp.second);
    }
    else if(!rt._childrenByIndex.empty())
    {
        for(auto& c : rt._childrenByIndex)
            sum += _sizeof_treeb(c);
    }
    else sum += sizeof(uint32_t) + rt._payload.first;

    return sum;
}

size_t ck_blob_tree::_write_treeb(const ck_blob_tree& rt, uint8_t* p, uint8_t* end)
{
    uint8_t* fp = p;

    if(_bytes_left(p, end) < 5)
        CK_STHROW(ck_invalid_argument_exception, ("Buffer too small to serialize ck_blob_tree."));


    uint8_t type = (!rt._children.empty())?NT_OBJECT:(!rt._childrenByIndex.empty())?NT_ARRAY:NT_LEAF;
    *p = type;
    ++p;

    if(type == NT_OBJECT || type == NT_ARRAY)
    {
        uint32_t numChildren = (type==NT_OBJECT)?rt._children.size():rt._childrenByIndex.size();
        uint32_t word = ck_networking::ck_htonl(numChildren);
        *(uint32_t*)p = word;
        p+=sizeof(uint32_t);

        if(type==NT_OBJECT)
        {
            for(auto& cp : rt._children)
            {
                if(_bytes_left(p, end) < sizeof(uint16_t))
                    CK_STHROW(ck_invalid_argument_exception, ("Buffer too small to serialize ck_blob_tree."));
                
                uint16_t nameSize = cp.first.length();
                uint16_t shortVal = ck_networking::ck_htons(nameSize);
                *(uint16_t*)p = shortVal;
                p+=sizeof(uint16_t);

                if(_bytes_left(p, end) < nameSize)
                    CK_STHROW(ck_invalid_argument_exception, ("Buffer too small to serialize ck_blob_tree."));            

                memcpy(p, cp.first.c_str(), nameSize);
                p+=nameSize;

                p+=_write_treeb(cp.second, p, end);
            }
        }
        else
        {
            for(auto& c : rt._childrenByIndex)
                p+=_write_treeb(c, p, end);
        }
    }
    else
    {
        if(_bytes_left(p, end) < sizeof(uint32_t))
            CK_STHROW(ck_invalid_argument_exception, ("Buffer too small to serialize ck_blob_tree."));            

        uint32_t payloadSize = rt._payload.first;
        uint32_t word = ck_networking::ck_htonl(payloadSize);
        *(uint32_t*)p = word;
        p+=sizeof(uint32_t);

        if(_bytes_left(p, end) < payloadSize)
            CK_STHROW(ck_invalid_argument_exception, ("Buffer too small to serialize ck_blob_tree."));            

        memcpy(p, rt._payload.second, payloadSize);
        p+=payloadSize;
    }

    return p - fp;
}

size_t ck_blob_tree::_read_treeb(const uint8_t* p, const uint8_t* end, ck_blob_tree& rt)
{
    if(_bytes_left(p, end) < 5)
        CK_STHROW(ck_invalid_argument_exception, ("Buffer too small to deserialize ck_blob_tree."));            

    const uint8_t* fp = p;
    uint8_t type = *p;
    ++p;

    if(type == NT_OBJECT || type == NT_ARRAY)
    {
        uint32_t word = *(uint32_t*)p;
        p+=sizeof(uint32_t);
        uint32_t numChildren = ck_networking::ck_ntohl(word);

        if(type == NT_OBJECT)
        {
            for(size_t i = 0; i < numChildren; ++i)
            {
                if(_bytes_left(p, end) < sizeof(uint16_t))
                    CK_STHROW(ck_invalid_argument_exception, ("Buffer too small to deserialize ck_blob_tree."));            

                uint16_t shortVal = *(uint16_t*)p;
                p+=sizeof(uint16_t);
                uint16_t nameLen = ck_networking::ck_ntohs(shortVal);

                if(_bytes_left(p, end) < nameLen)
                    CK_STHROW(ck_invalid_argument_exception, ("Buffer too small to deserialize ck_blob_tree."));            

                string name((char*)p, nameLen);
                p+=nameLen;
                ck_blob_tree childObj;
                p+=_read_treeb(p, end, childObj);
                rt._children[name] = childObj;
            }
        }
        else
        {
            rt._childrenByIndex.resize(numChildren);
            for(size_t i = 0; i < numChildren; ++i)
                p+=_read_treeb(p, end, rt._childrenByIndex[i]);
        }
    }
    else
    {
        if(_bytes_left(p, end) < sizeof(uint32_t))
            CK_STHROW(ck_invalid_argument_exception, ("Buffer too small to deserialize ck_blob_tree."));            

        uint32_t word = *(uint32_t*)p;
        p+=sizeof(uint32_t);
        uint32_t payloadSize = ck_networking::ck_ntohl(word);

        if(_bytes_left(p, end) < payloadSize)
            CK_STHROW(ck_invalid_argument_exception, ("Buffer too small to deserialize ck_blob_tree."));

        rt._payload = make_pair((size_t)payloadSize, p);
        p+=payloadSize;
    }

    return p - fp;
}
