
#include "framework.h"
#include "ck_lower_bound_test.h"
#include "cppkit/algorithms/ck_lower_bound.h"

using namespace std;
using namespace cppkit;

REGISTER_TEST_FIXTURE(ck_lower_bound_test);

void ck_lower_bound_test::setup()
{
    _storage.push_back( 10 );
    _storage.push_back( 20 );
    _storage.push_back( 30 );
    _storage.push_back( 40 );
    _storage.push_back( 50 );
    _storage.push_back( 60 );
    _storage.push_back( 70 );
    _storage.push_back( 80 );
    _storage.push_back( 90 );
    _storage.push_back( 100 );
    _storage.push_back( 110 );
    _storage.push_back( 120 );
}

void ck_lower_bound_test::teardown()
{
    _storage.clear();
}

void ck_lower_bound_test::test_before()
{
    int target = 5;

    uint8_t* b = (uint8_t*)&_storage[0];
    uint8_t* e = (uint8_t*)&_storage[11];

    uint8_t* result = lower_bound_bytes( b,
                                         e,
                                         (uint8_t*)&target,
                                         sizeof(int),
                                         []( const uint8_t* p1, const uint8_t* p2 )->int {
                                             const int* pi1 = (const int*)p1;
                                             const int* pi2 = (const int*)p2;
                                             if( *(int*)p1 < *(int*)p2 )
                                                 return -1;
                                             else if( *(int*)p1 > *(int*)p2 )
                                                 return 1;
                                             return 0;
                                         });

    UT_ASSERT( result != e );
    UT_ASSERT( *(int*)result == 10 );
}

void ck_lower_bound_test::test_first_exact()
{
    int target = 10;

    uint8_t* b = (uint8_t*)&_storage[0];
    uint8_t* e = (uint8_t*)&_storage[11];

    uint8_t* result = lower_bound_bytes( b,
                                         e,
                                         (uint8_t*)&target,
                                         sizeof(int),
                                         []( const uint8_t* p1, const uint8_t* p2 )->int {
                                             const int* pi1 = (const int*)p1;
                                             const int* pi2 = (const int*)p2;
                                             if( *(int*)p1 < *(int*)p2 )
                                                 return -1;
                                             else if( *(int*)p1 > *(int*)p2 )
                                                 return 1;
                                             return 0;
                                         });

    UT_ASSERT( result != e );
    UT_ASSERT( *(int*)result == 10 );
}

void ck_lower_bound_test::test_middle()
{
    int target = 75;

    uint8_t* b = (uint8_t*)&_storage[0];
    uint8_t* e = (uint8_t*)&_storage[11];

    uint8_t* result = lower_bound_bytes( b,
                                         e,
                                         (uint8_t*)&target,
                                         sizeof(int),
                                         []( const uint8_t* p1, const uint8_t* p2 )->int {
                                             const int* pi1 = (const int*)p1;
                                             const int* pi2 = (const int*)p2;
                                             if( *(int*)p1 < *(int*)p2 )
                                                 return -1;
                                             else if( *(int*)p1 > *(int*)p2 )
                                                 return 1;
                                             return 0;
                                         });

    UT_ASSERT( result != e );
    UT_ASSERT( *(int*)result == 80 );
}

void ck_lower_bound_test::test_middle_exact()
{
    int target = 80;

    uint8_t* b = (uint8_t*)&_storage[0];
    uint8_t* e = (uint8_t*)&_storage[11];

    uint8_t* result = lower_bound_bytes( b,
                                         e,
                                         (uint8_t*)&target,
                                         sizeof(int),
                                         []( const uint8_t* p1, const uint8_t* p2 )->int {
                                             const int* pi1 = (const int*)p1;
                                             const int* pi2 = (const int*)p2;
                                             if( *(int*)p1 < *(int*)p2 )
                                                 return -1;
                                             else if( *(int*)p1 > *(int*)p2 )
                                                 return 1;
                                             return 0;
                                         });

    UT_ASSERT( result != e );
    UT_ASSERT( *(int*)result == 80 );
}

void ck_lower_bound_test::test_greater()
{
    int target = 121;

    uint8_t* b = (uint8_t*)&_storage[0];
    uint8_t* e = (uint8_t*)&_storage[11];

    uint8_t* result = lower_bound_bytes( b,
                                         e,
                                         (uint8_t*)&target,
                                         sizeof(int),
                                         []( const uint8_t* p1, const uint8_t* p2 )->int {
                                             const int* pi1 = (const int*)p1;
                                             const int* pi2 = (const int*)p2;
                                             if( *(int*)p1 < *(int*)p2 )
                                                 return -1;
                                             else if( *(int*)p1 > *(int*)p2 )
                                                 return 1;
                                             return 0;
                                         });

    UT_ASSERT( result == e );
}
