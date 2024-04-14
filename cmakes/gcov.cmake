
if(DEFINED IS_DEBUG AND IS_DEBUG EQUAL 1 AND NOT DEFINED WIN32)
  # Create the gcov target. Run coverage tests with 'make gcov'
  add_custom_target(gcov
    COMMAND mkdir -p coverage
    WORKING_DIRECTORY ${H2B_ROOT_BIN_DIR}
  )
  set(OBJECT_DIR ${H2B_ROOT_BIN_DIR}/CMakeFiles/${H2B_PROJECT_NAME}.dir/src)
  add_custom_command(TARGET gcov
    COMMAND gcov -b ${hex2bin_src}.gcno -o ${OBJECT_DIR}
    COMMAND gcov -b ${intelhex_src}.gcno -o ${OBJECT_DIR}
    COMMAND gcov -b ${files_src}.gcno -o ${OBJECT_DIR}
    COMMAND gcov -b ${main_src}.gcno -o ${OBJECT_DIR}
    WORKING_DIRECTORY ${H2B_ROOT_BIN_DIR}/coverage
  )
  add_dependencies(gcov ${H2B_PROJECT_NAME})

  # Make sure to clean up the coverage folder
  set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES coverage)

  # Create the gcov-clean target. This cleans the build as well as generated 
  # .gcda and .gcno files.
  add_custom_target(scrub
    COMMAND ${CMAKE_MAKE_PROGRAM} clean
    COMMAND rm -f ${OBJECT_DIR}/*.gcno
    COMMAND rm -f ${OBJECT_DIR}/*.gcda
    WORKING_DIRECTORY ${CMAKH2B_ROOT_BIN_DIR_BINARY_DIR}
  )
endif()