/**
 * @file mod_init.cpp
 *
 * Implementation of the modding framework initialization.
 */

#include "mods/mod_init.h"

#include <string>
#include <filesystem>

#include "utils/paths.h"
#include "utils/log.hpp"
#include "utils/file_util.h"
#include "mods/drop_rate_modifier.h"
#include "mods/config/drop_rate_validator.h"
#include "mods/mod_framework.h"

namespace devilution {

bool InitializeModFramework()
{
    LogVerbose("Initializing modding framework");
    
    // Create the mods directory if it doesn't exist
    std::string modsDir = paths::PrefPath() + "mods";
    std::string configDir = modsDir + "/config";
    
    try {
        if (!std::filesystem::exists(modsDir)) {
            LogVerbose("Creating mods directory: {}", modsDir);
            std::filesystem::create_directory(modsDir);
        }
        
        if (!std::filesystem::exists(configDir)) {
            LogVerbose("Creating mods config directory: {}", configDir);
            std::filesystem::create_directory(configDir);
        }
    } catch (const std::exception& e) {
        LogError("Failed to create mods directories: {}", e.what());
        return false;
    }
    
    // Initialize the drop rate modifier
    std::string dropRateConfigPath = configDir + "/drop_rate_config.json";
    
    // If the config file doesn't exist, create a default one
    if (!std::filesystem::exists(dropRateConfigPath)) {
        LogVerbose("Creating default drop rate config at: {}", dropRateConfigPath);
        
        // Create a default configuration
        if (!DropRateConfig::getInstance().CreateDefaultConfig(dropRateConfigPath)) {
            LogError("Failed to create default drop rate config");
        }
    } else {
        // Validate the existing configuration
        auto validationResult = DropRateValidator::ValidateConfigFile(dropRateConfigPath);
        
        if (!validationResult.isValid) {
            LogError("Drop rate configuration validation failed:");
            for (const auto& error : validationResult.errors) {
                LogError("  Error: {}", error);
            }
            
            if (!validationResult.warnings.empty()) {
                LogWarning("Drop rate configuration has warnings:");
                for (const auto& warning : validationResult.warnings) {
                    LogWarning("  Warning: {}", warning);
                }
            }
            
            // Create a backup of the invalid configuration
            std::string backupPath = dropRateConfigPath + ".invalid";
            LogWarning("Creating backup of invalid configuration at: {}", backupPath);
            
            try {
                if (std::filesystem::exists(backupPath)) {
                    std::filesystem::remove(backupPath);
                }
                std::filesystem::copy_file(dropRateConfigPath, backupPath);
                
                // Replace with default configuration
                LogWarning("Replacing invalid configuration with default");
                if (!DropRateConfig::getInstance().CreateDefaultConfig(dropRateConfigPath)) {
                    LogError("Failed to create default drop rate config");
                }
            } catch (const std::exception& e) {
                LogError("Failed to backup invalid configuration: {}", e.what());
            }
        } else if (!validationResult.warnings.empty()) {
            LogWarning("Drop rate configuration has warnings:");
            for (const auto& warning : validationResult.warnings) {
                LogWarning("  Warning: {}", warning);
            }
        }
    }
    
    // Load the drop rate configuration
    if (!DropRateManager::getInstance().LoadConfig(dropRateConfigPath)) {
        LogWarning("Failed to load drop rate config, using defaults");
    } else {
        LogVerbose("Successfully loaded drop rate configuration");
    }
    
    return true;
}

} // namespace devilution
