/**
 * @file gear_config.cpp
 *
 * Implementation of the gear scoring configuration system.
 */
#include "gear/gear_config.h"

#include <fstream>
#include <algorithm>

#include "utils/log.hpp"
#include "utils/file_util.h"
#include "utils/paths.h"

namespace devilution {

// Initialize the static configuration
std::optional<GearScoringConfig> GearConfigLoader::config;

bool GearConfigLoader::LoadFromFile(const std::string &configPath)
{
    try {
        // For now, we'll just create a default config
        // In a real implementation, we would parse a JSON file here
        config = CreateDefaultConfig();
        LogVerbose("Loaded gear scoring configuration from default values");
        return true;
    } catch (const std::exception &e) {
        LogError("Failed to load gear scoring configuration: {}", e.what());
        return false;
    }
}

const GearScoringConfig &GearConfigLoader::GetConfig()
{
    if (!config.has_value()) {
        // If no config is loaded, create a default one
        config = CreateDefaultConfig();
        LogVerbose("Created default gear scoring configuration");
    }
    
    return config.value();
}

bool GearConfigLoader::SaveToFile(const std::string &configPath)
{
    try {
        // For now, we'll just log that we would save the config
        // In a real implementation, we would serialize to JSON here
        LogVerbose("Would save gear scoring configuration to: {}", configPath);
        return true;
    } catch (const std::exception &e) {
        LogError("Failed to save gear scoring configuration: {}", e.what());
        return false;
    }
}

GearScoringConfig GearConfigLoader::CreateDefaultConfig()
{
    GearScoringConfig config;
    
    // Set default quality scores
    config.qualityScores.normalScore = 1.0f;
    config.qualityScores.magicScore = 2.0f;
    config.qualityScores.rareScore = 3.5f;
    config.qualityScores.uniqueScore = 5.0f;
    config.qualityScores.setScore = 4.5f;
    config.qualityScores.craftedScore = 4.0f;
    
    // Set default category multipliers
    config.categoryMultipliers.weaponMultiplier = 1.5f;
    config.categoryMultipliers.armorMultiplier = 1.2f;
    config.categoryMultipliers.shieldMultiplier = 1.0f;
    config.categoryMultipliers.helmMultiplier = 0.8f;
    config.categoryMultipliers.jewelryMultiplier = 1.0f;
    config.categoryMultipliers.otherMultiplier = 0.5f;
    
    // Set default slot importance weights
    config.slotImportance.headWeight = 0.8f;
    config.slotImportance.ringLeftWeight = 0.6f;
    config.slotImportance.ringRightWeight = 0.6f;
    config.slotImportance.amuletWeight = 0.7f;
    config.slotImportance.handLeftWeight = 1.5f;
    config.slotImportance.handRightWeight = 1.0f;
    config.slotImportance.chestWeight = 1.3f;
    
    // Set default affix scoring configurations
    config.affixScores = {
        // Name                 Category                  BaseWeight  ValueMult  Weapon  Armor   Shield  Helm    Jewelry
        { "Strength",           AffixCategory::Attribute,  0.5f,      1.0f,      1.2f,   1.1f,   1.0f,   1.0f,   0.9f   },
        { "Magic",              AffixCategory::Attribute,  0.5f,      1.0f,      0.9f,   0.9f,   0.9f,   1.0f,   1.3f   },
        { "Dexterity",          AffixCategory::Attribute,  0.5f,      1.0f,      1.3f,   0.9f,   1.0f,   1.0f,   1.0f   },
        { "Vitality",           AffixCategory::Attribute,  0.5f,      1.0f,      0.8f,   1.2f,   1.2f,   1.1f,   0.9f   },
        { "All Attributes",     AffixCategory::Attribute,  0.7f,      1.0f,      1.1f,   1.1f,   1.1f,   1.1f,   1.2f   },
        { "Damage",             AffixCategory::Offensive,  0.6f,      0.3f,      1.5f,   0.7f,   0.8f,   0.7f,   1.0f   },
        { "To-Hit",             AffixCategory::Offensive,  0.4f,      0.05f,     1.4f,   0.6f,   0.7f,   0.7f,   1.0f   },
        { "Armor Class",        AffixCategory::Defensive,  0.5f,      0.2f,      0.5f,   1.4f,   1.3f,   1.2f,   0.7f   },
        { "Fire Resist",        AffixCategory::Defensive,  0.4f,      0.1f,      0.7f,   1.3f,   1.3f,   1.2f,   1.1f   },
        { "Lightning Resist",   AffixCategory::Defensive,  0.4f,      0.1f,      0.7f,   1.3f,   1.3f,   1.2f,   1.1f   },
        { "Magic Resist",       AffixCategory::Defensive,  0.4f,      0.1f,      0.7f,   1.3f,   1.3f,   1.2f,   1.1f   },
        { "All Resist",         AffixCategory::Defensive,  0.6f,      0.15f,     0.8f,   1.4f,   1.4f,   1.3f,   1.2f   },
        { "Life",               AffixCategory::Utility,    0.4f,      0.1f,      0.9f,   1.1f,   1.1f,   1.0f,   1.0f   },
        { "Mana",               AffixCategory::Utility,    0.4f,      0.1f,      0.9f,   0.9f,   0.9f,   1.0f,   1.2f   },
        { "Durability",         AffixCategory::Utility,    0.2f,      0.01f,     1.0f,   1.0f,   1.0f,   1.0f,   0.0f   }
    };
    
    // Set default scoring formula parameters
    config.levelFactorBase = 1.0f;
    config.levelFactorMultiplier = 0.02f;
    config.levelFactorMax = 2.0f;
    config.uniqueItemBonus = 1.2f;
    config.setItemBonus = 1.1f;
    
    // Set default gear level calculation parameters
    config.characterLevelWeight = 0.5f;
    config.gearScoreWeight = 1.0f;
    
    // Set default normalization parameters
    config.minGearLevel = 1.0f;
    config.maxGearLevel = 100.0f;
    config.rawScoreMin = 1.0f;
    config.rawScoreMax = 50.0f;
    
    return config;
}

} // namespace devilution
