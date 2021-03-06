cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(hex2bin)

message("Supported distrib.: cmake -DDISTRIBUTION=[debug|release]")
message("Default start value: cmake -DDEFSTART=[int value] (see the '-s, --start' option of the binary)")
message("Default limit value: cmake -DDEFLIMIT=[int value] (see the '-l, --limit' option of the binary)")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CXX_OPTIONS "")
if("${DISTRIBUTION}" MATCHES "release")
  set(CXX_OPTIONS "-O3 -fstack-protector-all -D_FORTIFY_SOURCE=2 -ffunction-sections -Wall -Werror")
else()
  set(DISTRIBUTION "debug")
  set(CMAKE_CXX_FLAGS "-Wall -Og -g")
  add_definitions(-DDEBUG)
endif()
set(CMAKE_CXX_FLAGS "${CXX_OPTIONS} -std=c++11")


if("${DEFSTART}" STREQUAL "")
  set(DEFSTART 0)
endif()
if("${DEFLIMIT}" STREQUAL "")
  set(DEFLIMIT 0)
endif()

# Information
message("-- Distrib.: ${DISTRIBUTION}")
message("-- Default start value: ${DEFSTART}")
message("-- Default limit value: ${DEFLIMIT}")


add_definitions(-DDEFAULT_START=${DEFSTART})
add_definitions(-DDEFAULT_LIMIT=${DEFLIMIT})
  
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


set(common_src ${APP_SRC_DIR}/Hex2Bin.cpp)
set(bin_src ${common_src} ${APP_SRC_DIR}/main.cpp)
add_executable(hex2bin ${bin_src})


if("${DISTRIBUTION}" MATCHES "debug")
  set(bin_src ${common_src} ${APP_SRC_DIR}/main_test.cpp)
  add_executable(hex2bin_gtest ${bin_src})
  # Now simply link against gtest or gtest_main as needed. Eg
  target_link_libraries(hex2bin_gtest gtest_main)
  add_test(NAME hex2bin_test COMMAND hex2bin_gtest)
endif()
