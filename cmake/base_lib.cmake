
message("devel_artifacts path: ${devel_artifacts_path}")

# First, append our compile options...
#

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug)
ENDIF(NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_FLAGS_DEBUG "")
set(CMAKE_CXX_FLAGS_RELEASE "")

macro(add_compiler_flag CONFIG FLAG)
    if("${CONFIG}" STREQUAL "Both")
        set(CMAKE_CXX_FLAGS_DEBUG "${FLAG} ${CMAKE_CXX_FLAGS_DEBUG}")
        set(CMAKE_CXX_FLAGS_RELEASE "${FLAG} ${CMAKE_CXX_FLAGS_RELEASE}")
    elseif("${CONFIG}" STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS_DEBUG "${FLAG} ${CMAKE_CXX_FLAGS_DEBUG}")
    elseif("${CONFIG}" STREQUAL "Release")
        set(CMAKE_CXX_FLAGS_RELEASE "${FLAG} ${CMAKE_CXX_FLAGS_RELEASE}")
    else()
        message(FATAL_ERROR "The CONFIG argument to add_compiler_flag must be \"Both\", \"Debug\", or \"Release\"")
    endif()
endmacro()

if(CMAKE_SYSTEM MATCHES "Linux")
    add_compiler_flag(Both -fthreadsafe-statics)
    add_compiler_flag(Both -fPIC)
    add_compiler_flag(Both -std=c++14)
    add_compiler_flag(Release -O3)
    add_compiler_flag(Release -DNDEBUG)
    add_compiler_flag(Debug -O0)
    add_compiler_flag(Debug -g)
    add_definitions(-D_LINUX)
    add_definitions(-DLINUX_OS)
    add_definitions(-D_REENTRANT)
    set(CMAKE_EXE_LINKER_FLAGS -rdynamic)
elseif(CMAKE_SYSTEM MATCHES "Windows")

    if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
        add_definitions(-DWIN64)
    endif(${CMAKE_SIZEOF_VOID_P} EQUAL 8)

    add_definitions(-DWIN32)
    add_definitions(-DUNICODE)
    add_definitions(-D_UNICODE)
    add_definitions(-DNOMINMAX)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
    add_definitions(-D__inline__=__inline)
    add_definitions(-D_SCL_SECURE_NO_WARNINGS)

    # Make sure that destructors get executed when exceptions exit C++ code.
    add_compiler_flag(Both /EHsc)
    # Enable multiprocess compilation
    add_compiler_flag(Both /MP)
    # Generate pdb files which support Edit and Continue Debugging
    add_compiler_flag(Debug /Z7)
    # Create an .sbr file with complete symbolic information.
    add_compiler_flag(Debug /FR)
    add_compiler_flag(Debug /Od)
    add_compiler_flag(Release /O2)
    SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
endif(CMAKE_SYSTEM MATCHES "Linux")

set(CMAKE_INSTALL_PREFIX ${devel_artifacts_path})
include_directories(include "${devel_artifacts_path}/include")
link_directories("${devel_artifacts_path}/lib")

if(CMAKE_SYSTEM MATCHES "Linux-")
  include_directories("/usr/local")
  link_directories("/usr/local/lib")
endif(CMAKE_SYSTEM MATCHES "Linux-")

# rpath setup
#

SET(CMAKE_SKIP_BUILD_RPATH TRUE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

# if we have been provided with additional rpath dirs, use them.
IF(ADDITIONAL_RELATIVE_RPATH)
  SET(CMAKE_INSTALL_RPATH "\$ORIGIN/libs:\$ORIGIN/${ADDITIONAL_RELATIVE_RPATH}")
ELSE()
  SET(CMAKE_INSTALL_RPATH "\$ORIGIN/libs")
ENDIF()

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Define our target name and build both SHARED and STATIC libs.

add_library(${PROJECT_NAME} SHARED ${SOURCES})
add_library(${PROJECT_NAME}_static STATIC ${SOURCES})

# If we're on windows, set our c library appropriately for all targets and build types.
IF(CMAKE_SYSTEM MATCHES "Windows")
  IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    TARGET_COMPILE_OPTIONS( ${PROJECT_NAME} PUBLIC "/MDd" )
    TARGET_COMPILE_OPTIONS( ${PROJECT_NAME}_static PUBLIC "/MTd" )
  ELSE()
    TARGET_COMPILE_OPTIONS( ${PROJECT_NAME} PUBLIC "/MD" )
    TARGET_COMPILE_OPTIONS( ${PROJECT_NAME}_static PUBLIC "/MT" )
  ENDIF()
ENDIF(CMAKE_SYSTEM MATCHES "Windows")

# linking stuff...

IF(CMAKE_SYSTEM MATCHES "Windows")
  SET(PLATFORM_LIBS DbgHelp ws2_32 iphlpapi Rpcrt4 Crypt32)
ELSEIF(CMAKE_SYSTEM MATCHES "Linux")
  SET(PLATFORM_LIBS pthread rt dl uuid)
ENDIF(CMAKE_SYSTEM MATCHES "Windows")

FOREACH(lib ${WINDOWS_LIBS})
  LIST(APPEND WINDOWS_LIBS_STATIC "${lib}_static")
ENDFOREACH(lib)

FOREACH(lib ${LINUX_LIBS})
  LIST(APPEND LINUX_LIBS_STATIC "${lib}_static")
ENDFOREACH(lib)

FOREACH(lib ${COMMON_LIBS})
  LIST(APPEND COMMON_LIBS_STATIC "${lib}_static")
ENDFOREACH(lib)

if(CMAKE_SYSTEM MATCHES "Windows")
  target_link_libraries(${PROJECT_NAME} ${WINDOWS_LIBS} ${COMMON_LIBS} ${THIRD_PARTY_LIBS} ${PLATFORM_LIBS})
  target_link_libraries(${PROJECT_NAME}_static ${WINDOWS_LIBS_STATIC} ${COMMON_LIBS_STATIC} ${THIRD_PARTY_LIBS} ${PLATFORM_LIBS})
elseif(CMAKE_SYSTEM MATCHES "Linux")
  target_link_libraries(${PROJECT_NAME} ${LINUX_LIBS} ${COMMON_LIBS} ${THIRD_PARTY_LIBS} ${PLATFORM_LIBS})
  target_link_libraries(${PROJECT_NAME}_static ${LINUX_LIBS_STATIC} ${COMMON_LIBS_STATIC} ${THIRD_PARTY_LIBS} ${PLATFORM_LIBS})
endif(CMAKE_SYSTEM MATCHES "Windows")

# Define our installation rules
#

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION "lib"
                                ARCHIVE DESTINATION "lib"
                                RUNTIME DESTINATION "lib"
                                COMPONENT library)

install(TARGETS ${PROJECT_NAME}_static LIBRARY DESTINATION "lib"
                                ARCHIVE DESTINATION "lib"
                                RUNTIME DESTINATION "lib"
                                COMPONENT library)

# if we're on MSVC, install our .pdb files as well (for debugging)
#if(MSVC)
#  IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
#    INSTALL(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.pdb
#                 DESTINATION lib
#                 CONFIGURATIONS Debug)
#  ENDIF()
#endif(MSVC)

install(TARGETS ARCHIVE DESTINATION "lib" COMPONENT library)

IF(NOT DEFINED INSTALL_SKIP_HEADERS)
install(DIRECTORY include/${PROJECT_NAME} DESTINATION include USE_SOURCE_PERMISSIONS)
ENDIF(NOT DEFINED INSTALL_SKIP_HEADERS)
