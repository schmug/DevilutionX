/**
 * @file gear_config.h
 *
 * Interface for the gear scoring configuration system.
 */
#pragma once

#include <cstdint>
#include <array>
#include <string>
#include <vector>
#include <optional>

#include "gear/gear_score.h"
#include "gear/affix_score.h"

namespace devilution {

/**
 * @brief Configuration structure for quality base scores
 */
struct QualityScoreConfig {
    float normalScore;
    float magicScore;
    float rareScore;
    float uniqueScore;
    float setScore;
    float craftedScore;
};

/**
 * @brief Configuration structure for category multipliers
 */
struct CategoryMultiplierConfig {
    float weaponMultiplier;
    float armorMultiplier;
    float shieldMultiplier;
    float helmMultiplier;
    float jewelryMultiplier;
    float otherMultiplier;
};

/**
 * @brief Configuration structure for slot importance weights
 */
struct SlotImportanceConfig {
    float headWeight;
    float ringLeftWeight;
    float ringRightWeight;
    float amuletWeight;
    float handLeftWeight;
    float handRightWeight;
    float chestWeight;
};

/**
 * @brief Configuration structure for affix scoring
 */
struct AffixScoreConfig {
    std::string name;
    AffixCategory category;
    float baseWeight;
    float valueMultiplier;
    float weaponMultiplier;
    float armorMultiplier;
    float shieldMultiplier;
    float helmMultiplier;
    float jewelryMultiplier;
};

/**
 * @brief Main configuration structure for the gear scoring system
 */
struct GearScoringConfig {
    QualityScoreConfig qualityScores;
    CategoryMultiplierConfig categoryMultipliers;
    SlotImportanceConfig slotImportance;
    std::vector<AffixScoreConfig> affixScores;
    
    // Scoring formula parameters
    float levelFactorBase;
    float levelFactorMultiplier;
    float levelFactorMax;
    float uniqueItemBonus;
    float setItemBonus;
    
    // Gear level calculation parameters
    float characterLevelWeight;
    float gearScoreWeight;
    
    // Normalization parameters
    float minGearLevel;
    float maxGearLevel;
    float rawScoreMin;
    float rawScoreMax;
};

/**
 * @brief Class for loading and managing gear scoring configuration
 */
class GearConfigLoader {
public:
    /**
     * @brief Loads configuration from a JSON file
     * @param configPath Path to the configuration file
     * @return True if loading was successful, false otherwise
     */
    static bool LoadFromFile(const std::string &configPath);
    
    /**
     * @brief Gets the current configuration
     * @return The current gear scoring configuration
     */
    static const GearScoringConfig &GetConfig();
    
    /**
     * @brief Saves the current configuration to a JSON file
     * @param configPath Path to save the configuration file
     * @return True if saving was successful, false otherwise
     */
    static bool SaveToFile(const std::string &configPath);
    
    /**
     * @brief Creates a default configuration
     * @return A default gear scoring configuration
     */
    static GearScoringConfig CreateDefaultConfig();
    
private:
    static std::optional<GearScoringConfig> config;
};

} // namespace devilution
