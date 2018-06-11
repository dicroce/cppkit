
#include "ck_lru_cache_test.h"
#include "cppkit/ck_lru_cache.h"
#include "cppkit/ck_pool.h"
#include "cppkit/ck_string.h"
#include <list>

using namespace std;
using namespace cppkit;

REGISTER_TEST_FIXTURE(ck_lru_cache_test);

void ck_lru_cache_test::test_constructor()
{
    ck_lru_cache<int> cache(10);
}

void ck_lru_cache_test::test_get()
{
    ck_lru_cache<int> cache(10);

    cache.put( "0", 0 );
    cache.put( "1", 1 );
    cache.put( "2", 2 );
    cache.put( "3", 3 );
    cache.put( "4", 4 );
    cache.put( "5", 5 );
    cache.put( "6", 6 );
    cache.put( "7", 7 );
    cache.put( "8", 8 );
    cache.put( "9", 9 );

    UT_ASSERT( cache.get( "0" ) == 0 );
    UT_ASSERT( cache.get( "9" ) == 9 );
} 

void ck_lru_cache_test::test_miss()
{
    ck_lru_cache<int> cache(5);

    cache.put( "0", 0 );
    cache.put( "1", 1 );
    cache.put( "2", 2 );
    cache.put( "3", 3 );
    cache.put( "4", 4 );

    UT_ASSERT( cache.get( "0" ) == 0 );
    UT_ASSERT( cache.get( "1" ) == 1 );
    UT_ASSERT( cache.get( "2" ) == 2 );
    UT_ASSERT( cache.get( "3" ) == 3 );
    UT_ASSERT( cache.get( "4" ) == 4 );

    cache.put( "5", 5 );

    UT_ASSERT( cache.is_cached( "0" ) == false );
    UT_ASSERT( cache.is_cached( "1" ) == true );
    UT_ASSERT( cache.is_cached( "2" ) == true );
    UT_ASSERT( cache.is_cached( "3" ) == true );
    UT_ASSERT( cache.is_cached( "4" ) == true );
    UT_ASSERT( cache.is_cached( "5" ) == true );

    cache.put( "6" , 6 );

    UT_ASSERT( cache.is_cached( "1" ) == false );
}
