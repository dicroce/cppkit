cmake_minimum_required (VERSION 2.8.12)

if(NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Debug)
endif()

set (CMAKE_CXX_STANDARD 14)

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
                    source/ck_memory_map.cpp)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    target_compile_definitions(cppkit PUBLIC IS_DARWIN)
else (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    target_compile_definitions(cppkit PUBLIC IS_LINUX)
endif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(cppkit PUBLIC "-O0")
    target_compile_options(cppkit PUBLIC "-g")
else(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(cppkit PUBLIC "-O3")
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

target_include_directories (cppkit PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_library(SSL_LIBRARY ssl)
find_library(CRYPTO_LIBRARY crypto)
find_library(DL_LIBRARY dl)
find_library(PTHREAD_LIBRARY pthread)
find_library(UUID_LIBRARY uuid)

target_link_libraries (cppkit ${PTHREAD_LIBRARY} ${SSL_LIBRARY} ${CRYPTO_LIBRARY} ${DL_LIBRARY} ${UUID_LIBRARY})

add_subdirectory(ut)
