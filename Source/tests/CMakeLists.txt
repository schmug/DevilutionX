set(TEST_SRCS
  monster_data_test.cpp
  difficulty_system_test.cpp
  gear_score_test.cpp
  difficulty_wave_test.cpp
  difficulty_integration_test.cpp
  system_verification_test.cpp
  difficulty_system_verification_tests.cpp
  run_verification_tests.cpp
  monster_data_load_test.cpp
)

# Create the test executable
add_executable(system_verification_tests ${TEST_SRCS})

# Link with the Google Test framework and our library
target_link_libraries(system_verification_tests
  PRIVATE
  gtest
  gtest_main
  libdevilutionx
)

# Include directories
target_include_directories(system_verification_tests
  PRIVATE
  ${CMAKE_SOURCE_DIR}/Source
  ${CMAKE_SOURCE_DIR}/3rdParty/googletest/googletest/include
)

# Add the test to CTest
add_test(NAME SystemVerificationTests COMMAND system_verification_tests)

# Set working directory for the test
set_tests_properties(SystemVerificationTests PROPERTIES
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Create a custom target to run the verification tests
add_custom_target(run_verification_tests
  COMMAND system_verification_tests
  DEPENDS system_verification_tests
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Running system verification tests for the dynamic difficulty system"
)
