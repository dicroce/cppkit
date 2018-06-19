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
                          source/framework.cpp
                          source/test_ck_string_utils.cpp
                          source/test_ck_sha_256.cpp
                          source/test_ck_socket.cpp
                          source/test_ck_server_threaded.cpp
                          source/test_ck_file.cpp
                          source/test_ck_pool.cpp
                          source/test_ck_uuid_utils.cpp
                          source/test_ck_time_utils.cpp
                          source/test_ck_blob_tree.cpp)

target_include_directories (cppkit_ut PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries (cppkit_ut LINK_PUBLIC cppkit)