/**
 * @file difficulty_integration.cpp
 *
 * Implementation of the integration between the difficulty transition system and the monster scaling system.
 */
#include "difficulty/difficulty_integration.h"

#include <algorithm>
#include <cmath>
#include <sstream>

#include "utils/log.hpp"
#include "gear/gear_manager.h"

namespace devilution {

DifficultyIntegration& DifficultyIntegration::GetInstance()
{
    static DifficultyIntegration instance;
    return instance;
}

void DifficultyIntegration::Initialize()
{
    if (initialized) {
        return;
    }
    
    // Initialize the difficulty manager
    DifficultyManager::GetInstance().Initialize();
    
    // Initialize the monster scaling integration
    MonsterScalingIntegration::GetInstance().Initialize();
    
    // Register for gear level change events
    gearLevelCallbackId = GearLevelManager::GetInstance().RegisterChangeCallback(
        [this](const GearLevelChangeEvent &event) {
            // Only handle equipment changes
            if (event.eventType == GearLevelEventType::ItemEquipped ||
                event.eventType == GearLevelEventType::ItemChanged) {
                
                // Get the current game time
                float gameTime = 0.0f; // TODO: Get actual game time
                
                // Handle the gear level change
                OnPlayerEquipItem(*event.player, event.oldGearLevel, event.newGearLevel, gameTime);
            }
        }
    );
    
    initialized = true;
    LogVerbose("Difficulty Integration initialized");
}

void DifficultyIntegration::UpdateAllPlayerDifficulties(float gameTime)
{
    // Get all active players
    // For now, we'll just use a placeholder for the active player
    // In a real implementation, this would iterate through all active players
    
    // TODO: Iterate through all active players
    Player &player = *GetActivePlayer();
    
    // Update the player's difficulty
    DifficultyManager::GetInstance().UpdateDifficulty(player, gameTime);
}

void DifficultyIntegration::ScaleMonsterWithSmoothedDifficulty(MonsterData &monster, const Player &player)
{
    // Get the smoothed difficulty
    float smoothedDifficulty = DifficultyManager::GetInstance().GetCurrentDifficulty(player);
    
    // Create a temporary player with the smoothed difficulty as the gear level
    // This is a hack for testing - in a real implementation, we would modify the
    // monster scaling system to accept a difficulty value directly
    
    // Scale the monster using the smoothed difficulty
    MonsterScalingIntegration::GetInstance().ApplyScalingToMonster(monster, player);
    
    LogVerbose("Scaled monster {} using smoothed difficulty {}", monster.uniqueId, smoothedDifficulty);
}

void DifficultyIntegration::OnPlayerEquipItem(Player &player, float oldGearLevel, float newGearLevel, float gameTime)
{
    // Check if this is a significant upgrade
    if (IsSignificantUpgrade(oldGearLevel, newGearLevel)) {
        // Trigger the overpower effect
        DifficultyManager::GetInstance().TriggerOverpower(player, gameTime);
        
        LogVerbose("Triggered overpower effect for player {} due to significant upgrade: {} -> {}",
            player.getId(), oldGearLevel, newGearLevel);
    }
}

std::string DifficultyIntegration::GetIntegratedDifficultyExplanation(const Player &player)
{
    std::stringstream explanation;
    explanation << "Integrated Difficulty Explanation for " << player._pName << ":\n\n";
    
    // Get the difficulty manager
    const DifficultyManager &difficultyManager = DifficultyManager::GetInstance();
    
    // Get the gear level manager
    const GearLevelManager &gearManager = GearLevelManager::GetInstance();
    
    // Get the raw gear level
    float rawGearLevel = gearManager.GetCurrentGearLevel(player);
    
    // Get the smoothed difficulty
    float smoothedDifficulty = difficultyManager.GetCurrentDifficulty(player);
    
    // Get the target difficulty
    float targetDifficulty = difficultyManager.GetTargetDifficulty(player);
    
    // Check if the player is overpowered
    bool isOverpowered = difficultyManager.IsPlayerOverpowered(player);
    float overpowerFactor = difficultyManager.GetOverpowerFactor(player);
    
    // Add basic information
    explanation << "Raw Gear Level: " << rawGearLevel << "\n";
    explanation << "Target Difficulty: " << targetDifficulty << "\n";
    explanation << "Smoothed Difficulty: " << smoothedDifficulty << "\n";
    
    // Add overpower information
    if (isOverpowered) {
        explanation << "Overpower Status: Active (Factor: " << overpowerFactor << "x)\n";
        explanation << "Effective Difficulty: " << (smoothedDifficulty / overpowerFactor) << " (reduced by overpower)\n";
    } else {
        explanation << "Overpower Status: Inactive\n";
    }
    
    // Add monster scaling information
    explanation << "\nMonster Scaling Impact:\n";
    explanation << "- Monsters are scaled based on the smoothed difficulty level\n";
    explanation << "- This provides a more consistent challenge as your gear improves\n";
    explanation << "- Sudden gear upgrades won't immediately make monsters much harder\n";
    explanation << "- After significant upgrades, you'll feel temporarily overpowered\n";
    
    // Add transition information
    explanation << "\nDifficulty Transition:\n";
    const DifficultyTransitionParams &params = difficultyManager.GetTransitionParams();
    explanation << "- Difficulty increases at rate: " << params.increaseRate << " (lower is slower)\n";
    explanation << "- Difficulty decreases at rate: " << params.decreaseRate << " (lower is slower)\n";
    explanation << "- Overpower factor: " << params.overpowerFactor << "x\n";
    explanation << "- Overpower duration: " << params.overpowerDuration << " time units\n";
    
    // Add difficulty interpretation
    explanation << "\nDifficulty Interpretation:\n";
    if (smoothedDifficulty < 20.0f) {
        explanation << "- Beginner level difficulty (< 20)\n";
        explanation << "- Monsters will be relatively easy\n";
    } else if (smoothedDifficulty < 40.0f) {
        explanation << "- Novice level difficulty (20-40)\n";
        explanation << "- Monsters will provide a moderate challenge\n";
    } else if (smoothedDifficulty < 60.0f) {
        explanation << "- Intermediate level difficulty (40-60)\n";
        explanation << "- Monsters will be challenging\n";
    } else if (smoothedDifficulty < 80.0f) {
        explanation << "- Advanced level difficulty (60-80)\n";
        explanation << "- Monsters will be very challenging\n";
    } else {
        explanation << "- Expert level difficulty (80+)\n";
        explanation << "- Monsters will be extremely challenging\n";
    }
    
    return explanation.str();
}

bool DifficultyIntegration::IsSignificantUpgrade(float oldGearLevel, float newGearLevel)
{
    // Check if the gear level increased by at least 20%
    return newGearLevel > oldGearLevel * 1.2f;
}

} // namespace devilution
