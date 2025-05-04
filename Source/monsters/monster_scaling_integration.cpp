/**
 * @file monster_scaling_integration.cpp
 *
 * Implementation of the integration between the monster stat scaling system and the game's monster creation and combat systems.
 */
#include "monsters/monster_scaling_integration.h"

#include <algorithm>
#include <cmath>

#include "utils/log.hpp"

namespace devilution {

MonsterScalingIntegration& MonsterScalingIntegration::GetInstance()
{
    static MonsterScalingIntegration instance;
    return instance;
}

void MonsterScalingIntegration::Initialize()
{
    if (initialized) {
        return;
    }
    
    // Initialize the monster stat scaler
    MonsterStatScaler::GetInstance().Initialize();
    
    // Initialize the gear level manager
    GearLevelManager::GetInstance().Initialize();
    
    // Clear any existing data
    monsterScalingLevels.clear();
    
    initialized = true;
    LogVerbose("Monster Scaling Integration initialized");
}

void MonsterScalingIntegration::ApplyScalingToMonster(MonsterData &monster, const Player &player)
{
    // Skip scaling if disabled
    if (!scalingEnabled) {
        return;
    }
    
    // Get the player's gear level
    float gearLevel = GetPlayerGearLevel(player);
    
    // Store the gear level used for this monster
    monsterScalingLevels[monster.uniqueId] = gearLevel;
    
    // Get the monster stat scaler
    const MonsterStatScaler &scaler = MonsterStatScaler::GetInstance();
    
    // Scale the monster's stats
    monster.hitPoints = scaler.ScaleHitPoints(monster, gearLevel);
    monster.maxHitPoints = monster.hitPoints;
    monster.damageMin = scaler.ScaleDamage(monster, gearLevel, true);
    monster.damageMax = scaler.ScaleDamage(monster, gearLevel, false);
    monster.armorClass = scaler.ScaleArmorClass(monster, gearLevel);
    monster.toHitChance = scaler.ScaleToHitChance(monster, gearLevel);
    
    // Log the scaling
    LogVerbose("Applied scaling to monster {} ({}): HP={}, DMG={}-{}, AC={}, ToHit={} (Gear Level: {})",
        monster.uniqueId, monster.name, monster.hitPoints, monster.damageMin, monster.damageMax,
        monster.armorClass, monster.toHitChance, gearLevel);
}

int MonsterScalingIntegration::ScaleMonsterDamage(const MonsterData &monster, const Player &player, int damage)
{
    // Skip scaling if disabled
    if (!scalingEnabled) {
        return damage;
    }
    
    // Get the gear level used for this monster
    float gearLevel = GetMonsterScalingGearLevel(monster);
    
    // If no gear level is stored, use the player's current gear level
    if (gearLevel <= 0.0f) {
        gearLevel = GetPlayerGearLevel(player);
    }
    
    // Get the monster stat scaler
    const MonsterStatScaler &scaler = MonsterStatScaler::GetInstance();
    
    // Create a temporary monster with the base damage
    MonsterData tempMonster = monster;
    tempMonster.damageMin = damage;
    tempMonster.damageMax = damage;
    
    // Scale the damage
    int scaledDamage = scaler.ScaleDamage(tempMonster, gearLevel, true);
    
    return scaledDamage;
}

int MonsterScalingIntegration::ScaleMonsterToHit(const MonsterData &monster, const Player &player, int baseToHit)
{
    // Skip scaling if disabled
    if (!scalingEnabled) {
        return baseToHit;
    }
    
    // Get the gear level used for this monster
    float gearLevel = GetMonsterScalingGearLevel(monster);
    
    // If no gear level is stored, use the player's current gear level
    if (gearLevel <= 0.0f) {
        gearLevel = GetPlayerGearLevel(player);
    }
    
    // Get the monster stat scaler
    const MonsterStatScaler &scaler = MonsterStatScaler::GetInstance();
    
    // Create a temporary monster with the base to-hit chance
    MonsterData tempMonster = monster;
    tempMonster.toHitChance = baseToHit;
    
    // Scale the to-hit chance
    int scaledToHit = scaler.ScaleToHitChance(tempMonster, gearLevel);
    
    return scaledToHit;
}

float MonsterScalingIntegration::ScaleMonsterSpecialEffectiveness(const MonsterData &monster, const Player &player, float baseEffectiveness)
{
    // Skip scaling if disabled
    if (!scalingEnabled) {
        return baseEffectiveness;
    }
    
    // Get the gear level used for this monster
    float gearLevel = GetMonsterScalingGearLevel(monster);
    
    // If no gear level is stored, use the player's current gear level
    if (gearLevel <= 0.0f) {
        gearLevel = GetPlayerGearLevel(player);
    }
    
    // Get the monster stat scaler
    const MonsterStatScaler &scaler = MonsterStatScaler::GetInstance();
    
    // Create a temporary monster with the base special effectiveness
    MonsterData tempMonster = monster;
    
    // Scale the special effectiveness
    float scaledEffectiveness = scaler.ScaleSpecialEffectiveness(tempMonster, gearLevel) * baseEffectiveness;
    
    return scaledEffectiveness;
}

float MonsterScalingIntegration::GetMonsterScalingGearLevel(const MonsterData &monster)
{
    // Try to find the stored gear level
    auto it = monsterScalingLevels.find(monster.uniqueId);
    if (it != monsterScalingLevels.end()) {
        return it->second;
    }
    
    // If not found, return 0
    return 0.0f;
}

void MonsterScalingIntegration::SetScalingEnabled(bool enabled)
{
    scalingEnabled = enabled;
    LogVerbose("Monster scaling {}", enabled ? "enabled" : "disabled");
}

bool MonsterScalingIntegration::IsScalingEnabled() const
{
    return scalingEnabled;
}

float MonsterScalingIntegration::GetPlayerGearLevel(const Player &player)
{
    // Get the gear level manager
    const GearLevelManager &manager = GearLevelManager::GetInstance();
    
    // Get the player's gear level
    return manager.GetCurrentGearLevel(player);
}

} // namespace devilution
