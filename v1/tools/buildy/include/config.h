
#ifndef __Config_h
#define __Config_h

#include "cppkit/ck_string.h"
#include "cppkit/ck_memory.h"
#include "cppkit/ck_json.h"
#include <list>
#include <vector>
#include <memory>
#include <map>

struct component
{
    cppkit::ck_string name;
    cppkit::ck_string src;
    cppkit::ck_string path;
    cppkit::ck_string rev;
    cppkit::ck_string branch;
    cppkit::ck_string cleanbuild;
    cppkit::ck_string cleanbuildContents;
    cppkit::ck_string cleantest;
    std::list<cppkit::ck_string> tags;
};

class config
{
public:
    config( const cppkit::ck_string& configDir, const cppkit::ck_string& configFileName = "" );
    virtual ~config() throw();

    cppkit::ck_string get_config_dir() const;

    size_t get_num_components();
    struct component get_component( size_t index );

    std::list<struct component> get_all_components();
    std::list<struct component> get_components_by_tag( const cppkit::ck_string& key );
    std::list<struct component> get_matching_components( const cppkit::ck_string& arg );

    void set_all_components( const std::list<struct component>& components );

    void write( const cppkit::ck_string& path );

private:
    struct component _create_component( std::shared_ptr<cppkit::ck_json_item> bc );

    cppkit::ck_string _configDir;
    cppkit::ck_string _configPath;
    cppkit::ck_string _localConfigPath;
    std::vector<struct component> _components;
    std::map<std::string, std::shared_ptr<std::list<struct component> > > _tagMembers;
};

#endif
