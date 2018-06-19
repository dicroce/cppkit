
#include "test_ck_pool.h"
#include "cppkit/ck_pool.h"
#include "cppkit/ck_string_utils.h"
#include <list>

using namespace std;
using namespace cppkit;

REGISTER_TEST_FIXTURE(test_ck_pool);

void test_ck_pool::test_constructor()
{
    ck_pool<string> pool( 100 );
    RTF_ASSERT( pool._free.size() == 100 );
    RTF_ASSERT( pool._all.size() == 100 );
    RTF_ASSERT( pool.get_num_free() == 100 );
    RTF_ASSERT( !pool.empty() );
}

void test_ck_pool::test_get()
{
    ck_pool<string> pool( 100, "Hola!" );
    RTF_ASSERT( pool._free.size() == 100 );
    RTF_ASSERT( pool._all.size() == 100 );
    RTF_ASSERT( pool.get_num_free() == 100 );
    RTF_ASSERT( !pool.empty() );

    {
        shared_ptr<string> s1 = pool.get();
        RTF_ASSERT( *s1 == "Hola!" );
        RTF_ASSERT( pool.get_num_free() == 99 );
    }

    RTF_ASSERT( pool.get_num_free() == 100 );
}

void test_ck_pool::test_deleter_transfer()
{
    ck_pool<string> pool( 100, "Hola!" );
    RTF_ASSERT( pool._free.size() == 100 );
    RTF_ASSERT( pool._all.size() == 100 );
    RTF_ASSERT( pool.get_num_free() == 100 );
    RTF_ASSERT( !pool.empty() );

    {
        shared_ptr<string> s2;

        {
            shared_ptr<string> s1 = pool.get();
            RTF_ASSERT( *s1 == "Hola!" );
            RTF_ASSERT( pool.get_num_free() == 99 );
            s2 = s1; // s2 (at wider scope) now references the string.
        }

        RTF_ASSERT( pool.get_num_free() == 99 ); // because s2 still exists, the string is not back in the pool
    }

    // but now s2 is destroyed, so it should be back in the pool.
    RTF_ASSERT( pool.get_num_free() == 100 );
}

void test_ck_pool::test_empty()
{
    ck_pool<int> pool( 100, 42 );

    RTF_ASSERT( pool.get_num_free() == 100 );

    {
        list<shared_ptr<int> > l1;
        while( !pool.empty() )
            l1.push_back( pool.get() );
        RTF_ASSERT( pool.empty() );
    }

    RTF_ASSERT( !pool.empty() );
    RTF_ASSERT( pool.get_num_free() == 100 );
}
