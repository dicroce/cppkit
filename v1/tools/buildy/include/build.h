
#ifndef __buildy_Build_h
#define __buildy_Build_h

#include <memory>
#include "cppkit/ck_string.h"
#include "config.h"

void super_clean( std::shared_ptr<config> cfg, const cppkit::ck_string& tag );

void clean_build( std::shared_ptr<config> cfg, const cppkit::ck_string& tag, bool release = false, bool removeDevelArtifacts = false );

#endif
