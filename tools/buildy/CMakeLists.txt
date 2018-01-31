cmake_minimum_required(VERSION 2.8)
project(buildy)

include(common.cmake NO_POLICY_SCOPE)
include(packaging.cmake NO_POLICY_SCOPE)

set(SOURCES source/main.cpp
            source/config.cpp
            source/option.cpp
            source/git.cpp
            source/utils.cpp
            source/build.cpp
            source/test.cpp)

set(COMMON_LIBS cppkit)

if(CMAKE_SYSTEM MATCHES "Windows")
#    set(RC_FILE "buildy.rc")
    set(APPLICATION_TYPE "WINDOWS_CONSOLE_STATIC")
elseif(CMAKE_SYSTEM MATCHES "Linux")
    set(APPLICATION_TYPE "NORMAL_STATIC")
endif(CMAKE_SYSTEM MATCHES "Windows")

include("${devel_artifacts_path}/build/base_app.cmake" NO_POLICY_SCOPE)

# Installation...

install(TARGETS buildy DESTINATION buildy)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build_scripts DESTINATION buildy)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/example_config DESTINATION buildy)

if(CMAKE_SYSTEM MATCHES "Windows")
    install(FILES ${devel_artifacts_path}/lib/cppkit.dll DESTINATION buildy)
elseif(CMAKE_SYSTEM MATCHES "Linux")
    install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/utils/ssh_cmd DESTINATION buildy)
    install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/utils/scp_cmd DESTINATION buildy)
endif()
