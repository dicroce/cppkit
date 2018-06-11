
#ifndef __buildy__GIT_h
#define __buildy__GIT_h

#include "cppkit/ck_string.h"
#include "config.h"

void git_clone( std::shared_ptr<config> cfg, const cppkit::ck_string& tag, bool excludeBranch = false, bool excludeRev = false );

void git_pull( std::shared_ptr<config> cfg, const cppkit::ck_string& tag );

void git_status( std::shared_ptr<config> cfg, const cppkit::ck_string& tag, const cppkit::ck_string& configDir );

void git_checkout( std::shared_ptr<config> cfg, const cppkit::ck_string& tag, const cppkit::ck_string& configDir, const cppkit::ck_string& branchName );

void buildy_snapshot( std::shared_ptr<config> cfg, const cppkit::ck_string& outputFilePath );

#endif
