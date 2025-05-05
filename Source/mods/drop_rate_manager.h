/**
 * @file drop_rate_manager.h
 *
 * Implementation of the drop rate manager singleton.
 */
#pragma once

#include "mods/drop_rate_settings.h"
#include "utils/log.hpp"

namespace devilution {

/**
 * @brief Singleton manager for drop rate settings
 * 
 * This class provides global access to drop rate settings
 * and ensures they are consistently applied throughout the game.
 */
class DropRateManager {
public:
    // Singleton access
    static DropRateManager& GetInstance() {
        static DropRateManager instance;
        return instance;
    }
    
    // Get current settings
    const DropRateSettings& GetSettings() const {
        return settings;
    }
    
    // Update settings
    void UpdateSettings(const DropRateSettings& newSettings) {
        settings = newSettings;
        LogInfo("Drop rate settings updated");
    }
    
    // Convenience methods for gold drop rate
    int GetGoldDropRate() const {
        return settings.GetGoldDropRate();
    }
    
    void SetGoldDropRate(int rate) {
        settings.SetGoldDropRate(rate);
        LogInfo("Gold drop rate set to {}%", settings.GetGoldDropRate());
    }
    
    // Convenience methods for item drop rate
    int GetItemDropRate() const {
        return settings.GetItemDropRate();
    }
    
    void SetItemDropRate(int rate) {
        settings.SetItemDropRate(rate);
        LogInfo("Item drop rate set to {}%", settings.GetItemDropRate());
    }
    
    // Convenience methods for special object quality scaling factor
    int GetSpecialObjectQualityScalingFactor() const {
        return settings.GetSpecialObjectQualityScalingFactor();
    }
    
    void SetSpecialObjectQualityScalingFactor(int factor) {
        settings.SetSpecialObjectQualityScalingFactor(factor);
        LogInfo("Special object quality scaling factor set to {}%", settings.GetSpecialObjectQualityScalingFactor());
    }
    
    // Reset to default values
    void ResetToDefaults() {
        settings = DropRateSettings(); // Use default constructor values
        LogInfo("Drop rate settings reset to defaults");
    }

private:
    // Private constructor for singleton
    DropRateManager() {
        LogInfo("Drop rate manager initialized");
    }
    
    // Settings storage
    DropRateSettings settings;
    
    // Delete copy constructor and assignment operator
    DropRateManager(const DropRateManager&) = delete;
    DropRateManager& operator=(const DropRateManager&) = delete;
};

} // namespace devilution
