
#include "cppkit/ck_args.h"
#include "cppkit/ck_string_utils.h"

using namespace cppkit;
using namespace std;

vector<args::argument> cppkit::args::parse_arguments(int argc, char* argv[])
{
    vector<args::argument> arguments;

    struct argument currentArgument;

    int argumentIndex = -1;

    for(int i = 0; i < argc; ++i)
    {
        string token = argv[i];

        if(ck_string_utils::starts_with(token, "-"))
        {
            if(argumentIndex != -1)
            {
                currentArgument.opt = argv[argumentIndex];
                arguments.push_back(currentArgument);
                currentArgument.arg = "";
            }

            currentArgument.opt = token;
            argumentIndex = i;
        }
        else
        {
            if(argumentIndex != -1)
            {
                if(currentArgument.arg.length() > 0)
                    currentArgument.arg += string(" ") + token;
                else currentArgument.arg = token;
            }
        }

        if((i + 1) == argc)
        {
            if(argumentIndex != -1)
                arguments.push_back(currentArgument);
        }
    }

    return arguments;
}

bool cppkit::args::check_argument(const vector<args::argument>& arguments, const string& opt, string& arg)
{
    vector<argument>::const_iterator i;
    for(i = arguments.begin(); i != arguments.end(); ++i)
    {
        if(i->opt == opt)
        {
            arg = i->arg;
            return true;
        }
    }

    return false;
}

bool cppkit::args::check_argument(const vector<args::argument>& arguments, const string& opt)
{
    vector<argument>::const_iterator i;
    for(i = arguments.begin(); i != arguments.end(); ++i)
    {
        if(i->opt == opt)
            return true;
    }

    return false;
}
