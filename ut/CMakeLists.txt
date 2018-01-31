cmake_minimum_required(VERSION 2.8)
project(ut)

include(common.cmake NO_POLICY_SCOPE)

set(SOURCES source/framework.cpp
            source/ck_ssl_socket_test.cpp
            source/chrono_test.cpp
            source/ck_byte_ptr_test.cpp
            source/ck_large_files_test.cpp
            source/ck_memory_test.cpp
            source/ck_random_test.cpp
            source/ck_socket_address_test.cpp
            source/ck_socket_test.cpp
            source/ck_string_test.cpp
            source/condition_variable_test.cpp
            source/recursive_mutex_test.cpp
            source/ck_variant_test.cpp
            source/ck_json_test.cpp
            source/ck_nullable_test.cpp
            source/ck_dynamic_library_test.cpp
            source/ck_actor_test.cpp
            source/ck_command_queue_test.cpp
            source/ck_memory_map_test.cpp
            source/ck_path_test.cpp
            source/ck_udp_receiver_test.cpp
            source/ck_udp_sender_test.cpp
            source/ck_string_slice_test.cpp
            source/ck_timer_test.cpp
            source/ck_statistics_test.cpp
            source/ck_bitsy_test.cpp
            source/ck_pool_test.cpp
            source/ck_file_lock_test.cpp
            source/ck_lower_bound_test.cpp
            source/ck_md5_test.cpp
            source/ck_process_test.cpp
            source/ck_lru_cache_test.cpp
            source/ck_server_threaded_test.cpp
            source/ck_sha1_test.cpp
            source/ck_sha256_test.cpp
            source/ck_functional_test.cpp)

set(COMMON_LIBS cppkit)

if(CMAKE_SYSTEM MATCHES "Windows")
    set(APPLICATION_TYPE "WINDOWS_CONSOLE_STATIC")
    set(THIRD_PARTY_LIBS libeay32_static ssleay32_static Iphlpapi crypt32)
elseif(CMAKE_SYSTEM MATCHES "Linux")
    set(APPLICATION_TYPE "NORMAL_STATIC")
    set(THIRD_PARTY_LIBS ssl crypto) # linux ssl libs are dynamically linked...    
endif(CMAKE_SYSTEM MATCHES "Windows")

include("${devel_artifacts_path}/build/base_ut.cmake" NO_POLICY_SCOPE)
