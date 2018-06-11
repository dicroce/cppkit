
#include "test.h"
#include "cppkit/ck_path.h"

using namespace cppkit;
using namespace std;

void test( shared_ptr<config> cfg, const cppkit::ck_string& tag )
{
    ck_string configDir = cfg->get_config_dir();

    list<struct component> components=(tag.length()>0) ? cfg->get_matching_components(tag) : cfg->get_all_components();

    int err = 0;
    list<struct component>::iterator i;
    for( i = components.begin(); i != components.end(); i++ )
    {
        if( i->cleantest.length() > 0 )
        {
            err = system( ck_string::format( "\"%s%s%s\" %s %s %s",
                                           configDir.c_str(),
                                           PATH_SLASH,
                                           i->cleantest.c_str(),
                                           i->name.c_str(),
                                           (i->src.length() > 0) ? i->src.c_str() : "NO_SOURCE",
                                           i->path.c_str() ).c_str() );
            if( err != 0 )
                CK_THROW(("Test failure."));
        }
    }
}
