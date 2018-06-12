
#ifndef _cppkit_ck_functional_h
#define _cppkit_ck_functional_h

#include <vector>
#include <algorithm>
#include <type_traits>
#include <numeric>
#include <functional>
#include <vector>

namespace cppkit
{

namespace ck_funky
{

// funky provides some rudimentary functional programming facilities. In most
// cases these functions are simply providing value semantics on top of existing
// std::algorithm's.. Which of course comes at the cost of performance, but they
// are super useful if you're careful.

template<typename C, typename F>
auto fmap(const C& cont, F f)
{
    std::vector<typename C::value_type> out(cont.size());
    std::transform(std::begin(cont), std::end(cont), std::begin(out), f);
    return out;
}

template<typename C, typename F>
auto filter(const C& cont, F f)
{
    std::vector<typename C::value_type> out;
    std::copy_if(std::begin(cont), std::end(cont), std::back_inserter(out), f);
    return out;
}

template<typename C, typename F, typename INIT>
auto reduce(const C& cont, INIT init, F f)
{
    return std::accumulate(std::begin(cont), std::end(cont), init, f);
}

// Returns the items in a that are not in b.
template<typename C>
auto set_diff(const C& a, const C& b)
{
    std::vector<typename C::value_type> output;
    for(auto& s : a)
    {
        auto i = begin(b), e = end(b), f = end(b);
        for( ; i != e; ++i)
        {
            if(*i == s)
                f = i;
        }

        if(f == end(b))
            output.push_back(s);
    }

    return output;
}

// This is primarily useful for map<>'s since they have no similar functionality (until library fundamentals TS v2)
template<typename CONT, typename PRED>
void erase_if(CONT& c, const PRED& p)
{
    for(auto it = c.begin(); it != c.end();)
    {
        if(p(*it))
            it = c.erase(it);
        else ++it;
    }
}

}

}

#endif
