
#include "ck_functional_test.h"
#include "cppkit/ck_functional.h"
#include <map>
#include <vector>

using namespace std;
using namespace cppkit;

REGISTER_TEST_FIXTURE(ck_functional_test);

void ck_functional_test::test_basic_fmap()
{
    vector<int> inp = {5, 10, 15, 20};
    auto increment = [](int v){ return v + 1; };
    auto out = funky::fmap(inp, increment);
    UT_ASSERT(out[0] == 6);
    UT_ASSERT(out[1] == 11);
    UT_ASSERT(out[2] == 16);
    UT_ASSERT(out[3] == 21);
}

void ck_functional_test::test_basic_filter()
{
    vector<int> inp = {5, 10, 15, 20};
    auto is_even = [](int v){ return v % 2 == 0; };
    auto out = funky::filter(inp, is_even);
    UT_ASSERT(out[0] == 10);
    UT_ASSERT(out[1] == 20);
}

void ck_functional_test::test_basic_reduce()
{
    vector<int> inp = {5, 10, 15, 20};
    UT_ASSERT(funky::reduce(inp, 0, std::plus<>()) == 50);
    // reduce with initial value of 5...
    UT_ASSERT(funky::reduce(inp, 5, std::plus<>()) == 55);
}

void ck_functional_test::test_combined()
{
    vector<int> inp = {4, 9, 14, 19};
    auto increment = [](int v){return v+1;};
    auto is_even = [](int v){return (v%2)==0;};
    UT_ASSERT(funky::reduce(funky::filter(funky::fmap(inp, increment), is_even), 1, std::multiplies<>()) == 200);
}

void ck_functional_test::test_set_diff()
{
    vector<int> inp_a = {1, 2, 3, 4, 5};
    vector<int> inp_b = {4, 5, 6, 7, 8};

    auto out = funky::set_diff(inp_a, inp_b);
    UT_ASSERT(out.size() == 3);
    UT_ASSERT(find(begin(out), end(out), 1) != out.end());
    UT_ASSERT(find(begin(out), end(out), 2) != out.end());
    UT_ASSERT(find(begin(out), end(out), 3) != out.end());
    UT_ASSERT(find(begin(out), end(out), 4) == out.end());
    UT_ASSERT(find(begin(out), end(out), 5) == out.end());

    // no difference...
    auto out2 = funky::set_diff(inp_a, inp_a);
    UT_ASSERT(out2.size() == 0);
}

void ck_functional_test::test_erase_if()
{
    vector<int> inp = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // erase even...
    funky::erase_if(inp, [](int a){return a % 2 == 0;});
    UT_ASSERT(inp.size() == 5);

    // erase nothing...
    funky::erase_if(inp, [](int a){return a % 2 == 0;});
    UT_ASSERT(inp.size() == 5);

    // erase everything else...
    funky::erase_if(inp, [](int a){return a % 2 == 1;});
    UT_ASSERT(inp.size() == 0);
    UT_ASSERT(inp.empty());
}

void ck_functional_test::test_complicate_erase()
{
    map<string, int> things;
    things.insert(make_pair("1",1));
    things.insert(make_pair("2",2));
    things.insert(make_pair("3",3));
    things.insert(make_pair("4",4));
    things.insert(make_pair("5",5));

    vector<int> matching = {2, 4};

    funky::erase_if(things,[&matching](const pair<string, int>& item){
                        return find_if(matching.begin(), matching.end(),
                                       [&item](int val){ return val == item.second; }) != matching.end();
                    });

    UT_ASSERT(things.size() == 3);
    UT_ASSERT(things.find("1") != things.end());
    UT_ASSERT(things.find("2") == things.end());
    UT_ASSERT(things.find("3") != things.end());
    UT_ASSERT(things.find("4") == things.end());
    UT_ASSERT(things.find("5") != things.end());
}