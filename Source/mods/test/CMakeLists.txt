# CMake configuration for the drop rate modification system tests

# Add the test directory to the include path
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Build the drop rate test script
add_executable(drop_rate_test_script
    drop_rate_test_script.cpp
)

# Link with the necessary libraries
target_link_libraries(drop_rate_test_script
    devilutionx_common
)

# Set the output directory
set_target_properties(drop_rate_test_script
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Add a test that runs the script
add_test(
    NAME DropRateModificationTest
    COMMAND drop_rate_test_script "${CMAKE_BINARY_DIR}/test_output"
)
