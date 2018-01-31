
#include "ck_file_lock_test.h"
#include "cppkit/ck_file_lock.h"
#include "cppkit/os/ck_time_utils.h"
#include <thread>

using namespace std;
using namespace cppkit;

REGISTER_TEST_FIXTURE(ck_file_lock_test);

#ifdef WIN32
#include <direct.h>
#define RMDIR _rmdir
#define UNLINK _unlink
#define MKDIR(a) _mkdir(a)
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define RMDIR rmdir
#define UNLINK unlink
#define MKDIR(a) mkdir(a,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
#endif

void ck_file_lock_test::setup()
{
    lockFile = fopen( "lockfile", "w+b" );
}

void ck_file_lock_test::teardown()
{
    fclose( lockFile );

    UNLINK( "lockfile" );
}

void ck_file_lock_test::test_constructor()
{
    ck_file_lock fileLock( fileno( lockFile ) );
}

void ck_file_lock_test::test_exclusive()
{
    FILE* otherFile = fopen( "lockfile", "w+b" );

    int state = 42;

    ck_file_lock fileLock( fileno( lockFile ) );

    {
        ck_file_lock_guard g( fileLock );

        thread t([&](){
                ck_file_lock newLock( fileno( otherFile ) );
                ck_file_lock_guard g( newLock );
                state = 43;
            });
        t.detach();

        ck_usleep( 250000 );

        UT_ASSERT( state == 42 );
    }

    ck_usleep( 100000 );

    UT_ASSERT( state == 43 );

    fclose( otherFile );
}

void ck_file_lock_test::test_shared()
{
    FILE* otherFileA = fopen( "lockfile", "w+b" );
    FILE* otherFileB = fopen( "lockfile", "w+b" );

    int state = 42;  // initially state is 42,

    // But then we fire up two threads with a shared lock

    thread t1([&](){
            ck_file_lock newLock( fileno( otherFileA ) );
            ck_file_lock_guard g( newLock, false );
            ++state;
            ck_usleep( 500000 );
        });
    t1.detach();

    thread t2([&](){
            ck_file_lock newLock( fileno( otherFileB ) );
            ck_file_lock_guard g( newLock, false );
            ++state;
            ck_usleep( 500000 );
        });
    t2.detach();

    ck_usleep( 250000 ); // give the above threads a chance to start...

    {
        ck_file_lock newLock( fileno( lockFile ) );
        ck_file_lock_guard g( newLock );
        // since the above shared locks must be let go before an exclusive can be acquired, then we know at this point
        // state should be 44.
        UT_ASSERT( state == 44 );
    }

    fclose( otherFileB );
    fclose( otherFileA );
}
