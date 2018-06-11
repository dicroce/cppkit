cmake_minimum_required (VERSION 2.8.12)

if(NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Debug)
endif()

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_FLAGS "-pthread -Wall -Wfatal-errors")
set (CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb")
set (CMAKE_CXX_FLAGS_RELEASE "-O3")

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    add_definitions(-DIS_DARWIN)
else (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    add_definitions(-DIS_LINUX)
endif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")

add_library (cppkit include/cppkit/ck_blob_tree.h
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
                    source/ck_uuid_utils.cpp)

target_include_directories (cppkit PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_library(SSL_LIBRARY ssl)
find_library(CRYPTO_LIBRARY crypto)
find_library(DL_LIBRARY dl)
find_library(PTHREAD_LIBRARY pthread)
find_library(UUID_LIBRARY uuid)

target_link_libraries (cppkit ${PTHREAD_LIBRARY} ${SSL_LIBRARY} ${CRYPTO_LIBRARY} ${DL_LIBRARY} ${UUID_LIBRARY})

#add_executable (test source/main.cpp)
#target_link_libraries(test cppkit)