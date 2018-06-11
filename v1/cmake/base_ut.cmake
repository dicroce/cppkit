
message("devel_artifacts path: ${devel_artifacts_path}")

# First, append our compile options...
#

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS_DEBUG "")
set(CMAKE_CXX_FLAGS_RELEASE "")

macro(add_compiler_flag CONFIG FLAG)
    if("${CONFIG}" STREQUAL "Both")
        set(CMAKE_CXX_FLAGS "${FLAG} ${CMAKE_CXX_FLAGS}")
        set(CMAKE_CXX_FLAGS_RELEASE "${FLAG} ${CMAKE_CXX_FLAGS_RELEASE}")
    elseif("${CONFIG}" STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS_DEBUG "${FLAG} ${CMAKE_CXX_FLAGS_DEBUG}")
    elseif("${CONFIG}" STREQUAL "Release")
        set(CMAKE_CXX_FLAGS_RELEASE "${FLAG} ${CMAKE_CXX_FLAGS_RELEASE}")
    else()
        message(FATAL_ERROR "The CONFIG argument to add_compiler_flag must be \"Both\", \"Debug\", or \"Release\"")
    endif()
endmacro()

if(CMAKE_SYSTEM MATCHES "Linux-")
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

    add_definitions(-DWIN32_LEAN_AND_MEAN)
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
endif(CMAKE_SYSTEM MATCHES "Linux-")

# Now, setup our artifact install root and add our default header and lib paths.
#

set(CMAKE_INSTALL_PREFIX ${devel_artifacts_path})
get_filename_component(ABSOLUTE_INC_DIR "${devel_artifacts_path}/include" ABSOLUTE)
include_directories(include ${ABSOLUTE_INC_DIR})
get_filename_component(ABSOLUTE_LIB_DIR "${devel_artifacts_path}/lib" ABSOLUTE)
link_directories(${ABSOLUTE_LIB_DIR})

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

# Add our executable target
#

IF(CMAKE_SYSTEM MATCHES "Windows")
  SET(PLATFORM_LIBS DbgHelp ws2_32 iphlpapi Rpcrt4)
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

# So basically, if we're building for Windows we include the "WIN32" in the
# add_executable to get it to build a real Windows application. All of the
# stuff in the "Windows" conditional is to get it to compile in our icon.

if(${APPLICATION_TYPE} STREQUAL "WINDOWS")
  set(RES_FILES ${RC_FILE})
  set(CMAKE_RC_COMPILER_INIT windres)
  ENABLE_LANGUAGE(RC)
  SET(CMAKE_RC_COMPILE_OBJECT
  "<CMAKE_RC_COMPILER> <FLAGS> -o coff <DEFINES> -i <SOURCE> -o <OBJECT>")
  add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${RES_FILES})
  target_link_libraries(${PROJECT_NAME} ${WINDOWS_LIBS} ${COMMON_LIBS} ${THIRD_PARTY_LIBS} ${PLATFORM_LIBS})
  IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
      TARGET_COMPILE_OPTIONS( ${PROJECT_NAME} PUBLIC "/MDd" )
  ELSE()
      TARGET_COMPILE_OPTIONS( ${PROJECT_NAME} PUBLIC "/MD" )
  ENDIF()
endif(${APPLICATION_TYPE} STREQUAL "WINDOWS")

if(${APPLICATION_TYPE} STREQUAL "WINDOWS_STATIC")
  set(RES_FILES ${RC_FILE})
  set(CMAKE_RC_COMPILER_INIT windres)
  ENABLE_LANGUAGE(RC)
  SET(CMAKE_RC_COMPILE_OBJECT
  "<CMAKE_RC_COMPILER> <FLAGS> -o coff <DEFINES> -i <SOURCE> -o <OBJECT>")
  add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${RES_FILES})
  target_link_libraries(${PROJECT_NAME} ${WINDOWS_LIBS_STATIC} ${COMMON_LIBS_STATIC} ${THIRD_PARTY_LIBS} ${PLATFORM_LIBS})
  IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
      TARGET_COMPILE_OPTIONS( ${PROJECT_NAME} PUBLIC "/MTd" )
  ELSE()
      TARGET_COMPILE_OPTIONS( ${PROJECT_NAME} PUBLIC "/MT" )
  ENDIF()
endif(${APPLICATION_TYPE} STREQUAL "WINDOWS_STATIC")

if(${APPLICATION_TYPE} STREQUAL "WINDOWS_CONSOLE")
  set(RES_FILES ${RC_FILE})
  set(CMAKE_RC_COMPILER_INIT windres)
  ENABLE_LANGUAGE(RC)
  SET(CMAKE_RC_COMPILE_OBJECT
  "<CMAKE_RC_COMPILER> <FLAGS> -o coff <DEFINES> -i <SOURCE> -o <OBJECT>")
  add_executable(${PROJECT_NAME} ${SOURCES} ${RES_FILES})
  target_link_libraries(${PROJECT_NAME} ${WINDOWS_LIBS} ${COMMON_LIBS} ${THIRD_PARTY_LIBS} ${PLATFORM_LIBS})
  IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
      TARGET_COMPILE_OPTIONS( ${PROJECT_NAME} PUBLIC "/MDd" )
  ELSE()
      TARGET_COMPILE_OPTIONS( ${PROJECT_NAME} PUBLIC "/MD" )
  ENDIF()
endif(${APPLICATION_TYPE} STREQUAL "WINDOWS_CONSOLE")

if(${APPLICATION_TYPE} STREQUAL "WINDOWS_CONSOLE_STATIC")
  set(RES_FILES ${RC_FILE})
  set(CMAKE_RC_COMPILER_INIT windres)
  ENABLE_LANGUAGE(RC)
  SET(CMAKE_RC_COMPILE_OBJECT
  "<CMAKE_RC_COMPILER> <FLAGS> -o coff <DEFINES> -i <SOURCE> -o <OBJECT>")
  add_executable(${PROJECT_NAME} ${SOURCES} ${RES_FILES})
  target_link_libraries(${PROJECT_NAME} ${WINDOWS_LIBS_STATIC} ${COMMON_LIBS_STATIC} ${THIRD_PARTY_LIBS} ${PLATFORM_LIBS})
  IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
      TARGET_COMPILE_OPTIONS( ${PROJECT_NAME} PUBLIC "/MTd" )
  ELSE()
      TARGET_COMPILE_OPTIONS( ${PROJECT_NAME} PUBLIC "/MT" )
  ENDIF()
endif(${APPLICATION_TYPE} STREQUAL "WINDOWS_CONSOLE_STATIC")

if(${APPLICATION_TYPE} STREQUAL "NORMAL")
  add_executable(${PROJECT_NAME} ${SOURCES})
  target_link_libraries(${PROJECT_NAME} ${LINUX_LIBS} ${COMMON_LIBS} ${THIRD_PARTY_LIBS} ${PLATFORM_LIBS})
endif(${APPLICATION_TYPE} STREQUAL "NORMAL")

if(${APPLICATION_TYPE} STREQUAL "NORMAL_STATIC")
  add_executable(${PROJECT_NAME} ${SOURCES})
  target_link_libraries(${PROJECT_NAME} ${LINUX_LIBS_STATIC} ${COMMON_LIBS_STATIC} ${THIRD_PARTY_LIBS} ${PLATFORM_LIBS})
endif(${APPLICATION_TYPE} STREQUAL "NORMAL_STATIC")
