cmake_minimum_required (VERSION 2.8.12)

add_executable (cppkit_ut include/framework.h
                          include/test_ck_string_utils.h
                          include/test_ck_sha_256.h
                          include/test_ck_socket.h
                          include/test_ck_server_threaded.h
                          include/test_ck_file.h
                          include/test_ck_pool.h
                          include/test_ck_uuid_utils.h
                          include/test_ck_time_utils.h
                          include/test_ck_blob_tree.h
                          include/test_ck_args.h
                          include/test_ck_memory_map.h
                          include/test_fastlz.h
                          source/framework.cpp
                          source/test_ck_string_utils.cpp
                          source/test_ck_sha_256.cpp
                          source/test_ck_socket.cpp
                          source/test_ck_server_threaded.cpp
                          source/test_ck_file.cpp
                          source/test_ck_pool.cpp
                          source/test_ck_uuid_utils.cpp
                          source/test_ck_time_utils.cpp
                          source/test_ck_blob_tree.cpp
                          source/test_ck_args.cpp
                          source/test_ck_memory_map.cpp
                          source/test_fastlz.cpp)

target_include_directories (cppkit_ut PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(cppkit_ut cppkit bzip2 pthread)

if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "iOS" AND NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    find_library(UUID_LIBRARY libuuid.a)
    target_link_libraries(cppkit_ut ${UUID_LIBRARY})
endif()
