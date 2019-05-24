
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
    auto scratchPath = RTF_OS_SCRATCH_DIR();
    mkdir( (scratchPath + ck_fs::PATH_SLASH + "foo").c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
    FILE* regularFile = fopen((scratchPath + ck_fs::PATH_SLASH + "foo/bar").c_str(),"w");
    mkdir( (scratchPath + ck_fs::PATH_SLASH + "foobar").c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );

    mkdir((scratchPath + ck_fs::PATH_SLASH + "上开花").c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
    FILE* unicodeFile = fopen((scratchPath + ck_fs::PATH_SLASH + "上开花/Чернобыльской").c_str(),"w");
    mkdir((scratchPath + ck_fs::PATH_SLASH + "走上").c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );

    unsigned char buf[128];
    memset( buf, 0, 128 );
    ck_fs::block_write_file(buf, 128, regularFile);
    ck_fs::block_write_file(buf, 128, unicodeFile);
    fclose( regularFile );
    fclose( unicodeFile );
}

void test_ck_file::teardown()
{
    auto scratchPath = RTF_OS_SCRATCH_DIR();
    remove((scratchPath + ck_fs::PATH_SLASH + "foo.txt").c_str());

    unlink((scratchPath + ck_fs::PATH_SLASH + "foo/bar").c_str());
    unlink((scratchPath + ck_fs::PATH_SLASH + "上开花/Чернобыльской").c_str());
    rmdir((scratchPath + ck_fs::PATH_SLASH + "上开花").c_str());
    rmdir((scratchPath + ck_fs::PATH_SLASH + "走上").c_str());
    rmdir((scratchPath + ck_fs::PATH_SLASH + "foo").c_str());
    rmdir((scratchPath + ck_fs::PATH_SLASH + "foobar").c_str());
}

void test_ck_file::test_conversion_op()
{
    auto scratchPath = RTF_OS_SCRATCH_DIR();

    {
        auto f = ck_file::open((scratchPath + ck_fs::PATH_SLASH + "foo.txt").c_str(), "w+");
        //can use libc file calls on f
        fprintf(f, "Hello %s!\n", "World");
    }

    {
        auto fb = ck_fs::read_file((scratchPath + ck_fs::PATH_SLASH + "foo.txt").c_str());
        string s((char*)&fb[0], fb.size());
        RTF_ASSERT(ck_string_utils::contains(s, "Hello World!"));
    }
}

void test_ck_file::test_stat()
{
    auto scratchPath = RTF_OS_SCRATCH_DIR();

    {
        auto f = ck_file::open((scratchPath + ck_fs::PATH_SLASH + "foo.txt").c_str(), "w+");
        //can use libc file calls on f
        fprintf(f, "Hello %s!\n", "World");
    }

    {
        ck_fs::ck_file_info rfi;
        ck_fs::stat((scratchPath + ck_fs::PATH_SLASH + "foo.txt").c_str(), &rfi);
        RTF_ASSERT(rfi.file_size == 13);
    }
}

void test_ck_file::test_path_ctor()
{
    auto scratchPath = RTF_OS_SCRATCH_DIR();

    RTF_ASSERT_NO_THROW(ck_path path( scratchPath + ck_fs::PATH_SLASH + "foo" ));
}

void test_ck_file::test_open_path()
{
    auto scratchPath = RTF_OS_SCRATCH_DIR();

    {
        ck_path path(scratchPath + ck_fs::PATH_SLASH + "foo");

        string fileName;
        bool ok = path.read_dir(fileName);

        RTF_ASSERT(ok == true);
        RTF_ASSERT(fileName == "bar");
    }
}

void test_ck_file::test_path_iter()
{
    auto scratchPath = RTF_OS_SCRATCH_DIR();

    {
        ck_path path( scratchPath + ck_fs::PATH_SLASH + "foo" );

        string fileName;
        int i = 0;
        for( ; path.read_dir( fileName ); ++i );

        RTF_ASSERT( i == 1 );
    }

    {
        ck_path path( scratchPath + ck_fs::PATH_SLASH + "foo" );

        string fileName;
        path.read_dir( fileName );

        RTF_ASSERT( fileName == "bar" );
    }
}

void test_ck_file::test_file_size()
{
    auto scratchPath = RTF_OS_SCRATCH_DIR();

    mkdir( (scratchPath + ck_fs::PATH_SLASH + "fs_root").c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir( (scratchPath + ck_fs::PATH_SLASH + "fs_root" + ck_fs::PATH_SLASH + "a").c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir( (scratchPath + ck_fs::PATH_SLASH + "fs_root" + ck_fs::PATH_SLASH + "b").c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir( (scratchPath + ck_fs::PATH_SLASH + "fs_root" + ck_fs::PATH_SLASH + "c").c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir( (scratchPath + ck_fs::PATH_SLASH + "fs_root" + ck_fs::PATH_SLASH + "d").c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    FILE* f = fopen( (scratchPath + ck_fs::PATH_SLASH + "fs_root" + ck_fs::PATH_SLASH + "reg").c_str(), "wb" );
    uint8_t buf[1024];
    memset(buf, 0, sizeof(buf));
    ck_fs::block_write_file(&buf[0], 1024, f);
    fclose( f );

    f = fopen( (scratchPath + ck_fs::PATH_SLASH + "fs_root" + ck_fs::PATH_SLASH + "a" + ck_fs::PATH_SLASH + "reg").c_str(), "wb" );
    ck_fs::block_write_file(&buf[0], 1024, f);
    fclose( f );

    RTF_ASSERT( ck_fs::file_size( scratchPath + ck_fs::PATH_SLASH + "fs_root" ) == 2048 );

    unlink( (scratchPath + ck_fs::PATH_SLASH + "fs_root" + ck_fs::PATH_SLASH + "a" + ck_fs::PATH_SLASH + "reg").c_str() );
    unlink( (scratchPath + ck_fs::PATH_SLASH + "fs_root" + ck_fs::PATH_SLASH + "reg").c_str() );

    rmdir( (scratchPath + ck_fs::PATH_SLASH + "fs_root" + ck_fs::PATH_SLASH + "a").c_str() );
    rmdir( (scratchPath + ck_fs::PATH_SLASH + "fs_root" + ck_fs::PATH_SLASH + "b").c_str() );
    rmdir( (scratchPath + ck_fs::PATH_SLASH + "fs_root" + ck_fs::PATH_SLASH + "c").c_str() );
    rmdir( (scratchPath + ck_fs::PATH_SLASH + "fs_root" + ck_fs::PATH_SLASH + "d").c_str() );
    rmdir( (scratchPath + ck_fs::PATH_SLASH + "fs_root").c_str() );
}

void test_ck_file::test_break_path()
{
    auto scratchPath = RTF_OS_SCRATCH_DIR();

    {
        string path, fileName;
        ck_fs::break_path(scratchPath + ck_fs::PATH_SLASH + "/foo/bar/baz", path, fileName);
        RTF_ASSERT(path == scratchPath + ck_fs::PATH_SLASH + "/foo/bar");
        RTF_ASSERT(fileName == "baz");
    }
    {
        string path, fileName;
        ck_fs::break_path(scratchPath + ck_fs::PATH_SLASH + "filename", path, fileName);
        RTF_ASSERT(path == string(scratchPath));
        RTF_ASSERT(fileName == "filename");
    }

}