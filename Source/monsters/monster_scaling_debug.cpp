/**
 * @file monster_scaling_debug.cpp
 *
 * Implementation of debug utilities for the monster stat scaling system.
 */
#include "monsters/monster_scaling_debug.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <random>
#include <sstream>

#include "utils/log.hpp"
#include "monsters/monster_scaling.h"
#include "monsters/monster_scaling_integration.h"

namespace devilution {

MonsterScalingDebug& MonsterScalingDebug::GetInstance()
{
    static MonsterScalingDebug instance;
    return instance;
}

void MonsterScalingDebug::Initialize()
{
    if (initialized) {
        return;
    }
    
    // Initialize dependencies
    MonsterStatScaler::GetInstance().Initialize();
    MonsterScalingIntegration::GetInstance().Initialize();
    
    initialized = true;
    LogVerbose("Monster Scaling Debug utilities initialized");
}

std::string MonsterScalingDebug::GenerateScalingReport(const MonsterData &monster, float minGearLevel, float maxGearLevel, float step)
{
    std::stringstream report;
    report << "Monster Scaling Report for " << monster.name << "\n";
    report << "==================================\n\n";
    
    // Table header
    report << std::setw(12) << "Gear Level" << " | "
           << std::setw(10) << "HP" << " | "
           << std::setw(10) << "Damage" << " | "
           << std::setw(10) << "AC" << " | "
           << std::setw(10) << "To-Hit" << " | "
           << std::setw(10) << "Special" << "\n";
    report << std::string(12, '-') << "-+-"
           << std::string(10, '-') << "-+-"
           << std::string(10, '-') << "-+-"
           << std::string(10, '-') << "-+-"
           << std::string(10, '-') << "-+-"
           << std::string(10, '-') << "\n";
    
    // Get the monster stat scaler
    const MonsterStatScaler &scaler = MonsterStatScaler::GetInstance();
    
    // Generate the report for each gear level
    for (float gearLevel = minGearLevel; gearLevel <= maxGearLevel; gearLevel += step) {
        // Scale the monster's stats
        int hp = scaler.ScaleHitPoints(monster, gearLevel);
        int damageMin = scaler.ScaleDamage(monster, gearLevel, true);
        int damageMax = scaler.ScaleDamage(monster, gearLevel, false);
        int ac = scaler.ScaleArmorClass(monster, gearLevel);
        int toHit = scaler.ScaleToHitChance(monster, gearLevel);
        float special = scaler.ScaleSpecialEffectiveness(monster, gearLevel);
        
        // Format the damage as a range
        std::stringstream damageStr;
        damageStr << damageMin << "-" << damageMax;
        
        // Add the row to the table
        report << std::setw(12) << std::fixed << std::setprecision(1) << gearLevel << " | "
               << std::setw(10) << hp << " | "
               << std::setw(10) << damageStr.str() << " | "
               << std::setw(10) << ac << " | "
               << std::setw(10) << toHit << " | "
               << std::setw(10) << std::fixed << std::setprecision(2) << special << "\n";
    }
    
    report << "\nScaling Parameters:\n";
    report << "==================================\n";
    
    // Get the monster definition
    const MonsterStatDefinition &def = scaler.GetMonsterStatDefinition(monster.type);
    
    // Add scaling parameters for each stat
    report << "Hit Points: Factor=" << def.hitPointsScaling.scalingFactor
           << ", Min=" << def.hitPointsScaling.minValue
           << ", Max=" << def.hitPointsScaling.maxValue
           << ", Curve=";
    switch (def.hitPointsScaling.curve) {
    case ScalingCurveType::Linear:
        report << "Linear\n";
        break;
    case ScalingCurveType::Exponential:
        report << "Exponential\n";
        break;
    case ScalingCurveType::Logarithmic:
        report << "Logarithmic\n";
        break;
    case ScalingCurveType::Sigmoid:
        report << "Sigmoid\n";
        break;
    case ScalingCurveType::Custom:
        report << "Custom\n";
        break;
    }
    
    report << "Damage: Factor=" << def.damageScaling.scalingFactor
           << ", Min=" << def.damageScaling.minValue
           << ", Max=" << def.damageScaling.maxValue
           << ", Curve=";
    switch (def.damageScaling.curve) {
    case ScalingCurveType::Linear:
        report << "Linear\n";
        break;
    case ScalingCurveType::Exponential:
        report << "Exponential\n";
        break;
    case ScalingCurveType::Logarithmic:
        report << "Logarithmic\n";
        break;
    case ScalingCurveType::Sigmoid:
        report << "Sigmoid\n";
        break;
    case ScalingCurveType::Custom:
        report << "Custom\n";
        break;
    }
    
    report << "Armor Class: Factor=" << def.armorClassScaling.scalingFactor
           << ", Min=" << def.armorClassScaling.minValue
           << ", Max=" << def.armorClassScaling.maxValue
           << ", Curve=";
    switch (def.armorClassScaling.curve) {
    case ScalingCurveType::Linear:
        report << "Linear\n";
        break;
    case ScalingCurveType::Exponential:
        report << "Exponential\n";
        break;
    case ScalingCurveType::Logarithmic:
        report << "Logarithmic\n";
        break;
    case ScalingCurveType::Sigmoid:
        report << "Sigmoid\n";
        break;
    case ScalingCurveType::Custom:
        report << "Custom\n";
        break;
    }
    
    report << "To-Hit Chance: Factor=" << def.toHitChanceScaling.scalingFactor
           << ", Min=" << def.toHitChanceScaling.minValue
           << ", Max=" << def.toHitChanceScaling.maxValue
           << ", Curve=";
    switch (def.toHitChanceScaling.curve) {
    case ScalingCurveType::Linear:
        report << "Linear\n";
        break;
    case ScalingCurveType::Exponential:
        report << "Exponential\n";
        break;
    case ScalingCurveType::Logarithmic:
        report << "Logarithmic\n";
        break;
    case ScalingCurveType::Sigmoid:
        report << "Sigmoid\n";
        break;
    case ScalingCurveType::Custom:
        report << "Custom\n";
        break;
    }
    
    report << "Special Effectiveness: Factor=" << def.specialEffectivenessScaling.scalingFactor
           << ", Min=" << def.specialEffectivenessScaling.minValue
           << ", Max=" << def.specialEffectivenessScaling.maxValue
           << ", Curve=";
    switch (def.specialEffectivenessScaling.curve) {
    case ScalingCurveType::Linear:
        report << "Linear\n";
        break;
    case ScalingCurveType::Exponential:
        report << "Exponential\n";
        break;
    case ScalingCurveType::Logarithmic:
        report << "Logarithmic\n";
        break;
    case ScalingCurveType::Sigmoid:
        report << "Sigmoid\n";
        break;
    case ScalingCurveType::Custom:
        report << "Custom\n";
        break;
    }
    
    return report.str();
}

std::vector<MonsterStatComparison> MonsterScalingDebug::CompareBaseToScaled(const MonsterData &monster, float gearLevel)
{
    // Use std::vector instead of StaticVector for more robust memory handling
    std::vector<MonsterStatComparison> comparisons;
    // Reserve space for all stats we'll compare (6 stats total)
    comparisons.reserve(6);
    
    // Get the monster stat scaler
    const MonsterStatScaler &scaler = MonsterStatScaler::GetInstance();
    
    // Compare hit points
    MonsterStatComparison hpComparison;
    hpComparison.statName = "Hit Points";
    hpComparison.baseValue = static_cast<float>(monster.hitPoints);
    hpComparison.scaledValue = static_cast<float>(scaler.ScaleHitPoints(monster, gearLevel));
    hpComparison.percentChange = (hpComparison.scaledValue / hpComparison.baseValue - 1.0f) * 100.0f;
    comparisons.emplace_back(hpComparison);
    
    // Compare minimum damage
    MonsterStatComparison damageMinComparison;
    damageMinComparison.statName = "Damage (Min)";
    damageMinComparison.baseValue = static_cast<float>(monster.damageMin);
    damageMinComparison.scaledValue = static_cast<float>(scaler.ScaleDamage(monster, gearLevel, true));
    damageMinComparison.percentChange = (damageMinComparison.scaledValue / damageMinComparison.baseValue - 1.0f) * 100.0f;
    comparisons.emplace_back(damageMinComparison);
    
    // Compare maximum damage
    MonsterStatComparison damageMaxComparison;
    damageMaxComparison.statName = "Damage (Max)";
    damageMaxComparison.baseValue = static_cast<float>(monster.damageMax);
    damageMaxComparison.scaledValue = static_cast<float>(scaler.ScaleDamage(monster, gearLevel, false));
    damageMaxComparison.percentChange = (damageMaxComparison.scaledValue / damageMaxComparison.baseValue - 1.0f) * 100.0f;
    comparisons.emplace_back(damageMaxComparison);
    
    // Compare armor class
    MonsterStatComparison acComparison;
    acComparison.statName = "Armor Class";
    acComparison.baseValue = static_cast<float>(monster.armorClass);
    acComparison.scaledValue = static_cast<float>(scaler.ScaleArmorClass(monster, gearLevel));
    acComparison.percentChange = (acComparison.scaledValue / acComparison.baseValue - 1.0f) * 100.0f;
    comparisons.emplace_back(acComparison);
    
    // Compare to-hit chance
    MonsterStatComparison toHitComparison;
    toHitComparison.statName = "To-Hit Chance";
    toHitComparison.baseValue = static_cast<float>(monster.toHitChance);
    toHitComparison.scaledValue = static_cast<float>(scaler.ScaleToHitChance(monster, gearLevel));
    toHitComparison.percentChange = (toHitComparison.scaledValue / toHitComparison.baseValue - 1.0f) * 100.0f;
    comparisons.emplace_back(toHitComparison);
    
    // Compare special effectiveness
    MonsterStatComparison specialComparison;
    specialComparison.statName = "Special Effectiveness";
    specialComparison.baseValue = 1.0f;
    specialComparison.scaledValue = scaler.ScaleSpecialEffectiveness(monster, gearLevel);
    specialComparison.percentChange = (specialComparison.scaledValue / specialComparison.baseValue - 1.0f) * 100.0f;
    comparisons.emplace_back(specialComparison);
    
    return comparisons;
}

std::string MonsterScalingDebug::SimulateCombat(const Player &player, const MonsterData &monster, int numRounds)
{
    std::stringstream report;
    report << "Combat Simulation: " << player._pName << " vs. " << monster.name << "\n";
    report << "==================================\n\n";
    
    // Get the monster stat scaler
    const MonsterStatScaler &scaler = MonsterStatScaler::GetInstance();
    
    // Get the player's gear level
    float gearLevel = MonsterScalingIntegration::GetInstance().GetPlayerGearLevel(player);
    
    // Scale the monster's stats
    int monsterHp = scaler.ScaleHitPoints(monster, gearLevel);
    int monsterDamageMin = scaler.ScaleDamage(monster, gearLevel, true);
    int monsterDamageMax = scaler.ScaleDamage(monster, gearLevel, false);
    int monsterAc = scaler.ScaleArmorClass(monster, gearLevel);
    int monsterToHit = scaler.ScaleToHitChance(monster, gearLevel);
    
    // Get player stats
    int playerHp = player._pHitPoints >> 6; // Convert from internal format
    int playerDamageMin = player._pIMinDam;
    int playerDamageMax = player._pIMaxDam;
    int playerAc = player._pIAC;
    int playerToHit = player._pIBonusToHit;
    
    // Display initial stats
    report << "Initial Stats:\n";
    report << "Player: HP=" << playerHp << ", DMG=" << playerDamageMin << "-" << playerDamageMax
           << ", AC=" << playerAc << ", ToHit=" << playerToHit << "\n";
    report << "Monster: HP=" << monsterHp << ", DMG=" << monsterDamageMin << "-" << monsterDamageMax
           << ", AC=" << monsterAc << ", ToHit=" << monsterToHit << "\n\n";
    
    // Set up random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> playerDamageDist(playerDamageMin, playerDamageMax);
    std::uniform_int_distribution<> monsterDamageDist(monsterDamageMin, monsterDamageMax);
    std::uniform_int_distribution<> hitRollDist(1, 100);
    
    // Simulate combat
    int currentPlayerHp = playerHp;
    int currentMonsterHp = monsterHp;
    int round = 1;
    
    while (round <= numRounds && currentPlayerHp > 0 && currentMonsterHp > 0) {
        report << "Round " << round << ":\n";
        
        // Player attacks monster
        int playerHitRoll = hitRollDist(gen);
        bool playerHit = playerHitRoll <= (playerToHit - monsterAc + 50); // Simplified hit calculation
        
        if (playerHit) {
            int playerDamage = playerDamageDist(gen);
            currentMonsterHp -= playerDamage;
            report << "Player hits for " << playerDamage << " damage. Monster HP: " << currentMonsterHp << "\n";
        } else {
            report << "Player misses. Monster HP: " << currentMonsterHp << "\n";
        }
        
        // Check if monster is defeated
        if (currentMonsterHp <= 0) {
            report << "Monster defeated!\n";
            break;
        }
        
        // Monster attacks player
        int monsterHitRoll = hitRollDist(gen);
        bool monsterHit = monsterHitRoll <= (monsterToHit - playerAc + 50); // Simplified hit calculation
        
        if (monsterHit) {
            int monsterDamage = monsterDamageDist(gen);
            currentPlayerHp -= monsterDamage;
            report << "Monster hits for " << monsterDamage << " damage. Player HP: " << currentPlayerHp << "\n";
        } else {
            report << "Monster misses. Player HP: " << currentPlayerHp << "\n";
        }
        
        // Check if player is defeated
        if (currentPlayerHp <= 0) {
            report << "Player defeated!\n";
            break;
        }
        
        round++;
        report << "\n";
    }
    
    // Combat summary
    report << "\nCombat Summary:\n";
    report << "Rounds: " << round << "\n";
    report << "Player HP: " << currentPlayerHp << "/" << playerHp << " (" << (currentPlayerHp * 100 / playerHp) << "%)\n";
    report << "Monster HP: " << currentMonsterHp << "/" << monsterHp << " (" << (currentMonsterHp * 100 / monsterHp) << "%)\n";
    
    if (currentPlayerHp <= 0) {
        report << "Result: Monster wins\n";
    } else if (currentMonsterHp <= 0) {
        report << "Result: Player wins\n";
    } else {
        report << "Result: Draw (reached round limit)\n";
    }
    
    return report.str();
}

std::string MonsterScalingDebug::VisualizeStatScaling(const MonsterData &monster, const std::string &statName, float minGearLevel, float maxGearLevel, float step)
{
    std::stringstream visualization;
    visualization << "Scaling Visualization for " << monster.name << " - " << statName << "\n";
    visualization << "==================================\n\n";
    
    // Get the monster stat scaler
    const MonsterStatScaler &scaler = MonsterStatScaler::GetInstance();
    
    // Get the base stat value
    float baseStat = GetMonsterStat(monster, statName);
    
    // Table header
    visualization << std::setw(12) << "Gear Level" << " | "
                  << std::setw(10) << "Value" << " | "
                  << std::setw(10) << "% Change" << " | "
                  << "Graph\n";
    visualization << std::string(12, '-') << "-+-"
                  << std::string(10, '-') << "-+-"
                  << std::string(10, '-') << "-+-"
                  << std::string(50, '-') << "\n";
    
    // Maximum value for scaling the graph
    float maxValue = 0.0f;
    
    // First pass to find the maximum value
    for (float gearLevel = minGearLevel; gearLevel <= maxGearLevel; gearLevel += step) {
        // Create a temporary monster
        MonsterData tempMonster = monster;
        
        // Scale the stat
        float scaledStat = 0.0f;
        
        if (statName == "Hit Points") {
            scaledStat = static_cast<float>(scaler.ScaleHitPoints(tempMonster, gearLevel));
        } else if (statName == "Damage (Min)") {
            scaledStat = static_cast<float>(scaler.ScaleDamage(tempMonster, gearLevel, true));
        } else if (statName == "Damage (Max)") {
            scaledStat = static_cast<float>(scaler.ScaleDamage(tempMonster, gearLevel, false));
        } else if (statName == "Armor Class") {
            scaledStat = static_cast<float>(scaler.ScaleArmorClass(tempMonster, gearLevel));
        } else if (statName == "To-Hit Chance") {
            scaledStat = static_cast<float>(scaler.ScaleToHitChance(tempMonster, gearLevel));
        } else if (statName == "Special Effectiveness") {
            scaledStat = scaler.ScaleSpecialEffectiveness(tempMonster, gearLevel);
        }
        
        maxValue = std::max(maxValue, scaledStat);
    }
    
    // Second pass to generate the visualization
    for (float gearLevel = minGearLevel; gearLevel <= maxGearLevel; gearLevel += step) {
        // Create a temporary monster
        MonsterData tempMonster = monster;
        
        // Scale the stat
        float scaledStat = 0.0f;
        
        if (statName == "Hit Points") {
            scaledStat = static_cast<float>(scaler.ScaleHitPoints(tempMonster, gearLevel));
        } else if (statName == "Damage (Min)") {
            scaledStat = static_cast<float>(scaler.ScaleDamage(tempMonster, gearLevel, true));
        } else if (statName == "Damage (Max)") {
            scaledStat = static_cast<float>(scaler.ScaleDamage(tempMonster, gearLevel, false));
        } else if (statName == "Armor Class") {
            scaledStat = static_cast<float>(scaler.ScaleArmorClass(tempMonster, gearLevel));
        } else if (statName == "To-Hit Chance") {
            scaledStat = static_cast<float>(scaler.ScaleToHitChance(tempMonster, gearLevel));
        } else if (statName == "Special Effectiveness") {
            scaledStat = scaler.ScaleSpecialEffectiveness(tempMonster, gearLevel);
        }
        
        // Calculate percent change
        float percentChange = (scaledStat / baseStat - 1.0f) * 100.0f;
        
        // Generate the graph bar
        int barLength = static_cast<int>((scaledStat / maxValue) * 50.0f);
        std::string bar = std::string(barLength, '#');
        
        // Add the row to the table
        visualization << std::setw(12) << std::fixed << std::setprecision(1) << gearLevel << " | "
                      << std::setw(10) << std::fixed << std::setprecision(1) << scaledStat << " | "
                      << std::setw(10) << std::fixed << std::setprecision(1) << percentChange << "% | "
                      << bar << "\n";
    }
    
    return visualization.str();
}

float MonsterScalingDebug::GetMonsterStat(const MonsterData &monster, const std::string &statName)
{
    if (statName == "Hit Points") {
        return static_cast<float>(monster.hitPoints);
    } else if (statName == "Damage (Min)") {
        return static_cast<float>(monster.damageMin);
    } else if (statName == "Damage (Max)") {
        return static_cast<float>(monster.damageMax);
    } else if (statName == "Armor Class") {
        return static_cast<float>(monster.armorClass);
    } else if (statName == "To-Hit Chance") {
        return static_cast<float>(monster.toHitChance);
    } else if (statName == "Special Effectiveness") {
        return 1.0f; // Base special effectiveness
    }
    
    return 0.0f;
}

void MonsterScalingDebug::SetMonsterStat(MonsterData &monster, const std::string &statName, float value)
{
    if (statName == "Hit Points") {
        monster.hitPoints = static_cast<int>(value);
        monster.maxHitPoints = monster.hitPoints;
    } else if (statName == "Damage (Min)") {
        monster.damageMin = static_cast<int>(value);
    } else if (statName == "Damage (Max)") {
        monster.damageMax = static_cast<int>(value);
    } else if (statName == "Armor Class") {
        monster.armorClass = static_cast<int>(value);
    } else if (statName == "To-Hit Chance") {
        monster.toHitChance = static_cast<int>(value);
    }
    // Special effectiveness is not directly stored in the monster
}

} // namespace devilution
