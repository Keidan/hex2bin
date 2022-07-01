cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(hex2bin)

file(READ "version.txt" ver)
string(REGEX MATCH "VERSION_MAJOR ([0-9]*)" _ ${ver})
set(VERSION_MAJOR ${CMAKE_MATCH_1})
string(REGEX MATCH "VERSION_MINOR ([0-9]*)" _ ${ver})
set(VERSION_MINOR ${CMAKE_MATCH_1})

message("Supported distrib.: cmake -DDISTRIBUTION=[debug|release]")
message("Default start value: cmake -DDEFSTART=[int value] (see the '-s, --start' option of the binary)")
message("Default limit value: cmake -DDEFLIMIT=[int value] (see the '-l, --limit' option of the binary)")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DISTRIBUTION "release" CACHE STRING "Distribution type (release or debug)")

set(CXX_OPTIONS "")
set(LINKER_OPTIONS "")
if("${DISTRIBUTION}" MATCHES "release")
  if(WIN32)
    set(CXX_OPTIONS "/O2 /Zi /W3 /Gd /Gy /GS /Oi /MT /FC /EHsc")
    set(LINKER_OPTIONS "/DEBUG /INCREMENTAL:NO")
    add_definitions(-DWIN32)
  else(WIN32)
    set(CXX_OPTIONS "-O3 -fstack-protector-all -D_FORTIFY_SOURCE=2 -ffunction-sections -Wall -Werror -std=c++11")
  endif(WIN32)
  add_definitions(-DNDEBUG)
else()
  set(DISTRIBUTION "debug")
  if(WIN32)
    set(CXX_OPTIONS "/Od /ZI /W3 /Gd /GS /MTd /FC /EHsc")
    set(LINKER_OPTIONS "/INCREMENTAL")
    add_definitions(-DWIN32)
  else(WIN32)
    set(CXX_OPTIONS "-Wall -Og -g -std=c++11")
  endif(WIN32)
  add_definitions(-DDEBUG)
  add_definitions(-D_DEBUG)
endif()
set(CMAKE_CXX_FLAGS "${CXX_OPTIONS}")
set(CMAKE_EXE_LINKER_FLAGS "${LINKER_OPTIONS}")

set(DEFSTART "0" CACHE STRING "Default start value (see the '-s, --start' option of the binary)")
set(DEFLIMIT "0" CACHE STRING "Default limit value (see the '-l, --limit' option of the binary)")

if(NOT DEFSTART MATCHES "^[0-9]+$")
   message(FATAL_ERROR "DEFSTART must be a valid integer: '${DEFSTART}'")
endif()
if(NOT DEFLIMIT MATCHES "^[0-9]+$")
   message(FATAL_ERROR "DEFLIMIT must be a valid integer: '${DEFLIMIT}'")
endif()

# Information
message("-- Version: ${VERSION_MAJOR}.${VERSION_MINOR}")
message("-- Distrib.: ${DISTRIBUTION}")
message("-- Default start value: ${DEFSTART}")
message("-- Default limit value: ${DEFLIMIT}")

configure_file (
  "${PROJECT_SOURCE_DIR}/src/config.h.in"
  "${PROJECT_SOURCE_DIR}/src/config.h"
)
  
# update directories
set(APP_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(APP_SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(APP_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src)

file(MAKE_DIRECTORY ${APP_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${APP_BINARY_DIR})


if("${DISTRIBUTION}" MATCHES "debug")
  # Download and unpack googletest at configure time
  configure_file(CMakeLists.txt.in googletest/download/CMakeLists.txt)
  execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest/download )
  if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
  endif()
  execute_process(COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest/download )
  if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
  endif()

  # Prevent overriding the parent project's compiler/linker
  # settings on Windows
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  # Add googletest directly to our build. This defines
  # the gtest and gtest_main targets.
  add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest/src
                   ${CMAKE_CURRENT_BINARY_DIR}/googletest/build
                   EXCLUDE_FROM_ALL)

  # The gtest/gtest_main targets carry header search path
  # dependencies automatically when using CMake 2.8.11 or
  # later. Otherwise we have to add them here ourselves.
  if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
  endif()
endif()

add_custom_target(
 update_sonar_version ALL
 COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/update_sonar_version.py --root ${CMAKE_CURRENT_SOURCE_DIR}
 COMMENT "Target update_sonar_version"
)
set(common_src ${APP_SRC_DIR}/Hex2Bin.cpp)
set(bin_src ${common_src} ${APP_SRC_DIR}/main.cpp)
if(WIN32)
  set(bin_src ${bin_src} ${APP_SRC_DIR}/win32/getopt.c ${APP_SRC_DIR}/win32/getopt_long.c ${APP_SRC_DIR}/win32/getopt.h)
endif()
add_executable(hex2bin ${bin_src})
add_dependencies(hex2bin update_sonar_version)


if("${DISTRIBUTION}" MATCHES "debug")
  set(bin_src ${common_src} ${APP_SRC_DIR}/main_test.cpp)
  add_executable(hex2bin_gtest ${bin_src})
  # Now simply link against gtest or gtest_main as needed. Eg
  target_link_libraries(hex2bin_gtest gtest_main)
  add_test(NAME hex2bin_test COMMAND hex2bin_gtest)
endif()
