/**
 * @file monster_scaling_integration.h
 *
 * Integration of the monster stat scaling system with the game's monster creation and combat systems.
 */
#pragma once

#include "monsters.h"
#include "player.h"
#include "monsters/monster_scaling.h"
#include "gear/gear_manager.h"

namespace devilution {

/**
 * @brief Class for integrating monster scaling with the game
 */
class MonsterScalingIntegration {
public:
    /**
     * @brief Gets the singleton instance
     * @return Reference to the singleton instance
     */
    static MonsterScalingIntegration& GetInstance();
    
    /**
     * @brief Initializes the monster scaling integration
     */
    void Initialize();
    
    /**
     * @brief Applies scaling to a monster based on the player's gear level
     * @param monster The monster to scale
     * @param player The player to scale against
     */
    void ApplyScalingToMonster(MonsterData &monster, const Player &player);
    
    /**
     * @brief Applies scaling to a monster's damage
     * @param monster The monster doing damage
     * @param player The player being damaged
     * @param damage The base damage
     * @return The scaled damage
     */
    int ScaleMonsterDamage(const MonsterData &monster, const Player &player, int damage);
    
    /**
     * @brief Applies scaling to a monster's to-hit chance
     * @param monster The monster attacking
     * @param player The player being attacked
     * @param baseToHit The base to-hit chance
     * @return The scaled to-hit chance
     */
    int ScaleMonsterToHit(const MonsterData &monster, const Player &player, int baseToHit);
    
    /**
     * @brief Applies scaling to a monster's special ability effectiveness
     * @param monster The monster using the special ability
     * @param player The player affected by the special ability
     * @param baseEffectiveness The base effectiveness
     * @return The scaled effectiveness
     */
    float ScaleMonsterSpecialEffectiveness(const MonsterData &monster, const Player &player, float baseEffectiveness);
    
    /**
     * @brief Gets the gear level used for scaling a monster
     * @param monster The monster to get the gear level for
     * @return The gear level used for scaling
     */
    float GetMonsterScalingGearLevel(const MonsterData &monster);
    
    /**
     * @brief Sets whether monster scaling is enabled
     * @param enabled True to enable scaling, false to disable
     */
    void SetScalingEnabled(bool enabled);
    
    /**
     * @brief Checks if monster scaling is enabled
     * @return True if scaling is enabled, false otherwise
     */
    bool IsScalingEnabled() const;
    
private:
    MonsterScalingIntegration() = default;
    ~MonsterScalingIntegration() = default;
    
    MonsterScalingIntegration(const MonsterScalingIntegration&) = delete;
    MonsterScalingIntegration& operator=(const MonsterScalingIntegration&) = delete;
    
    /**
     * @brief Gets the player's gear level
     * @param player The player to get the gear level for
     * @return The player's gear level
     */
    float GetPlayerGearLevel(const Player &player);
    
    // Flag to track initialization
    bool initialized = false;
    
    // Flag to enable/disable scaling
    bool scalingEnabled = true;
    
    // Map of monster IDs to gear levels used for scaling
    std::unordered_map<uint32_t, float> monsterScalingLevels;
};

} // namespace devilution
