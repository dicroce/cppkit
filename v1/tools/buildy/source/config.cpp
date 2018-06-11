
#include "config.h"
#include "utils.h"

#include "cppkit/os/ck_large_files.h"
#include "cppkit/ck_path.h"

using namespace cppkit;
using namespace std;

//[
//   {
//        "name": "x264",
//        "src": "https://github.com/MultiSight/x264",
//        "path": "x264",
//        "rev": "f8eb7e276646f473822ba00369660491676b4be4",
//        "branch": "2.0",
//        "tags": [ "3rdparty", "gateway_libs" ]
//    }
//],
//[
//    {
//        "name": "xsdk",
//        "src": "https://github.com/MultiSight/xsdk",
//        "path": "xsdk",
//        "tags": [ "gateway_libs" ]
//    }
//],


config::config( const ck_string& configDir, const ck_string& configFileName ) :
    _configDir( configDir ),
    _configPath( ck_string::format( "%s%s%s", _configDir.c_str(), PATH_SLASH, (configFileName.length()!=0)?configFileName.c_str():"buildy.json" ) ),
    _localConfigPath( "local.json" ),
    _components(),
    _tagMembers()
{
    shared_ptr<ck_json_item> localComponents;

    if( ck_path::exists( _localConfigPath ) )
    {
        shared_ptr<ck_memory> localConfigBuffer = read_file( _localConfigPath );
        ck_string localDoc = ck_string( (char*)localConfigBuffer->map().get_ptr(), localConfigBuffer->size_data() );
        localComponents = ck_json_item::parse_document( localDoc )->index( "components" );
    }

    shared_ptr<ck_memory> buffer = read_file( _configPath );
    ck_string doc = ck_string( (char*)buffer->map().get_ptr(), buffer->size_data() );
    shared_ptr<ck_json_item> components = ck_json_item::parse_document( doc )->index( "components" );

    if (components.get() == NULL)
        CK_THROW(("Found no build components in config"));

    size_t numComponents = components->size();

    size_t numLocalComponents = (localComponents.get()) ? localComponents->size() : 0;

    _components.reserve( numComponents + numLocalComponents );

    for( size_t i = 0; i < numComponents; i++ )
    {
        shared_ptr<ck_json_item> bc = components->index( i );
        _components.push_back( _create_component( bc ) );
    }

    for( size_t i = 0; i < numLocalComponents; i++ )
    {
        shared_ptr<ck_json_item> bc = localComponents->index( i );
        _components.push_back( _create_component( bc ) );
    }
}

config::~config() throw()
{
}

ck_string config::get_config_dir() const
{
    return _configDir;
}

size_t config::get_num_components()
{
    return _components.size();
}

struct component config::get_component( size_t index )
{
    if( index >= _components.size() )
        CK_THROW(("Invalid component index."));

    return _components[index];
}

list<struct component> config::get_all_components()
{
    list<struct component> components;

    vector<struct component>::iterator i;
    for( i = _components.begin(); i != _components.end(); i++ )
        components.push_back( *i );

    return components;
}

list<struct component> config::get_components_by_tag( const ck_string& key )
{
    list<struct component> components;

    if( _tagMembers.find( key ) != _tagMembers.end() )
        components = *(_tagMembers[key]);

    return components;
}

list<struct component> config::get_matching_components( const ck_string& arg )
{
    list<struct component> matched = get_components_by_tag( arg );

    vector<struct component>::iterator i;
    for( i = _components.begin(); i != _components.end(); i++ )
        if( i->name == arg )
            matched.push_back( *i );

    return matched;
}

void config::set_all_components( const list<struct component>& components )
{
    _components.clear();

    list<struct component>::const_iterator i;
    for( i = components.begin(); i != components.end(); i++ )
        _components.push_back( *i );
}

void config::write( const ck_string& path )
{
    ck_string doc;

    doc += "{\n";
    doc += "  \"components\":\n";
    doc += "  [\n";

    vector<struct component>::iterator i;
    for( i = _components.begin(); i != _components.end(); i++ )
    {
        if( i->src.length() > 0 )
        {
            bool lastComponent = (next(i) == _components.end()) ? true : false;

            doc += "    {\n";
            doc += ck_string::format( "      \"name\": \"%s\",\n", i->name.c_str() );
            doc += ck_string::format( "      \"src\": \"%s\",\n", i->src.c_str() );
            doc += ck_string::format( "      \"path\": \"%s\",\n", i->path.c_str() );
            if( i->rev.length() > 0 )
                doc += ck_string::format( "      \"rev\": \"%s\",\n", i->rev.c_str() );
            if( i->branch.length() > 0 )
                doc += ck_string::format( "      \"branch\": \"%s\",\n", i->branch.c_str() );
            if( i->tags.size() > 0 )
            {
                doc += "      \"tags\": [\n";

                list<ck_string>::iterator ii;
                for( ii = i->tags.begin(); ii != i->tags.end(); ii++ )
                {
                    bool lastTag = (next(ii) == i->tags.end()) ? true : false;
                    doc += ck_string::format( "        \"%s\"%s\n", ii->c_str(), (lastTag) ? "" : "," );
                }

                doc += "      ],\n";
            }

            if( !i->cleanbuildContents.length() )
                CK_THROW(("Snapshots require embedded build scripts."));

            doc += ck_string::format( "      \"cleanbuild_contents\": \"%s\"%s\n",
                                      ck_string::to_base64( i->cleanbuildContents.c_str(), i->cleanbuildContents.length() ).c_str(),
                                      (i->cleantest.length() > 0)?",":"" );

            if( i->cleantest.length() > 0 )
                doc += ck_string::format( "      \"cleantest\": \"%s\"\n", i->cleantest.c_str() );

            doc += ck_string::format( "    }%s\n", (lastComponent) ? "" : "," );
        }
    }

    doc += "  ]\n";
    doc += "}\n";

    FILE* outputFile = fopen( path.c_str(), "wb" );
    if( !outputFile )
        CK_THROW(("Unable to open output file."));

    fwrite( doc.c_str(), 1, doc.length(), outputFile );

    fclose( outputFile );
}

struct component config::_create_component( shared_ptr<ck_json_item> bc )
{
    struct component comp;

    comp.name = bc->index( "name" )->get().to_string();
    comp.src = bc->index( "src" )->get().to_string();
    comp.path = bc->index( "path" )->get().to_string();
    comp.rev = (bc->has_index( "rev" )) ? bc->index( "rev" )->get().to_string() : "";
    comp.branch = (bc->has_index( "branch" )) ? bc->index( "branch" )->get().to_string() : "";
    comp.cleanbuild = (bc->has_index( "cleanbuild" )) ? bc->index( "cleanbuild" )->get().to_string() : "";

    if( bc->has_index( "cleanbuild_contents" ) )
    {
        ck_string encodedCleanBuildContents = bc->index( "cleanbuild_contents" )->get().to_string();
        shared_ptr<ck_memory> decoded = encodedCleanBuildContents.from_base64();

        comp.cleanbuildContents = ck_string( (char*)decoded->map().get_ptr(), decoded->size_data() );
    }

    comp.cleantest = (bc->has_index( "cleantest" )) ? bc->index( "cleantest" )->get().to_string() : "";

    if( bc->has_index( "tags" ) )
    {
        for( size_t ii = 0; ii < bc->index( "tags" )->size(); ii++ )
        {
            ck_string tag = bc->index( "tags" )->index( ii )->get().to_string();

            comp.tags.push_back( tag );

            if( _tagMembers.find( tag ) == _tagMembers.end() )
                _tagMembers.insert( make_pair((string)tag, make_shared<list<struct component> >()) );

            _tagMembers[tag]->push_back( comp );
        }
    }

    return comp;
}
