
#include "test_ck_file.h"
#include "cppkit/ck_file.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

using namespace std;
using namespace cppkit;

REGISTER_TEST_FIXTURE(test_ck_file);

void test_ck_file::setup()
{
    mkdir( "foo", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
    FILE* regularFile = fopen("foo/bar","w");
    mkdir( "foobar", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );

    mkdir("上开花", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
    FILE* unicodeFile = fopen("上开花/Чернобыльской","w");
    mkdir("走上", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );

    unsigned char buf[128];
    memset( buf, 0, 128 );
    ck_fs::write_file(buf, 128, regularFile);
    ck_fs::write_file(buf, 128, unicodeFile);
    fclose( regularFile );
    fclose( unicodeFile );
}

void test_ck_file::teardown()
{
    remove("foo.txt");

    unlink( "foo/bar" );
    unlink("上开花/Чернобыльской");
    rmdir("上开花");
    rmdir("走上");
    rmdir( "foo" );
    rmdir( "foobar" );
}

void test_ck_file::test_conversion_op()
{
    {
        auto f = ck_file::open("foo.txt", "w+");
        //can use libc file calls on f
        fprintf(f, "Hello %s!\n", "World");
    }

    {
        auto fb = ck_fs::read_file("foo.txt");
        string s((char*)&fb[0], fb.size());
        RTF_ASSERT(ck_string_utils::contains(s, "Hello World!"));
    }
}

void test_ck_file::test_stat()
{
    {
        auto f = ck_file::open("foo.txt", "w+");
        //can use libc file calls on f
        fprintf(f, "Hello %s!\n", "World");
    }

    {
        ck_fs::ck_file_info rfi;
        ck_fs::stat("foo.txt", &rfi);
        RTF_ASSERT(rfi.file_size == 13);
    }
}

void test_ck_file::test_path_ctor()
{
    RTF_ASSERT_NO_THROW(ck_path path( "foo" ));
}

void test_ck_file::test_open_path()
{
    {
        ck_path path("foo");

        string fileName;
        bool ok = path.read_dir(fileName);

        RTF_ASSERT(ok == true);
        RTF_ASSERT(fileName == "bar");
    }
}

void test_ck_file::test_path_iter()
{
    {
        ck_path path( "foo" );

        string fileName;
        int i = 0;
        for( ; path.read_dir( fileName ); ++i );

        RTF_ASSERT( i == 1 );
    }

    {
        ck_path path( "foo" );

        string fileName;
        path.read_dir( fileName );

        RTF_ASSERT( fileName == "bar" );
    }
}

void test_ck_file::test_file_size()
{
    mkdir( "fs_root", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir( ("fs_root" + ck_fs::PATH_SLASH + "a").c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir( ("fs_root" + ck_fs::PATH_SLASH + "b").c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir( ("fs_root" + ck_fs::PATH_SLASH + "c").c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir( ("fs_root" + ck_fs::PATH_SLASH + "d").c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    FILE* f = fopen( ("fs_root" + ck_fs::PATH_SLASH + "reg").c_str(), "wb" );
    uint8_t buf[1024];
    memset(buf, 0, sizeof(buf));
    ck_fs::write_file(&buf[0], 1024, f);
    fclose( f );

    f = fopen( ("fs_root" + ck_fs::PATH_SLASH + "a" + ck_fs::PATH_SLASH + "reg").c_str(), "wb" );
    ck_fs::write_file(&buf[0], 1024, f);
    fclose( f );

    RTF_ASSERT( ck_fs::file_size( "fs_root" ) == 2048 );

    unlink( ("fs_root" + ck_fs::PATH_SLASH + "a" + ck_fs::PATH_SLASH + "reg").c_str() );
    unlink( ("fs_root" + ck_fs::PATH_SLASH + "reg").c_str() );

    rmdir( ("fs_root" + ck_fs::PATH_SLASH + "a").c_str() );
    rmdir( ("fs_root" + ck_fs::PATH_SLASH + "b").c_str() );
    rmdir( ("fs_root" + ck_fs::PATH_SLASH + "c").c_str() );
    rmdir( ("fs_root" + ck_fs::PATH_SLASH + "d").c_str() );
    rmdir( "fs_root" );
}
