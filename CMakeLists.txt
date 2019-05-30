cmake_minimum_required (VERSION 2.8.12)

include(FetchContent)

if(NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Debug)
endif()

set (CMAKE_CXX_STANDARD 14)

FetchContent_Declare(
    bzip2
    GIT_REPOSITORY https://github.com/dicroce/bzip2-1.0.6
    GIT_TAG 49f653a10e5104be9e5cb9ac39586499dbec261d
)

FetchContent_GetProperties(bzip2)
if(NOT bzip2_POPULATED)
  FetchContent_Populate(bzip2)
  add_subdirectory(${bzip2_SOURCE_DIR} ${bzip2_BINARY_DIR})
endif()

add_library (cppkit include/cppkit/ck_actor.h
                    include/cppkit/ck_blob_tree.h
                    include/cppkit/ck_exception.h
                    include/cppkit/ck_file.h
                    include/cppkit/ck_logger.h
                    include/cppkit/ck_macro.h
                    include/cppkit/ck_nullable.h
                    include/cppkit/ck_server_threaded.h
                    include/cppkit/ck_sha_256.h
                    include/cppkit/ck_socket_address.h
                    include/cppkit/ck_socket.h
                    include/cppkit/ck_stack_trace.h
                    include/cppkit/ck_string_utils.h
                    include/cppkit/ck_time_utils.h
                    include/cppkit/ck_uuid_utils.h
                    include/cppkit/ck_args.h
                    include/cppkit/ck_memory_map.h
                    include/cppkit/ck_compression_utils.h
                    include/cppkit/3rdparty/fastlz/fastlz.h
                    source/ck_blob_tree.cpp
                    source/ck_exception.cpp
                    source/ck_file.cpp
                    source/ck_logger.cpp
                    source/ck_sha_256.cpp
                    source/ck_socket_address.cpp
                    source/ck_socket.cpp
                    source/ck_stack_trace.cpp
                    source/ck_string_utils.cpp
                    source/ck_time_utils.cpp
                    source/ck_uuid_utils.cpp
                    source/ck_args.cpp
                    source/ck_memory_map.cpp
                    source/ck_compression_utils.cpp
                    source/fastlz.cpp)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    target_compile_definitions(cppkit PUBLIC IS_DARWIN)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
    target_compile_definitions(cppkit PUBLIC IS_IOS)
else()
    target_compile_definitions(cppkit PUBLIC IS_LINUX)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(cppkit PUBLIC "-O0")
    target_compile_options(cppkit PUBLIC "-ggdb")
else(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(cppkit PUBLIC "-O3")
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

target_include_directories (cppkit PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${bzip2_SOURCE_DIR})

if (${CMAKE_SYSTEM_NAME} STREQUAL "iOS" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    target_link_libraries(cppkit "-framework CoreFoundation")
else()
find_library(UUID_LIBRARY libuuid.a)
    target_link_libraries(cppkit ${UUID_LIBRARY})
endif()

target_link_libraries(cppkit bzip2)

add_subdirectory(ut)
