/**
 * @file monster_scaling_debug.h
 *
 * Debug utilities for the monster stat scaling system.
 */
#pragma once

#include <string>
#include <vector>

#include "monsters.h"
#include "player.h"

namespace devilution {

/**
 * @brief Structure for monster stat comparison
 */
struct MonsterStatComparison {
    std::string statName;
    float baseValue;
    float scaledValue;
    float percentChange;
};

/**
 * @brief Class for debugging and visualizing monster scaling
 */
class MonsterScalingDebug {
public:
    /**
     * @brief Gets the singleton instance
     * @return Reference to the singleton instance
     */
    static MonsterScalingDebug& GetInstance();
    
    /**
     * @brief Initializes the debug utilities
     */
    void Initialize();
    
    /**
     * @brief Generates a scaling report for a monster at different gear levels
     * @param monster The monster to generate the report for
     * @param minGearLevel The minimum gear level to test
     * @param maxGearLevel The maximum gear level to test
     * @param step The step size between gear levels
     * @return A text report of the monster's scaling
     */
    std::string GenerateScalingReport(const MonsterData &monster, float minGearLevel, float maxGearLevel, float step);
    
    /**
     * @brief Compares a monster's base stats to its scaled stats
     * @param monster The monster to compare
     * @param gearLevel The gear level to scale to
     * @return A vector of stat comparisons
     */
    std::vector<MonsterStatComparison> CompareBaseToScaled(const MonsterData &monster, float gearLevel);
    
    /**
     * @brief Simulates combat between a player and a monster
     * @param player The player
     * @param monster The monster
     * @param numRounds The number of rounds to simulate
     * @return A text report of the simulated combat
     */
    std::string SimulateCombat(const Player &player, const MonsterData &monster, int numRounds);
    
    /**
     * @brief Generates a scaling visualization for a specific stat
     * @param monster The monster to visualize
     * @param statName The name of the stat to visualize
     * @param minGearLevel The minimum gear level to test
     * @param maxGearLevel The maximum gear level to test
     * @param step The step size between gear levels
     * @return A text visualization of the stat's scaling
     */
    std::string VisualizeStatScaling(const MonsterData &monster, const std::string &statName, float minGearLevel, float maxGearLevel, float step);
    
private:
    MonsterScalingDebug() = default;
    ~MonsterScalingDebug() = default;
    
    MonsterScalingDebug(const MonsterScalingDebug&) = delete;
    MonsterScalingDebug& operator=(const MonsterScalingDebug&) = delete;
    
    /**
     * @brief Gets a monster's stat value
     * @param monster The monster to get the stat from
     * @param statName The name of the stat to get
     * @return The value of the stat
     */
    float GetMonsterStat(const MonsterData &monster, const std::string &statName);
    
    /**
     * @brief Sets a monster's stat value
     * @param monster The monster to set the stat on
     * @param statName The name of the stat to set
     * @param value The value to set
     */
    void SetMonsterStat(MonsterData &monster, const std::string &statName, float value);
    
    // Flag to track initialization
    bool initialized = false;
};

} // namespace devilution
