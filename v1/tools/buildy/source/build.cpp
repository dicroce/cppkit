
#include "build.h"
#include "utils.h"
#include "cppkit/ck_path.h"

using namespace cppkit;
using namespace std;

void super_clean( shared_ptr<config> cfg, const cppkit::ck_string& tag )
{
    int err = 0;

    printf("Removing devel_artifacts...");
    fflush(stdout);

    err = system( "rm -rf devel_artifacts" );
    if( err < 0 )
        CK_THROW(("Unable to remove devel_artifacts."));

    ck_string configDir = cfg->get_config_dir();

    list<struct component> components=(tag.length()>0) ? cfg->get_matching_components(tag) : cfg->get_all_components();

    list<struct component>::iterator i;
    for( i = components.begin(); i != components.end(); i++ )
    {
        if( i->cleanbuild.length() > 0 )
        {
            printf("%s\n",i->cleanbuild.c_str());
            fflush(stdout);

            err = system( ck_string::format( "\"%s%s%s\" %s %s %s %s",
                                           configDir.c_str(),
                                           PATH_SLASH,
                                           i->cleanbuild.c_str(),
                                           i->name.c_str(),
                                           (i->src.length() > 0) ? i->src.c_str() : "NO_SOURCE",
                                           i->path.c_str(),
                                           "CLEAN" ).c_str() );
            if( err != 0 )
                CK_THROW(("Build command failure."));
        }
        else if( i->cleanbuildContents.length() > 0 )
        {
#ifdef IS_WINDOWS
            ck_string fileName = ".\\embedded_build.bat";
#else
            ck_string fileName = "./embedded_build.sh";
#endif
            if( ck_path::exists( fileName ) )
                remove( fileName.c_str() );

            write_file_contents( fileName, i->cleanbuildContents );

#ifdef IS_POSIX
            err = system( ck_string::format( "chmod +x %s",
                                           fileName.c_str() ).c_str() );
            if( err < 0 )
                CK_THROW(("Build command failure."));
#endif

            err = system( ck_string::format( "%s %s %s %s %s",
                                           fileName.c_str(),
                                           i->name.c_str(),
                                           (i->src.length() > 0) ? i->src.c_str() : "NO_SOURCE",
                                           i->path.c_str(),
                                           "CLEAN" ).c_str() );
            if( err != 0 )
                CK_THROW(("Build command failure."));

            if( ck_path::exists( fileName ) )
                remove( fileName.c_str() );
        }
        else CK_THROW(( "No build script file specified and no cleanbuild_contents specified." ));
    }
}

void clean_build( shared_ptr<config> cfg, const cppkit::ck_string& tag, bool release, bool removeDevelArtifacts )
{
    int err = 0;
    if( removeDevelArtifacts )
    {
        printf("Removing devel_artifacts...");
        fflush(stdout);

        err = system( "rm -rf devel_artifacts" );
	if( err < 0 )
	  CK_THROW(("Unable to remove devel_artifacts."));
    }

    ck_string configDir = cfg->get_config_dir();

    list<struct component> components=(tag.length()>0) ? cfg->get_matching_components(tag) : cfg->get_all_components();

    list<struct component>::iterator i;
    for( i = components.begin(); i != components.end(); i++ )
    {
        if( i->cleanbuild.length() > 0 )
        {
            printf("%s\n",i->cleanbuild.c_str());
            fflush(stdout);

            err = system( ck_string::format( "\"%s%s%s\" %s %s %s %s",
                                           configDir.c_str(),
                                           PATH_SLASH,
                                           i->cleanbuild.c_str(),
                                           i->name.c_str(),
                                           (i->src.length() > 0) ? i->src.c_str() : "NO_SOURCE",
                                           i->path.c_str(),
                                           (release)?"RELEASE":"DEBUG" ).c_str() );
            if( err != 0 )
                CK_THROW(("Build command failure."));
        }
        else if( i->cleanbuildContents.length() > 0 )
        {
#ifdef IS_WINDOWS
            ck_string fileName = ".\\embedded_build.bat";
#else
            ck_string fileName = "./embedded_build.sh";
#endif
            if( ck_path::exists( fileName ) )
                remove( fileName.c_str() );

            write_file_contents( fileName, i->cleanbuildContents );

#ifdef IS_POSIX
            err = system( ck_string::format( "chmod +x %s",
                                           fileName.c_str() ).c_str() );
            if( err < 0 )
                CK_THROW(("Build command failure."));
#endif

            err = system( ck_string::format( "%s %s %s %s %s",
                                           fileName.c_str(),
                                           i->name.c_str(),
                                           (i->src.length() > 0) ? i->src.c_str() : "NO_SOURCE",
                                           i->path.c_str(),
                                           (release)?"RELEASE":"DEBUG" ).c_str() );
            if( err != 0 )
                CK_THROW(("Build command failure."));

            if( ck_path::exists( fileName ) )
                remove( fileName.c_str() );
        }
        else CK_THROW(( "No build script file specified and no cleanbuild_contents specified." ));
    }
}
