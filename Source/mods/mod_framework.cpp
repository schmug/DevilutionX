/**
 * @file mod_framework.cpp
 *
 * Implementation of the mod framework initialization.
 */

#include "mods/mod_framework.h"
#include "mods/drop_rate_modifier.h"
#include "utils/paths.h"
#include "utils/log.hpp"

namespace devilution {

bool InitializeModFramework()
{
    LogVerbose("Initializing mod framework");
    
    // Initialize the drop rate manager
    DropRateManager& dropRateManager = DropRateManager::getInstance();
    
    // Load the drop rate configuration
    std::string configPath = paths::PrefPath() + "mods/config/drop_rate_config.json";
    bool success = dropRateManager.LoadConfig(configPath);
    
    if (!success) {
        LogWarning("Failed to load drop rate configuration from: {}", configPath);
        LogWarning("Using default drop rates");
        // Continue even if the config failed to load - we'll use default rates
    } else {
        LogVerbose("Drop rate configuration loaded successfully from: {}", configPath);
    }
    
    // Initialize other mod systems here as needed
    
    return true;
}

} // namespace devilution
