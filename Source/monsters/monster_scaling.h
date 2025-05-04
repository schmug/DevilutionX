/**
 * @file monster_scaling.h
 *
 * Interface for the monster stat scaling system that adjusts monster statistics based on player gear level.
 */
#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

#include "monsters.h"
#include "player.h"

namespace devilution {

/**
 * @brief Enum for different scaling curve types
 */
enum class ScalingCurveType {
    Linear,      // Linear scaling: base + (factor * gearLevel)
    Exponential, // Exponential scaling: base * (1 + factor * gearLevel)
    Logarithmic, // Logarithmic scaling: base * (1 + factor * log(gearLevel))
    Sigmoid,     // Sigmoid scaling: base * (1 + factor * (2 / (1 + exp(-gearLevel * 0.1)) - 1))
    Custom       // Custom scaling function
};

/**
 * @brief Structure for stat scaling parameters
 */
struct StatScalingParams {
    float scalingFactor;    // Factor to multiply gear level by
    float minValue;         // Minimum value for the stat
    float maxValue;         // Maximum value for the stat
    ScalingCurveType curve; // Type of scaling curve to use
    
    // Default constructor
    StatScalingParams()
        : scalingFactor(0.1f), minValue(0.0f), maxValue(1000.0f), curve(ScalingCurveType::Linear)
    {
    }
    
    // Constructor with parameters
    StatScalingParams(float factor, float min, float max, ScalingCurveType curveType)
        : scalingFactor(factor), minValue(min), maxValue(max), curve(curveType)
    {
    }
};

/**
 * @brief Structure for monster stat definition
 */
struct MonsterStatDefinition {
    // Base stats
    int baseHitPoints;
    int baseDamageMin;
    int baseDamageMax;
    int baseArmorClass;
    int baseToHitChance;
    float baseSpecialEffectiveness;
    
    // Scaling parameters
    StatScalingParams hitPointsScaling;
    StatScalingParams damageScaling;
    StatScalingParams armorClassScaling;
    StatScalingParams toHitChanceScaling;
    StatScalingParams specialEffectivenessScaling;
    
    // Monster type and level
    monster_id type;
    int level;
    
    // Constructor
    MonsterStatDefinition()
        : baseHitPoints(0), baseDamageMin(0), baseDamageMax(0), baseArmorClass(0),
          baseToHitChance(0), baseSpecialEffectiveness(1.0f), type(static_cast<monster_id>(0)), level(1)
    {
    }
};

/**
 * @brief Type for custom scaling functions
 */
using CustomScalingFunction = std::function<float(float base, float gearLevel, const StatScalingParams &params)>;

/**
 * @brief Class for monster stat scaling
 */
class MonsterStatScaler {
public:
    /**
     * @brief Gets the singleton instance
     * @return Reference to the singleton instance
     */
    static MonsterStatScaler& GetInstance();
    
    /**
     * @brief Initializes the monster stat scaler
     */
    void Initialize();
    
    /**
     * @brief Loads monster stat definitions from file
     * @param filePath Path to the monster stat definition file
     * @return True if loading was successful, false otherwise
     */
    bool LoadMonsterStatDefinitions(const std::string &filePath);
    
    /**
     * @brief Gets the monster stat definition for a monster type
     * @param type The monster type
     * @return The monster stat definition
     */
    const MonsterStatDefinition& GetMonsterStatDefinition(monster_id type) const;
    
    /**
     * @brief Scales a monster's hit points based on gear level
     * @param monster The monster to scale
     * @param gearLevel The player's gear level
     * @return The scaled hit points
     */
    int ScaleHitPoints(const MonsterData &monster, float gearLevel) const;
    
    /**
     * @brief Scales a monster's damage based on gear level
     * @param monster The monster to scale
     * @param gearLevel The player's gear level
     * @param isMinDamage True to scale minimum damage, false for maximum
     * @return The scaled damage
     */
    int ScaleDamage(const MonsterData &monster, float gearLevel, bool isMinDamage) const;
    
    /**
     * @brief Scales a monster's armor class based on gear level
     * @param monster The monster to scale
     * @param gearLevel The player's gear level
     * @return The scaled armor class
     */
    int ScaleArmorClass(const MonsterData &monster, float gearLevel) const;
    
    /**
     * @brief Scales a monster's to-hit chance based on gear level
     * @param monster The monster to scale
     * @param gearLevel The player's gear level
     * @return The scaled to-hit chance
     */
    int ScaleToHitChance(const MonsterData &monster, float gearLevel) const;
    
    /**
     * @brief Scales a monster's special ability effectiveness based on gear level
     * @param monster The monster to scale
     * @param gearLevel The player's gear level
     * @return The scaled special ability effectiveness
     */
    float ScaleSpecialEffectiveness(const MonsterData &monster, float gearLevel) const;
    
    /**
     * @brief Registers a custom scaling function
     * @param curveType The curve type to register the function for
     * @param function The function to register
     */
    void RegisterCustomScalingFunction(ScalingCurveType curveType, CustomScalingFunction function);
    
    /**
     * @brief Gets a text explanation of monster scaling
     * @param monster The monster to explain
     * @param gearLevel The player's gear level
     * @return A text explanation of the monster scaling
     */
    std::string GetMonsterScalingExplanation(const MonsterData &monster, float gearLevel) const;
    
private:
    MonsterStatScaler() = default;
    ~MonsterStatScaler() = default;
    
    MonsterStatScaler(const MonsterStatScaler&) = delete;
    MonsterStatScaler& operator=(const MonsterStatScaler&) = delete;
    
    /**
     * @brief Applies scaling to a stat
     * @param baseStat The base stat value
     * @param gearLevel The player's gear level
     * @param params The scaling parameters
     * @return The scaled stat value
     */
    float ApplyScaling(float baseStat, float gearLevel, const StatScalingParams &params) const;
    
    /**
     * @brief Applies linear scaling to a stat
     * @param baseStat The base stat value
     * @param gearLevel The player's gear level
     * @param params The scaling parameters
     * @return The scaled stat value
     */
    float ApplyLinearScaling(float baseStat, float gearLevel, const StatScalingParams &params) const;
    
    /**
     * @brief Applies exponential scaling to a stat
     * @param baseStat The base stat value
     * @param gearLevel The player's gear level
     * @param params The scaling parameters
     * @return The scaled stat value
     */
    float ApplyExponentialScaling(float baseStat, float gearLevel, const StatScalingParams &params) const;
    
    /**
     * @brief Applies logarithmic scaling to a stat
     * @param baseStat The base stat value
     * @param gearLevel The player's gear level
     * @param params The scaling parameters
     * @return The scaled stat value
     */
    float ApplyLogarithmicScaling(float baseStat, float gearLevel, const StatScalingParams &params) const;
    
    /**
     * @brief Applies sigmoid scaling to a stat
     * @param baseStat The base stat value
     * @param gearLevel The player's gear level
     * @param params The scaling parameters
     * @return The scaled stat value
     */
    float ApplySigmoidScaling(float baseStat, float gearLevel, const StatScalingParams &params) const;
    
    // Monster stat definitions by monster type
    std::unordered_map<monster_id, MonsterStatDefinition> monsterStatDefinitions;
    
    // Custom scaling functions by curve type
    std::unordered_map<ScalingCurveType, CustomScalingFunction> customScalingFunctions;
    
    // Flag to track initialization
    bool initialized = false;
};

} // namespace devilution
