if(DEFINED IS_DEBUG AND IS_DEBUG EQUAL 1)
  # Download and unpack googletest at configure time
  configure_file(${PROJECT_SOURCE_DIR}/cmakes/CMakeLists.txt.in ${PROJECT_SOURCE_DIR}/googletest/download/CMakeLists.txt)
  execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/googletest/download )
  if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
  endif()
  execute_process(COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/googletest/download )
  if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
  endif()

  # Prevent overriding the parent project's compiler/linker
  # settings on Windows
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  # Add googletest directly to our build. This defines
  # the gtest and gtest_main targets.
  add_subdirectory(${PROJECT_SOURCE_DIR}/googletest/src
                   ${PROJECT_SOURCE_DIR}/googletest/build
                   EXCLUDE_FROM_ALL)

  set(bin_src ${common_src} ${main_test_src})
  set(gtest ${H2B_PROJECT_NAME}_gtest)
  add_executable(${gtest} ${bin_src})
  # Now simply link against gtest or gtest_main as needed. Eg
  target_link_libraries(${gtest} gtest_main)
  if(MSVC)
    set_target_properties(${gtest} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${H2B_OUTPUT_BINARY_DIR} )
  endif(MSVC)
  add_test(NAME ${H2B_PROJECT_NAME}_test COMMAND ${gtest})
endif()