/**
 * @file difficulty_integration.h
 *
 * Integration of the difficulty transition system with the monster scaling system.
 */
#pragma once

#include "player.h"
#include "monsters.h"
#include "difficulty/difficulty_manager.h"
#include "monsters/monster_scaling_integration.h"

namespace devilution {

/**
 * @brief Class for integrating difficulty transitions with monster scaling
 */
class DifficultyIntegration {
public:
    /**
     * @brief Gets the singleton instance
     * @return Reference to the singleton instance
     */
    static DifficultyIntegration& GetInstance();
    
    /**
     * @brief Initializes the difficulty integration
     */
    void Initialize();
    
    /**
     * @brief Updates the difficulty for all players
     * @param gameTime The current game time
     */
    void UpdateAllPlayerDifficulties(float gameTime);
    
    /**
     * @brief Scales a monster based on the smoothed difficulty
     * @param monster The monster to scale
     * @param player The player to scale against
     */
    void ScaleMonsterWithSmoothedDifficulty(MonsterData &monster, const Player &player);
    
    /**
     * @brief Handles a player equipping a new item
     * @param player The player who equipped the item
     * @param oldGearLevel The player's old gear level
     * @param newGearLevel The player's new gear level
     * @param gameTime The current game time
     */
    void OnPlayerEquipItem(Player &player, float oldGearLevel, float newGearLevel, float gameTime);
    
    /**
     * @brief Gets a text explanation of the integrated difficulty
     * @param player The player to explain
     * @return A text explanation of the integrated difficulty
     */
    std::string GetIntegratedDifficultyExplanation(const Player &player);
    
private:
    DifficultyIntegration() = default;
    ~DifficultyIntegration() = default;
    
    DifficultyIntegration(const DifficultyIntegration&) = delete;
    DifficultyIntegration& operator=(const DifficultyIntegration&) = delete;
    
    /**
     * @brief Determines if an item upgrade is significant
     * @param oldGearLevel The old gear level
     * @param newGearLevel The new gear level
     * @return True if the upgrade is significant, false otherwise
     */
    bool IsSignificantUpgrade(float oldGearLevel, float newGearLevel);
    
    // Flag to track initialization
    bool initialized = false;
    
    // Callback ID for gear level changes
    uint32_t gearLevelCallbackId = 0;
};

} // namespace devilution
