/**
 * @file drop_rate_settings.h
 *
 * Implementation of the drop rate settings class.
 */
#pragma once

#include <algorithm> // for std::clamp

namespace devilution {

/**
 * @brief Class to store drop rate settings
 * 
 * This class manages the drop rate settings for different item types.
 * All values are stored as percentages (0-100).
 */
class DropRateSettings {
public:
    // Default constructor with reasonable defaults
    DropRateSettings() 
        : goldDropRate(20), // 20% default
          itemDropRate(80)  // 80% default
    {}
    
    // Getters with validation
    int GetGoldDropRate() const { return goldDropRate; }
    int GetItemDropRate() const { return itemDropRate; }
    
    // Setters with validation
    void SetGoldDropRate(int rate) {
        goldDropRate = std::clamp(rate, 0, 100);
    }
    
    void SetItemDropRate(int rate) {
        itemDropRate = std::clamp(rate, 0, 100);
    }

private:
    int goldDropRate; // 0-100 percentage
    int itemDropRate; // 0-100 percentage
};

} // namespace devilution
