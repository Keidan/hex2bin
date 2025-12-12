
if(DEFINED IS_DEBUG AND IS_DEBUG EQUAL 1 AND NOT DEFINED MSVC)
  set(OBJECT_DIR ${CMAKE_BINARY_DIR}/CMakeFiles/${CMAKE_PROJECT_NAME}.dir)
  # Create the gcov target. Run coverage tests with 'make gcov'
  add_custom_target(gcov
    COMMAND mkdir -p coverage
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
  add_custom_command(TARGET gcov
    COMMAND gcov -b ${CMAKE_SOURCE_DIR}/src/h2b/ihex/Bin2Intel.cpp.gcno -o ${OBJECT_DIR}/src/h2b/ihex/
    COMMAND gcov -b ${CMAKE_SOURCE_DIR}/src/h2b/ihex/Intel2Bin.cpp.gcno -o ${OBJECT_DIR}/src/h2b/ihex/
    COMMAND gcov -b ${CMAKE_SOURCE_DIR}/src/h2b/utils/Files.cpp.gcno -o ${OBJECT_DIR}/src/h2b/utils/
    COMMAND gcov -b ${CMAKE_SOURCE_DIR}/src/h2b/Hex2Bin.cpp.gcno -o ${OBJECT_DIR}/src/h2b/
    COMMAND gcov -b ${CMAKE_SOURCE_DIR}/src/h2b/IntelHex.cpp.gcno -o ${OBJECT_DIR}/src/h2b/
    COMMAND gcov -b ${CMAKE_SOURCE_DIR}/src/main.cpp.gcno -o ${OBJECT_DIR}/src/
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/coverage
  )
  add_dependencies(gcov ${CMAKE_PROJECT_NAME})

  # Make sure to clean up the coverage folder
  set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES coverage)

  # Create the gcov-clean target. This cleans the build as well as generated 
  # .gcda and .gcno files.
  add_custom_target(scrub
    COMMAND ${CMAKE_MAKE_PROGRAM} clean
    COMMAND find ${OBJECT_DIR} -type f -name "*.gcno" | xargs rm 2>/dev/null
    COMMAND find ${OBJECT_DIR} -type f -name "*.gcda" | xargs rm 2>/dev/null
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
endif()