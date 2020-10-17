cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(hex2bin)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-O0 -g -fstack-protector-all -D_FORTIFY_SOURCE=2 -ffunction-sections -Wall -Werror -std=c++11")


# update directories
set(APP_BINARY_DIR ${CMAKE_SOURCE_DIR})
set(APP_SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(APP_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src)

file(MAKE_DIRECTORY ${APP_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${APP_BINARY_DIR})


# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()


set(bin_src
	${APP_SRC_DIR}/Hex2Bin.cpp
	${APP_SRC_DIR}/main.cpp
)
add_executable(hex2bin ${bin_src})


set(bin_src
	${APP_SRC_DIR}/Hex2Bin.cpp
	${APP_SRC_DIR}/main_test.cpp
)
add_executable(hex2bin_gtest ${bin_src})
# Now simply link against gtest or gtest_main as needed. Eg
target_link_libraries(hex2bin_gtest gtest_main)
add_test(NAME hex2bin_test COMMAND hex2bin_gtest)
