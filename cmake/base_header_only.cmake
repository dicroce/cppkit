
message("devel_artifacts path: ${devel_artifacts_path}")

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug)
ENDIF(NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_FLAGS_DEBUG "")
set(CMAKE_CXX_FLAGS_RELEASE "")

set(CMAKE_INSTALL_PREFIX ${devel_artifacts_path})
include_directories(include "${devel_artifacts_path}/include")
link_directories("${devel_artifacts_path}/lib")

# Define our installation rules
#

install(DIRECTORY include/${PROJECT_NAME} DESTINATION include USE_SOURCE_PERMISSIONS)
