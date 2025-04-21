# CMake configuration for the modding framework

# Add the modding framework source files to the main game
target_sources(devilutionx PRIVATE
    mod_init.cpp
    mod_framework.cpp
    drop_rate_modifier.cpp
    drop_rate_settings.cpp
    config/drop_rate_config.cpp
    config/drop_rate_validator.cpp
    drop_rate_test.cpp
)

# Add the modding framework include directories
target_include_directories(devilutionx PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Build the drop rate CLI tool as a separate executable
add_executable(droprate_tool
    drop_rate_cli.cpp
    drop_rate_test.cpp
    drop_rate_modifier.cpp
    drop_rate_settings.cpp
    config/drop_rate_config.cpp
)

# Link the drop rate CLI tool with the necessary libraries
target_link_libraries(droprate_tool PRIVATE
    devilutionx_common
)

# Install the drop rate CLI tool
install(TARGETS droprate_tool DESTINATION bin)

# Copy the sample configuration files to the installation directory
install(FILES
    config/drop_rate_config_sample.json
    config/drop_rate_schema.json
    DESTINATION share/devilutionx/mods/config
)
