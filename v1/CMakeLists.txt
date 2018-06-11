cmake_minimum_required(VERSION 2.8)
project(cppkit)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(SOURCES include/cppkit/algorithms/ck_lower_bound.h
            include/cppkit/os/ck_error_msg.h
            include/cppkit/os/ck_exports.h
            include/cppkit/os/ck_large_files.h
            include/cppkit/os/ck_platform.h
            include/cppkit/os/ck_sprintf.h
            include/cppkit/os/ck_time_utils.h
            include/cppkit/os/ck_files.h
            include/cppkit/interfaces/ck_stream_io.h
            include/cppkit/interfaces/ck_pollable.h
            include/cppkit/ck_actor.h
            include/cppkit/ck_bitsy.h
            include/cppkit/ck_byte_ptr.h
            include/cppkit/ck_command_queue.h
            include/cppkit/ck_dynamic_library.h
            include/cppkit/ck_exception.h
            include/cppkit/ck_file_lock.h
            include/cppkit/ck_json.h
            include/cppkit/ck_logger.h
            include/cppkit/ck_md5.h
            include/cppkit/ck_memory.h
            include/cppkit/ck_memory_map.h
            include/cppkit/ck_nullable.h
            include/cppkit/ck_path.h
            include/cppkit/ck_pool.h
            include/cppkit/ck_process.h
            include/cppkit/ck_socket.h
            include/cppkit/ck_socket_address.h
            include/cppkit/ck_stack_trace.h
            include/cppkit/ck_statistics.h
            include/cppkit/ck_string.h
            include/cppkit/ck_string_slice.h
            include/cppkit/ck_timer.h
            include/cppkit/ck_types.h
            include/cppkit/ck_udp_receiver.h
            include/cppkit/ck_udp_sender.h
            include/cppkit/ck_uuid.h
            include/cppkit/ck_variant.h
            include/cppkit/ck_file.h
            include/cppkit/ck_ssl_socket.h
            include/cppkit/ck_lru_cache.h
            include/cppkit/ck_server_threaded.h
            include/cppkit/ck_sha1.h
            include/cppkit/ck_sha256.h
            include/cppkit/ck_functional.h
            source/ck_library_id.cpp
            source/ck_byte_ptr.cpp
            source/ck_error_msg.cpp
            source/ck_exception.cpp
            source/ck_large_files.cpp
            source/ck_logger.cpp
            source/ck_memory.cpp
            source/ck_socket_address.cpp
            source/ck_socket.cpp
            source/ck_sprintf.cpp
            source/ck_stack_trace.cpp
            source/ck_string.cpp
            source/ck_string_slice.cpp
            source/ck_time_utils.cpp
            source/ck_files.cpp
            source/ck_variant.cpp
            source/ck_json.cpp
            source/ck_nullable.cpp
            source/ck_dynamic_library.cpp
            source/ck_actor.cpp
            source/ck_command_queue.cpp
            source/ck_memory_map.cpp
            source/ck_path.cpp
            source/ck_udp_sender.cpp
            source/ck_udp_receiver.cpp
            source/ck_timer.cpp
            source/ck_pool.cpp
            source/ck_file_lock.cpp
            source/ck_lower_bound.cpp
            source/ck_md5.cpp
            source/ck_process.cpp
            source/ck_uuid.cpp
            source/ck_file.cpp
	          source/ck_ssl_socket.cpp
            source/ck_sha1.cpp
            source/ck_sha256.cpp)

# cppkit is special and has to create ../devel_artifacts for everyone else.
file(MAKE_DIRECTORY ../devel_artifacts)

include(cmake/common.cmake NO_POLICY_SCOPE)
include(cmake/base_lib.cmake NO_POLICY_SCOPE)

if(CMAKE_SYSTEM MATCHES "Windows")
  set(WINDOWS_LIBS libeay32 ssleay32)
  if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    TARGET_INCLUDE_DIRECTORIES( ${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Win64-OpenSSL-VS2017/debug/dynamic/include" )
    TARGET_INCLUDE_DIRECTORIES( ${PROJECT_NAME}_static PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Win64-OpenSSL-VS2017/debug/static/include" )
  else()
    TARGET_INCLUDE_DIRECTORIES( ${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Win64-OpenSSL-VS2017/release/dynamic/include" )
    TARGET_INCLUDE_DIRECTORIES( ${PROJECT_NAME}_static PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Win64-OpenSSL-VS2017/release/static/include" )
  endif()
endif(CMAKE_SYSTEM MATCHES "Windows")

if(CMAKE_SYSTEM MATCHES "Windows")
  if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    target_link_libraries(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/Win64-OpenSSL-VS2017/debug/dynamic/libs/libeay32.lib" "${CMAKE_CURRENT_SOURCE_DIR}/Win64-OpenSSL-VS2017/debug/dynamic/libs/ssleay32.lib")
    target_link_libraries(${PROJECT_NAME}_static "${CMAKE_CURRENT_SOURCE_DIR}/Win64-OpenSSL-VS2017/debug/static/libs/libeay32_static.lib" "${CMAKE_CURRENT_SOURCE_DIR}/Win64-OpenSSL-VS2017/debug/static/libs/ssleay32_static.lib")
  else()
    target_link_libraries(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/Win64-OpenSSL-VS2017/release/dynamic/libs/libeay32.lib" "${CMAKE_CURRENT_SOURCE_DIR}/Win64-OpenSSL-VS2017/release/dynamic/libs/ssleay32.lib")
    target_link_libraries(${PROJECT_NAME}_static "${CMAKE_CURRENT_SOURCE_DIR}/Win64-OpenSSL-VS2017/release/static/libs/libeay32_static.lib" "${CMAKE_CURRENT_SOURCE_DIR}/Win64-OpenSSL-VS2017/release/static/libs/ssleay32_static.lib")
  endif()
elseif(CMAKE_SYSTEM MATCHES "Linux")
    target_link_libraries(${PROJECT_NAME} ssl crypto)
endif(CMAKE_SYSTEM MATCHES "Windows")

install(FILES cmake/common.cmake DESTINATION "${devel_artifacts_path}/build")
install(FILES cmake/base_lib.cmake DESTINATION "${devel_artifacts_path}/build")
install(FILES cmake/base_ut.cmake DESTINATION "${devel_artifacts_path}/build")
install(FILES cmake/base_app.cmake DESTINATION "${devel_artifacts_path}/build")
install(FILES cmake/base_header_only.cmake DESTINATION "${devel_artifacts_path}/build")

if(CMAKE_SYSTEM MATCHES "Windows")
install(DIRECTORY Win64-OpenSSL-VS2017/release/dynamic/include/openssl DESTINATION "${devel_artifacts_path}/include" USE_SOURCE_PERMISSIONS)
endif(CMAKE_SYSTEM MATCHES "Windows")

if(CMAKE_SYSTEM MATCHES "Windows")
  if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    install(FILES Win64-OpenSSL-VS2017/debug/dynamic/libs/libeay32.lib DESTINATION "${devel_artifacts_path}/lib")
    install(FILES Win64-OpenSSL-VS2017/debug/dynamic/libs/libeay32.dll DESTINATION "${devel_artifacts_path}/lib")
    install(FILES Win64-OpenSSL-VS2017/debug/dynamic/libs/ssleay32.lib DESTINATION "${devel_artifacts_path}/lib")
    install(FILES Win64-OpenSSL-VS2017/debug/dynamic/libs/ssleay32.dll DESTINATION "${devel_artifacts_path}/lib")
    install(FILES Win64-OpenSSL-VS2017/debug/static/libs/libeay32_static.lib DESTINATION "${devel_artifacts_path}/lib")
    install(FILES Win64-OpenSSL-VS2017/debug/static/libs/ssleay32_static.lib DESTINATION "${devel_artifacts_path}/lib")
  else()
    install(FILES Win64-OpenSSL-VS2017/release/dynamic/libs/libeay32.lib DESTINATION "${devel_artifacts_path}/lib")
    install(FILES Win64-OpenSSL-VS2017/release/dynamic/libs/libeay32.dll DESTINATION "${devel_artifacts_path}/lib")
    install(FILES Win64-OpenSSL-VS2017/release/dynamic/libs/ssleay32.lib DESTINATION "${devel_artifacts_path}/lib")
    install(FILES Win64-OpenSSL-VS2017/release/dynamic/libs/ssleay32.dll DESTINATION "${devel_artifacts_path}/lib")
    install(FILES Win64-OpenSSL-VS2017/release/static/libs/libeay32_static.lib DESTINATION "${devel_artifacts_path}/lib")
    install(FILES Win64-OpenSSL-VS2017/release/static/libs/ssleay32_static.lib DESTINATION "${devel_artifacts_path}/lib")
  endif()
endif(CMAKE_SYSTEM MATCHES "Windows")
