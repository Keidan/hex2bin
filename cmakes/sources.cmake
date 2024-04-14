

configure_file (
  "${H2B_SRC_DIR}/config.h.in"
  "${H2B_SRC_DIR}/config.h"
)
add_custom_target(
 update_sonar_version ALL
 COMMAND ${Python_EXECUTABLE} ${H2B_ROOT_SRC_DIR}/update_sonar_version.py --root ${H2B_ROOT_SRC_DIR}
 COMMENT "Target update_sonar_version"
)


set(hex2bin_src ${H2B_SRC_DIR}/Hex2Bin.cpp)
set(intelhex_src ${H2B_SRC_DIR}/IntelHex.cpp)
set(files_src ${H2B_SRC_DIR}/Files.cpp)
set(main_src ${H2B_SRC_DIR}/main.cpp)
set(main_test_src ${H2B_SRC_DIR}/main_test.cpp)
set(common_src ${hex2bin_src} ${intelhex_src} ${files_src})
set(bin_src ${common_src} ${main_src})
if(WIN32)
  set(bin_src ${bin_src} ${H2B_SRC_DIR}/win32/getopt.c ${H2B_SRC_DIR}/win32/getopt_long.c ${H2B_SRC_DIR}/win32/getopt.h)
endif()

add_executable(${H2B_PROJECT_NAME} ${bin_src})


if(MSVC)
  set_target_properties(${H2B_PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${H2B_OUTPUT_BINARY_DIR})
  set_target_properties(${H2B_PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${H2B_OUTPUT_BINARY_DIR})
endif(MSVC)

add_dependencies(${H2B_PROJECT_NAME} update_sonar_version)

