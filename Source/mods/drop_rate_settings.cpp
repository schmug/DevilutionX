/**
 * @file drop_rate_settings.cpp
 *
 * Implementation of the dynamic drop rate settings.
 */

#include "mods/drop_rate_modifier.h"
#include "utils/paths.h"
#include "utils/log.hpp"
#include "utils/file_util.h"
#include <fstream>
#include <algorithm> // For std::clamp
#include <nlohmann/json.hpp>

namespace devilution {

// Constructor implementation
DropRateManager::DropRateManager()
{
    LogVerbose("Drop rate manager initializing");
    
    // Try to load settings from file
    if (LoadSettings()) {
        LogVerbose("Loaded drop rate settings from file");
    } else {
        LogVerbose("Using default drop rate settings");
        LogVerbose("Default gold drop rate: {}%", goldDropRatePercent);
    }
}

// Gold drop rate setter implementation
void DropRateManager::SetGoldDropRatePercent(int percent)
{
    // Clamp the value to 0-100 range
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;
    
    // Set the value
    goldDropRatePercent = percent;
    
    // Log the change
    LogVerbose("Gold drop rate set to {}%", goldDropRatePercent);
    
    // Save the settings to persist the change
    SaveSettings();
}

// Reset to defaults implementation
void DropRateManager::ResetDropRatesToDefaults()
{
    // Reset gold drop rate to default
    goldDropRatePercent = 20;
    
    // Log the reset
    LogVerbose("Drop rates reset to defaults");
    LogVerbose("Gold drop rate: {}%", goldDropRatePercent);
    
    // Save the default settings
    SaveSettings();
}

// Get default settings path implementation
std::string DropRateManager::GetDefaultSettingsPath() const
{
    // Use the preferences directory for settings
    return paths::PrefPath() + "drop_rate_settings.json";
}

// Save settings implementation
bool DropRateManager::SaveSettings(const std::string& filePath)
{
    // Use the provided path or the default path
    std::string path = filePath.empty() ? GetDefaultSettingsPath() : filePath;
    
    // Save the settings to a JSON file
    bool success = SaveSettingsToJson(path);
    
    // Log the result
    if (success) {
        LogVerbose("Drop rate settings saved to: {}", path);
    } else {
        LogError("Failed to save drop rate settings to: {}", path);
    }
    
    return success;
}

// Load settings implementation
bool DropRateManager::LoadSettings(const std::string& filePath)
{
    // Use the provided path or the default path
    std::string path = filePath.empty() ? GetDefaultSettingsPath() : filePath;
    
    // Check if the file exists
    if (!::devilution::FileExists(path)) {
        LogVerbose("Drop rate settings file not found: {}", path);
        LogVerbose("Using default settings");
        return false;
    }
    
    // Load the settings from a JSON file
    bool success = LoadSettingsFromJson(path);
    
    // Log the result
    if (success) {
        LogVerbose("Drop rate settings loaded from: {}", path);
        LogVerbose("Gold drop rate: {}%", goldDropRatePercent);
    } else {
        LogError("Failed to load drop rate settings from: {}", path);
        LogVerbose("Using default settings");
    }
    
    return success;
}

// Save settings to JSON implementation
bool DropRateManager::SaveSettingsToJson(const std::string& filePath)
{
    try {
        // Create a JSON object with the settings
        nlohmann::json settings;
        settings["goldDropRatePercent"] = goldDropRatePercent;
        
        // Write the JSON to a file
        std::ofstream file(filePath);
        if (!file.is_open()) {
            LogError("Failed to open file for writing: {}", filePath);
            return false;
        }
        
        // Pretty print the JSON with 4-space indentation
        file << settings.dump(4);
        file.close();
        
        return true;
    } catch (const std::exception& e) {
        LogError("Exception while saving drop rate settings: {}", e.what());
        return false;
    }
}

// Load settings from JSON implementation
bool DropRateManager::LoadSettingsFromJson(const std::string& filePath)
{
    try {
        // Read the JSON from a file
        std::ifstream file(filePath);
        if (!file.is_open()) {
            LogError("Failed to open file for reading: {}", filePath);
            return false;
        }
        
        // Parse the JSON
        nlohmann::json settings;
        file >> settings;
        file.close();
        
        // Load the settings
        if (settings.contains("goldDropRatePercent")) {
            goldDropRatePercent = settings["goldDropRatePercent"];
            
            // Validate the gold drop rate
            if (goldDropRatePercent < 0 || goldDropRatePercent > 100) {
                LogWarning("Invalid gold drop rate in settings file: {}", goldDropRatePercent);
                goldDropRatePercent = std::clamp(goldDropRatePercent, 0, 100);
                LogWarning("Clamped to valid range: {}", goldDropRatePercent);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        LogError("Exception while loading drop rate settings: {}", e.what());
        return false;
    }
}

} // namespace devilution
