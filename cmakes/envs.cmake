
# help
message(STATUS "Supported distrib.: cmake -DDISTRIBUTION=[debug|release]")
message(STATUS "Supported distrib.: cmake -DCMAKE_BUILD_TYPE=[debug|release]")
message(STATUS "Default start value: cmake -DDEFSTART=[int value] (see the '-s, --start' option of the binary)")
message(STATUS "Default limit value: cmake -DDEFLIMIT=[int value] (see the '-l, --limit' option of the binary)")
message(STATUS "Default width value: cmake -DDEFWIDTH=[int value] (see the '--width' option of the binary)")

# Python for update_sonar_version.py
find_package(Python3 COMPONENTS Interpreter)

if(NOT DEFINED Python3_EXECUTABLE OR Python3_EXECUTABLE STREQUAL "")
  message(FATAL_ERROR "Python interpreter not found!")
endif()

# update version
file(READ "${CMAKE_SOURCE_DIR}/version.txt" ver)
string(REGEX MATCH "VERSION_MAJOR ([0-9]*)" _ ${ver})
set(VERSION_MAJOR ${CMAKE_MATCH_1})
string(REGEX MATCH "VERSION_MINOR ([0-9]*)" _ ${ver})
set(VERSION_MINOR ${CMAKE_MATCH_1})
string(REGEX MATCH "VERSION_RELEASE ([0-9]*)" _ ${ver})
set(VERSION_RELEASE ${CMAKE_MATCH_1})

# export options
#set(DISTRIBUTION "" CACHE STRING "Distribution type (release or debug)")
#set(CMAKE_BUILD_TYPE "" CACHE STRING "Distribution type (release or debug)")
set(DEFSTART "0" CACHE STRING "Default start value (see the '-s, --start' option of the binary)")
set(DEFLIMIT "0" CACHE STRING "Default limit value (see the '-l, --limit' option of the binary)")
set(DEFWIDTH "16" CACHE STRING "Default width value (see the '--width' option of the binary)")

# sanity check
if (NOT DEFINED DISTRIBUTION OR DISTRIBUTION STREQUAL "")
  set(DISTRIBUTION "${CMAKE_BUILD_TYPE}")
  if (DISTRIBUTION STREQUAL "")
    set(DISTRIBUTION "release")
    set(CMAKE_BUILD_TYPE "release")
  endif()
endif()

string(TOLOWER ${DISTRIBUTION} DISTRIBUTION)

set(IS_DEBUG 0)
if (NOT DISTRIBUTION STREQUAL "debug" AND NOT DISTRIBUTION STREQUAL "release")
  message(FATAL_ERROR "Only debug/release supported")
elseif (DISTRIBUTION STREQUAL "debug")
  set(IS_DEBUG 1)
endif()

if(NOT DEFSTART MATCHES "^[0-9]+$")
 message(FATAL_ERROR "DEFSTART must be a valid integer: '${DEFSTART}'")
endif()

if(NOT DEFLIMIT MATCHES "^[0-9]+$")
  message(FATAL_ERROR "DEFLIMIT must be a valid integer: '${DEFLIMIT}'")
endif()

if(NOT DEFWIDTH MATCHES "^[0-9]+$")
  message(FATAL_ERROR "DEFWIDTH must be a valid integer: '${DEFWIDTH}'")
endif()
if(DEFWIDTH EQUAL 0)
  message(FATAL_ERROR "DEFWIDTH cannot be equal to 0: '${DEFWIDTH}'")
endif()
if(DEFWIDTH GREATER 255)
  message(FATAL_ERROR "DEFWIDTH cannot exceed 255: '${DEFWIDTH}'")
endif()

# Information
message(STATUS "-- Version: ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_RELEASE}")
message(STATUS "-- Distrib.: ${DISTRIBUTION}")
message(STATUS "-- Default start value: ${DEFSTART}")
message(STATUS "-- Default limit value: ${DEFLIMIT}")
message(STATUS "-- Default width value: ${DEFWIDTH}")

# update directories
set(H2B_ROOT_SRC_DIR ${CMAKE_SOURCE_DIR})
set(H2B_ROOT_BIN_DIR ${CMAKE_BINARY_DIR})
set(H2B_OUTPUT_BINARY_DIR ${H2B_ROOT_SRC_DIR}/bin)
set(H2B_SRC_DIR ${H2B_ROOT_SRC_DIR}/src)
set(H2B_INCLUDE_DIR ${H2B_ROOT_SRC_DIR}/src)

file(MAKE_DIRECTORY ${H2B_OUTPUT_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${H2B_OUTPUT_BINARY_DIR})
