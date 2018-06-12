
#include "framework.h"
#include "ck_process_test.h"
#include "cppkit/ck_process.h"
#include "cppkit/os/ck_time_utils.h"
#include <future>

using namespace std;
using namespace cppkit;

REGISTER_TEST_FIXTURE(ck_process_test);

// sleepy.exe 2000 3 "hello"
char* sleepyCodeLin = (char*)"#include <unistd.h>\n"
                             "#include <stdio.h>\n"
                             "#include <stdlib.h>\n"
                             "int main( int argc, char* argv[] )\n"
                             "{\n"
                             "    if(argc>3) { printf(\"%s\\n\",argv[3]); }\n"
                             "    int millis = atoi(argv[1]);\n"
                             "    sleep(millis / 1000);\n"
                             "    return (argc>2)?atoi(argv[2]):0;\n"
                             "}\n";

char* sleepyCodeWin = (char*)"#include <Windows.h>\n"
                             "#include <stdio.h>\n"
                             "#include <stdlib.h>\n"
                             "int main( int argc, char* argv[] )\n"
                             "{\n"
                             "    if(argc>3) { printf(\"%s\\n\",argv[3]); }\n"
                             "    int millis = atoi(argv[1]);\n"
                             "    Sleep(millis);\n"
                             "    return (argc>2)?atoi(argv[2]):0;\n"
                             "}\n";

void ck_process_test::setup()
{
    FILE* outFile = fopen( "sleepy.cpp", "w+b" );
#ifdef IS_LINUX
    fwrite( sleepyCodeLin, 1, strlen( sleepyCodeLin ), outFile );
#else
    fwrite( sleepyCodeWin, 1, strlen( sleepyCodeWin ), outFile );
#endif
    fclose( outFile );

#ifdef IS_LINUX
    system( "g++ -o sleepy.exe sleepy.cpp" );
#else
    system( "cl /EHsc sleepy.cpp" );
#endif
}

void ck_process_test::teardown()
{
//    remove( "sleepy.cpp" );
//    remove( "sleepy.exe" );
}

void ck_process_test::test_basic_process()
{
    {
        ck_process p( "sleepy.exe 2000 0" );
        p.start();
    }

    ck_usleep(250000);
    // putting the above process in a scope causes to to be destroyed before we get here... and destroying a process
    // involves waiting until it terminates... so at this point, we shouldn't have any children running.

    list<ck_pid> children = ck_process::get_child_processes( ck_process::get_current_pid() );
    UT_ASSERT( children.size() == 0 );
}

void ck_process_test::test_enumerate_child_processes()
{
    list<ck_pid> children = ck_process::get_child_processes( ck_process::get_current_pid() );
    UT_ASSERT( children.size() == 0 );

    ck_process p1( "sleepy.exe 2000 0" );
    p1.start();

    ck_process p2( "sleepy.exe 2000 0" );
    p2.start();

    children = ck_process::get_child_processes( ck_process::get_current_pid() );
    UT_ASSERT( children.size() == 2 );
}

void ck_process_test::test_get_processes_for_module()
{
    ck_process p1( "sleepy.exe 2000 0" );
    p1.start();

    ck_process p2( "sleepy.exe 2000 0" );
    p2.start();

    ck_usleep( 500000 );

    list<ck_pid> matchingProcesses = ck_process::get_processes_for_module( "sleepy.exe" );
    UT_ASSERT( matchingProcesses.size() == 2 );

    ck_pid first = matchingProcesses.front();
    matchingProcesses.pop_front();

    ck_pid second = matchingProcesses.front();
    matchingProcesses.pop_front();

    UT_ASSERT( first == p1.get_pid() || first == p2.get_pid() );
    UT_ASSERT( second == p1.get_pid() || second == p2.get_pid() );
}

void ck_process_test::test_wait_fock_time_utilsout()
{
    ck_process p1( "sleepy.exe 2000 0" );
    p1.start();
    int status = 0;
    auto res = p1.wait_for( status, chrono::milliseconds(1000) );
    UT_ASSERT( res == CK_PROCESS_WAIT_TIMEDOUT );
    // status is unreliable here because we timedout
}

void ck_process_test::test_wait_for_exited()
{
    ck_process p1( "sleepy.exe 1000 42" );
    p1.start();
    int status = 0;
    auto res = p1.wait_for( status, chrono::milliseconds(2000) );
    UT_ASSERT( res == CK_PROCESS_EXITED );
    UT_ASSERT( status == 42 );
}

void ck_process_test::test_kill()
{
    ck_process p1( "sleepy.exe 5000 0" );
    p1.start();
    int status = 0;
    auto res = p1.wait_for( status, chrono::milliseconds(1000) );
    UT_ASSERT( res == CK_PROCESS_WAIT_TIMEDOUT );
    p1.kill();
    res = p1.wait_for( status, chrono::milliseconds(1000) );
    UT_ASSERT( res == CK_PROCESS_EXITED );
}

void ck_process_test::test_read_stdout()
{
    ck_process p1( "sleepy.exe 1000 0 This_is_a_test!" );
    p1.start();

    char buf[1024];
    memset(buf, 0, 1024);

    ck_string msg;
    while(!p1.stdout_eof())
    {
        p1.stdout_read(buf, 1, 1024);
        msg += buf;
    }

    UT_ASSERT(msg.contains("This_is_a_test!"));
}
