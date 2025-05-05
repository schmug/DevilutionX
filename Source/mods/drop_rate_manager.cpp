/**
 * @file drop_rate_manager.cpp
 *
 * Implementation of the item drop rate manager.
 */

// Make sure we include the header file that defines the DropRateManager class
#include "mods/drop_rate_modifier.h"
#include "utils/paths.h"
#include "utils/log.hpp"
#include "utils/file_util.h"
#include "diablo.h"
#include "mods/config/drop_rate_validator.h"
#include <fstream>
#include <algorithm> // For std::clamp
#include <nlohmann/json.hpp>

namespace devilution {

// This implementation is commented out to avoid duplicate singletons
// The implementation in drop_rate_modifier.cpp will be used instead
/*
DropRateManager& DropRateManager::getInstance()
{
    static DropRateManager instance;
    return instance;
}
*/

DropRateManager::DropRateManager()
{
    LogVerbose("Drop rate manager initializing");
    
    // Try to load settings from file
    if (LoadSettings()) {
        LogVerbose("Loaded drop rate settings from file");
    } else {
        LogVerbose("Using default drop rate settings");
        LogVerbose("Default gold drop rate: {}%", goldDropRatePercent);
        LogVerbose("Default special object quality scaling factor: {}%", specialObjectQualityScalingFactor);
    }
}

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

void DropRateManager::ResetDropRatesToDefaults()
{
    // Reset gold drop rate to default
    goldDropRatePercent = 20;
    
    // Reset special object quality scaling factor to default
    specialObjectQualityScalingFactor = 25;
    
    // Reset item quality and item drop rate to defaults
    itemQualityPercent = 50;
    itemDropRatePercent = 80;
    
    // Log the reset
    LogVerbose("Drop rates reset to defaults");
    LogVerbose("Gold drop rate: {}%", goldDropRatePercent);
    LogVerbose("Special object quality scaling factor: {}%", specialObjectQualityScalingFactor);
    
    // Save the default settings
    SaveSettings();
}

std::string DropRateManager::GetDefaultSettingsPath() const
{
    // Use the preferences directory for settings
    return paths::PrefPath() + "drop_rate_settings.json";
}

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

bool DropRateManager::SaveSettingsToJson(const std::string& filePath)
{
    try {
        // Create a JSON object with the settings
        nlohmann::json settings;
        settings["goldDropRatePercent"] = goldDropRatePercent;
        settings["specialObjectQualityScalingFactor"] = specialObjectQualityScalingFactor;
        settings["itemQualityPercent"] = itemQualityPercent;
        settings["itemDropRatePercent"] = itemDropRatePercent;
        
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
        
        // Load special object quality scaling factor
        if (settings.contains("specialObjectQualityScalingFactor")) {
            specialObjectQualityScalingFactor = settings["specialObjectQualityScalingFactor"];
            
            // Validate the scaling factor
            if (specialObjectQualityScalingFactor < 0 || specialObjectQualityScalingFactor > 100) {
                LogWarning("Invalid special object quality scaling factor in settings file: {}", specialObjectQualityScalingFactor);
                specialObjectQualityScalingFactor = std::clamp(specialObjectQualityScalingFactor, 0, 100);
                LogWarning("Clamped to valid range: {}", specialObjectQualityScalingFactor);
            }
        }
        
        // Load item quality percent
        if (settings.contains("itemQualityPercent")) {
            itemQualityPercent = settings["itemQualityPercent"];
            
            // Validate the item quality
            if (itemQualityPercent < 0 || itemQualityPercent > 100) {
                LogWarning("Invalid item quality in settings file: {}", itemQualityPercent);
                itemQualityPercent = std::clamp(itemQualityPercent, 0, 100);
                LogWarning("Clamped to valid range: {}", itemQualityPercent);
            }
        }
        
        // Load item drop rate percent
        if (settings.contains("itemDropRatePercent")) {
            itemDropRatePercent = settings["itemDropRatePercent"];
            
            // Validate the item drop rate
            if (itemDropRatePercent < 0 || itemDropRatePercent > 100) {
                LogWarning("Invalid item drop rate in settings file: {}", itemDropRatePercent);
                itemDropRatePercent = std::clamp(itemDropRatePercent, 0, 100);
                LogWarning("Clamped to valid range: {}", itemDropRatePercent);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        LogError("Exception while loading drop rate settings: {}", e.what());
        return false;
    }
}

bool DropRateManager::LoadConfig(const std::string& configPath)
{
    LogVerbose("Loading drop rate configuration from: {}", configPath);
    
    // Validate the configuration before loading
    auto validationResult = DropRateValidator::ValidateConfigFile(configPath);
    if (!validationResult.isValid) {
        LogError("Drop rate configuration validation failed:");
        for (const auto& error : validationResult.errors) {
            LogError("  Error: {}", error);
        }
        return false;
    }
    
    // Load the configuration
    return config.LoadFromFile(configPath);
}

bool DropRateManager::ReloadConfig(const std::string& configPath)
{
    // If no config path is provided, use the last loaded config path
    std::string path = configPath;
    if (path.empty()) {
        path = paths::PrefPath() + "mods/config/drop_rate_config.json";
    }
    
    LogVerbose("Reloading drop rate configuration from: {}", path);
    
    // Check if the file exists
    if (!::devilution::FileExists(path)) {
        LogError("Drop rate configuration file not found: {}", path);
        return false;
    }
    
    // Validate the configuration before reloading
    auto validationResult = DropRateValidator::ValidateConfigFile(path);
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
        
        return false;
    } else if (!validationResult.warnings.empty()) {
        LogWarning("Drop rate configuration has warnings:");
        for (const auto& warning : validationResult.warnings) {
            LogWarning("  Warning: {}", warning);
        }
    }
    
    // Reload the configuration
    return config.LoadFromFile(path);
}

float DropRateManager::GetMonsterDropRate(const ItemData& item, int monsterLevel, bool isUnique) const
{
    int dungeonLevel = 1; // Default to level 1 if we can't get the current level
    
    // Use the current dungeon level if available
    if (currlevel > 0) {
        dungeonLevel = currlevel;
    }
    
    return config.GetModifiedDropRate(item, DropRateContext::MonsterDrop, monsterLevel, dungeonLevel, isUnique);
}

float DropRateManager::GetChestDropRate(const ItemData& item, int dungeonLevel, bool isUnique) const
{
    return config.GetModifiedDropRate(item, DropRateContext::ChestDrop, 0, dungeonLevel, isUnique);
}

float DropRateManager::GetGroundDropRate(const ItemData& item, int dungeonLevel, bool isUnique) const
{
    return config.GetModifiedDropRate(item, DropRateContext::GroundDrop, 0, dungeonLevel, isUnique);
}

float DropRateManager::GetQuestRewardDropRate(const ItemData& item, int dungeonLevel, bool isUnique) const
{
    return config.GetModifiedDropRate(item, DropRateContext::QuestReward, 0, dungeonLevel, isUnique);
}

} // namespace devilution
