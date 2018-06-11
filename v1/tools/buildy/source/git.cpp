
#include "cppkit/ck_types.h"
#include "cppkit/ck_path.h"

#include "git.h"
#include "utils.h"

using namespace cppkit;
using namespace std;

static bool _uncommitted_or_unstashed_changes( shared_ptr<config> cfg, const ck_string& tag = "" )
{
    bool result = false;

    list<struct component> components=(tag.length()>0) ? cfg->get_matching_components(tag) : cfg->get_all_components();

    list<struct component>::iterator i;
    for( i = components.begin(); i != components.end(); i++ )
    {
        if( i->src.length() > 0 )
        {
            ck_string dir = get_directory_from_url( i->src );

            ck_string output = exec_and_get_output( ck_string::format( "git -C %s status --short", dir.c_str() ) );

            if( output.length() > 0 )
            {
                printf( "Found uncommited or unstashed changes in %s.\n", i->name.c_str() );
                fflush( stdout );

                result = true;
                break;
            }
        }
    }

    return result;
}

void git_clone( shared_ptr<config> cfg, const ck_string& tag, bool excludeBranch, bool excludeRev )
{
    list<struct component> components=(tag.length()>0) ? cfg->get_matching_components(tag) : cfg->get_all_components();

    int err = 0;
    list<struct component>::iterator i;
    for( i = components.begin(); i != components.end(); i++ )
    {
        if( i->src.length() > 0 )
        {
            ck_string dir = get_directory_from_url( i->src );

            if( !ck_path::exists( dir ) )
            {
                printf("cloning component: %s\n",i->name.c_str());
                fflush(stdout);

                err = system( ck_string::format( "git clone %s %s", i->src.c_str(), dir.c_str() ).c_str() );
                if( err < 0 )
                    CK_THROW(("Unable to execute git command."));

                if( !excludeBranch )
                {
                    // only do branch checkouts if revision is not set...
                    if( i->rev.length() == 0 )
                    {
                        if( i->branch.length() > 0 )
                        {
                            err = system( ck_string::format( "git -C %s checkout %s",
                                                           dir.c_str(),
                                                           i->branch.c_str() ).c_str() );
                            if( err < 0 )
                                CK_THROW(("Unable to execute git command."));
                        }
                    }
                }

                if( !excludeRev )
                {
                    if( i->rev.length() > 0 )
                    {
                        err = system( ck_string::format( "git -C %s checkout %s",
                                                       dir.c_str(),
                                                       i->rev.c_str() ).c_str() );
                        if( err < 0 )
                            CK_THROW(("Unable to execute git command."));
                    }
                }
            }
            else
            {
                printf("component exits: %s\n",i->name.c_str());
                fflush(stdout);
            }
        }
    }
}

void git_pull( shared_ptr<config> cfg, const ck_string& tag )
{
    list<struct component> components=(tag.length()>0) ? cfg->get_matching_components(tag) : cfg->get_all_components();

    int err = 0;
    list<struct component>::iterator i;
    for( i = components.begin(); i != components.end(); i++ )
    {
        if( i->path.length() )
        {
            ck_string dir = i->path;

            printf("pulling: %s\n",dir.c_str());
            fflush(stdout);

            err = system( ck_string::format( "git -C %s pull --rebase", dir.c_str() ).c_str() );
            if( err < 0 )
                CK_THROW(("Unable to execute git command."));
        }
        else if( i->src.length() )
        {
            ck_string dir = get_directory_from_url( i->src );

            printf("pulling: %s\n",dir.c_str());
            fflush(stdout);

            err = system( ck_string::format( "git -C %s pull --rebase", dir.c_str() ).c_str() );
            if( err < 0 )
                CK_THROW(("Unable to execute git command."));
        }
    }
}

void git_status( shared_ptr<config> cfg, const ck_string& tag, const ck_string& configDir )
{
    list<struct component> components=(tag.length()>0) ? cfg->get_matching_components(tag) : cfg->get_all_components();

    int err = 0;

    ck_string dir = configDir;

    ck_string cmdOutput = exec_and_get_output( ck_string::format( "git -C %s rev-parse --abbrev-ref HEAD", dir.c_str() ) );
    if( cmdOutput.ends_with( '\n' ) )
        cmdOutput = cmdOutput.substr( 0, cmdOutput.length() - 1 );

    printf("status: %s [%s]\n",dir.c_str(),cmdOutput.c_str());
    fflush(stdout);

    err = system( ck_string::format( "git -C %s status --short", dir.c_str() ).c_str() );
    if( err < 0 )
        CK_THROW(("Unable to execute git command."));

    list<struct component>::iterator i;
    for( i = components.begin(); i != components.end(); i++ )
    {
        if( i->src.length() > 0 )
        {
            ck_string dir;

            if( i->path.empty() )
                dir = get_directory_from_url( i->src );
            else dir = i->path;

            cmdOutput = exec_and_get_output( ck_string::format( "git -C %s rev-parse --abbrev-ref HEAD", dir.c_str() ) );
            if( cmdOutput.ends_with( '\n' ) )
                cmdOutput = cmdOutput.substr( 0, cmdOutput.length() - 1 );

            printf("status: %s [%s]\n",i->path.c_str(),cmdOutput.c_str());
            fflush(stdout);

            err = system( ck_string::format( "git -C %s status --short", dir.c_str() ).c_str() );
            if( err < 0 )
                CK_THROW(("Unable to execute git command."));
        }
    }
}

void git_checkout( shared_ptr<config> cfg, const ck_string& tag, const ck_string& configDir, const ck_string& branchName )
{
    list<struct component> components=(tag.length()>0) ? cfg->get_matching_components(tag) : cfg->get_all_components();

    int err = 0;

    ck_string dir = configDir;

    ck_string cmd;
#ifdef IS_WINDOWS
    cmd = ck_string::format( "git -C %s checkout %s 2> nul", dir.c_str(), branchName.c_str() );
#else
    cmd = ck_string::format( "git -C %s checkout %s &> /dev/null", dir.c_str(), branchName.c_str() );
#endif

    ck_string cmdOutput = exec_and_get_output( cmd );

    list<struct component>::iterator i;
    for( i = components.begin(); i != components.end(); i++ )
    {
        if( i->src.length() > 0 )
        {
            ck_string dir = get_directory_from_url( i->src );

#ifdef IS_WINDOWS
            cmd = ck_string::format( "git -C %s checkout %s 2> nul", dir.c_str(), branchName.c_str() );
#else
            cmd = ck_string::format( "git -C %s checkout %s &> /dev/null", dir.c_str(), branchName.c_str() );
#endif

            cmdOutput = exec_and_get_output( cmd );
        }
    }
}

void buildy_snapshot( shared_ptr<config> cfg, const ck_string& outputFilePath )
{
    if( _uncommitted_or_unstashed_changes( cfg ) )
        CK_THROW(("Cannot snapshot with uncommited or unstashed changes."));

    ck_string configDir = cfg->get_config_dir();

    list<struct component> components = cfg->get_all_components();

    int err = 0;
    list<struct component>::iterator i;
    for( i = components.begin(); i != components.end(); i++ )
    {
        if( i->src.length() > 0 )
        {
            ck_string dir = get_directory_from_url( i->src );

            i->branch = exec_and_get_output(ck_string::format( "git -C %s rev-parse --abbrev-ref HEAD", dir.c_str() )).strip();
            i->rev = exec_and_get_output(ck_string::format( "git -C %s rev-parse HEAD", dir.c_str() )).strip();

            ck_string cleanbuildFileName =
                ck_string::format( "%s%s%s", configDir.c_str(), PATH_SLASH, i->cleanbuild.c_str() );

            i->cleanbuildContents = read_file_as_string( cleanbuildFileName );
            i->cleanbuild = "";
        }
    }

    cfg->set_all_components( components );

    cfg->write( outputFilePath );
}
