
#include "option.h"

using namespace cppkit;
using namespace std;

list<struct option> parse_options( int argc, char* argv[] )
{
    list<struct option> options;

    struct option currentOption;

    int optionIndex = -1;

    for( int i = 0; i < argc; i++ )
    {
        ck_string token = argv[i];

        if( token.starts_with( "-" ) )
        {
            if( optionIndex != -1 )
            {
                currentOption.opt = argv[optionIndex];
                options.push_back( currentOption );
                currentOption.arg = "";
            }

            currentOption.opt = token;
            optionIndex = i;
        }
        else
        {
            if( optionIndex != -1 )
            {
                if( currentOption.arg.length() > 0 )
                    currentOption.arg += ck_string(" ") + token;
                else currentOption.arg = token;
            }
        }

        if( (i + 1) == argc )
        {
            if( optionIndex != -1 )
                options.push_back( currentOption );
        }
    }

    return options;
}

bool check_option( const list<struct option>& options, const cppkit::ck_string& opt, cppkit::ck_string& arg )
{
    list<struct option>::const_iterator i;
    for( i = options.begin(); i != options.end(); i++ )
    {
        if( i->opt == opt )
        {
            arg = i->arg;
            return true;
        }
    }

    return false;
}

bool check_option( const list<struct option>& options, const cppkit::ck_string& opt )
{
    list<struct option>::const_iterator i;
    for( i = options.begin(); i != options.end(); i++ )
    {
        if( i->opt == opt )
            return true;
    }

    return false;
}
