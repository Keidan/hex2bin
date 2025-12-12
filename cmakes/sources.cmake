

configure_file (
  "${H2B_SRC_DIR}/config.h.in"
  "${H2B_SRC_DIR}/config.h"
)
add_custom_target(
 update_sonar_version ALL
 COMMAND ${Python3_EXECUTABLE} ${H2B_ROOT_SRC_DIR}/update_sonar_version.py --root ${H2B_ROOT_SRC_DIR}
 COMMENT "Target update_sonar_version"
)


set(hex2bin_src ${H2B_SRC_DIR}/h2b/Hex2Bin.cpp)
set(intelhex_src ${H2B_SRC_DIR}/h2b/IntelHex.cpp)
set(bin2intel_src ${H2B_SRC_DIR}/h2b/ihex/Bin2Intel.cpp)
set(intel2bin_src ${H2B_SRC_DIR}/h2b/ihex/Intel2Bin.cpp)
set(files_src ${H2B_SRC_DIR}/h2b/utils/Files.cpp)
set(main_src ${H2B_SRC_DIR}/main.cpp)
set(main_test_src ${H2B_SRC_DIR}/main_test.cpp)
set(common_src ${hex2bin_src} ${intelhex_src} ${bin2intel_src} ${intel2bin_src} ${files_src})
set(bin_src ${common_src} ${main_src})
if(MSVC)
  set(bin_src ${bin_src} ${H2B_SRC_DIR}/win32/getopt.c ${H2B_SRC_DIR}/win32/getopt_long.c ${H2B_SRC_DIR}/win32/getopt.h)
endif()

add_executable(${H2B_PROJECT_NAME} ${bin_src})
set(HDR_DIR "${H2B_SRC_DIR}")
if(DEFINED IS_DEBUG AND IS_DEBUG EQUAL 1)
  set(HDR_DIR "${HDR_DIR}" "${PROJECT_SOURCE_DIR}/googletest/src/include")
endif()
include_directories(${HDR_DIR})


if(MSVC)
  set_target_properties(${H2B_PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${H2B_OUTPUT_BINARY_DIR})
  set_target_properties(${H2B_PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${H2B_OUTPUT_BINARY_DIR})
endif()

add_dependencies(${H2B_PROJECT_NAME} update_sonar_version)

