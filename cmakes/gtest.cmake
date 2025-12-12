if(DEFINED IS_DEBUG AND IS_DEBUG EQUAL 1)

  include(FetchContent)
  # Download and prepare googletest
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG main
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/googletest
  )
  # Download and configure Googletest
  FetchContent_GetProperties(googletest)
  if(NOT googletest_POPULATED)
      FetchContent_Populate(googletest)
  endif()

  # Add GoogleTest to the build
  add_subdirectory(
    ${PROJECT_SOURCE_DIR}/googletest
    ${CMAKE_BINARY_DIR}/googletest
    EXCLUDE_FROM_ALL
  )
  if(MSVC)
    # Force MSVC to use the same runtime as the parent project
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  endif()

  # Creates the test executable
  set(bin_src ${common_src} ${main_test_src})
  set(gtest_exe ${H2B_PROJECT_NAME}_gtest)
  add_executable(${gtest_exe} ${bin_src})
  target_link_libraries(${gtest_exe} gtest_main)

  if(MSVC)
    set_target_properties(${gtest_exe} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${H2B_OUTPUT_BINARY_DIR})
  endif()

  add_test(NAME ${H2B_PROJECT_NAME}_test COMMAND ${gtest_exe})
endif()