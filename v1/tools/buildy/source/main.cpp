
#include <stdio.h>
#include <stdlib.h>
#include <list>

#include "cppkit/ck_string.h"
#include "cppkit/ck_path.h"

#include "config.h"
#include "option.h"
#include "git.h"
#include "build.h"
#include "test.h"

using namespace cppkit;
using namespace std;

void print_help()
{
    printf("usage: buildy <command> <arg>\n\n");
    printf("commands -\n");
    printf("    --clone - Execute git clone. Applies to provided tag, project name or ALL\n");
    printf("              if no arg provided.\n");
    printf("    --pull - Execute git pull. Applies to provided tag, project name or ALL\n");
    printf("             if no arg provided.\n");
    printf("    --status - Execute git status. Applies to provided tag, project name or\n");
    printf("               ALL if no arg provided.\n");
    printf("    --build - cleanbuild script on each project matching tag, project name or\n");
    printf("              ALL if no arg provided.\n");
    printf("    --snapshot - Create a buildy.json containing revision information for \n");
    printf("                 reproducing a build.\n");
    printf("    --config <config_file_name> - Use provided path to buildy.json instead of\n");
    printf("                                  automatically finding it.\n");
    printf("    --superclean - Deletes devel_artifacts and all components build\n");
    printf("                   directories.\n");
    printf("    --test - Executes unit tests. Applies to provided tag, project name or ALL\n");
    printf("             if no arg provided.\n");
    printf("    --checkout <branch> - Executes git checkout <branch> on all components. If\n");
    printf("                          a component lacks a branch with that name, it will do\n");
    printf("                          nothing.\n");

    exit(0);
}

ck_string find_config_dir()
{
    if( ck_path::exists( "buildy.json" ) )
        return ck_string::format( ".%s", PATH_SLASH );

    ck_path path( "." );

    ck_string entryName;
    while( path.read_dir( entryName ) )
    {
        if( entryName == "." || entryName == ".." )
            continue;

        if( ck_path::is_dir( entryName ) )
        {
            ck_string potentialPath = ck_string::format( "%s%s", entryName.c_str(), PATH_SLASH );
            if( ck_path::exists( potentialPath + "buildy.json" ) )
                return potentialPath;
        }
    }

    return "";
}

int main( int argc, char* argv[] )
{
    try
    {
        list<struct option> options = parse_options( argc, argv );

        ck_string configDir = find_config_dir();
        ck_string configFileName;

        ck_string overrideConfigPath;
        if( check_option( options, "--config", overrideConfigPath ) )
        {
            if( overrideConfigPath.contains( PATH_SLASH ) )
            {
                size_t lastSlash = overrideConfigPath.rfind( PATH_SLASH );
                configDir = overrideConfigPath.substr( 0, lastSlash );
                configFileName = overrideConfigPath.substr( lastSlash + 1 );
            }
            else
            {
                configDir = ".";
                configFileName = overrideConfigPath;
            }
        }

        if( configDir.length() == 0 )
            CK_THROW(("buildy.json not found."));

        shared_ptr<config> cfg = make_shared<config>( configDir, configFileName );

        ck_string arg;
        if( check_option( options, "--clone", arg ) )
        {
            bool excludeBranch = check_option( options, "--exclude-branch" );
            bool excludeRev = check_option( options, "--exclude-rev" );

            git_clone( cfg, arg, excludeBranch, excludeRev );
        }
        else if( check_option( options, "--pull", arg ) )
        {
            git_pull( cfg, arg );
        }
        else if( check_option( options, "--status", arg ) )
        {
            git_status( cfg, arg, configDir );
        }
        else if( check_option( options, "--build", arg ) )
        {
            ck_string arg2;

            clean_build( cfg,
                         arg,
                         check_option( options, "--release", arg2 ),
                         check_option( options, "--superclean", arg2 ) );
        }
        else if( check_option( options, "--snapshot", arg ) )
        {
            if( arg.length() == 0 )
                CK_THROW(("--snapshot requested without providing an output path."));

            buildy_snapshot( cfg, arg );
        }
        else if( check_option( options, "--superclean", arg ) )
        {
            super_clean( cfg, arg );
        }
        else if( check_option( options, "--test", arg ) )
        {
            test( cfg, arg );
        }
        else if( check_option( options, "--checkout", arg ) )
        {
            git_checkout( cfg, "", configDir, arg );
        }
        else print_help();
    }
    catch( std::exception& ex )
    {
        printf("%s\n",ex.what());
        fflush(stdout);
        exit(1);
    }

    return 0;
}
