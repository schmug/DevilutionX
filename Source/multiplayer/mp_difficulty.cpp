/**
 * @file mp_difficulty.cpp
 *
 * Implementation of the multiplayer difficulty system that handles difficulty scaling in multiplayer games.
 */
#include "multiplayer/mp_difficulty.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <sstream>

#include "utils/log.hpp"
#include "difficulty/difficulty_manager.h"
#include "multi.h"

namespace devilution {

MpDifficultyManager& MpDifficultyManager::GetInstance()
{
    static MpDifficultyManager instance;
    return instance;
}

void MpDifficultyManager::Initialize()
{
    if (initialized) {
        return;
    }
    
    // Initialize the difficulty manager
    DifficultyManager::GetInstance().Initialize();
    
    // Set default parameters
    params = MpDifficultyParams();
    
    // Reset cache
    cachedCombinedDifficulty = 0.0f;
    isCacheValid = false;
    lastUpdateTime = 0.0f;
    
    initialized = true;
    LogVerbose("Multiplayer Difficulty Manager initialized");
}

void MpDifficultyManager::Update(float gameTime)
{
    // Skip if no time has passed
    if (gameTime <= lastUpdateTime) {
        return;
    }
    
    // Update the last update time
    lastUpdateTime = gameTime;
    
    // Update individual player difficulties
    for (int i = 0; i < MAX_PLRS; i++) {
        if (plr[i].plractive) {
            DifficultyManager::GetInstance().UpdateDifficulty(plr[i], gameTime);
        }
    }
    
    // Normalize player difficulties if needed
    if (params.maxPlayerDiffVariance > 0.0f) {
        NormalizePlayerDifficulties();
    }
    
    // Invalidate the combined difficulty cache
    isCacheValid = false;
    
    // Sync with other clients if needed
    if (params.syncDifficulty && gbIsMultiplayer) {
        SyncWithClients(myplr == 0);
    }
}

float MpDifficultyManager::GetEffectiveDifficulty(const MonsterData &monster, int nearestPlayerId)
{
    // If we're not in multiplayer, just return the single player difficulty
    if (!gbIsMultiplayer) {
        return DifficultyManager::GetInstance().GetCurrentDifficulty(plr[myplr]);
    }
    
    // Handle different difficulty methods
    switch (params.method) {
    case MpDifficultyMethod::Average:
        return ApplyPlayerCountScaling(CalculateAverageDifficulty());
        
    case MpDifficultyMethod::Maximum:
        return ApplyPlayerCountScaling(CalculateMaximumDifficulty());
        
    case MpDifficultyMethod::Weighted:
        return ApplyPlayerCountScaling(CalculateWeightedDifficulty(monster));
        
    case MpDifficultyMethod::PerPlayer:
        // Find the nearest player if not provided
        if (nearestPlayerId < 0) {
            nearestPlayerId = FindNearestPlayer(monster);
        }
        
        // If we found a player, use their difficulty
        if (nearestPlayerId >= 0 && nearestPlayerId < MAX_PLRS && plr[nearestPlayerId].plractive) {
            return DifficultyManager::GetInstance().GetCurrentDifficulty(plr[nearestPlayerId]);
        }
        
        // Fall back to average if no player is found
        return ApplyPlayerCountScaling(CalculateAverageDifficulty());
        
    case MpDifficultyMethod::Host:
        return ApplyPlayerCountScaling(GetHostDifficulty());
        
    default:
        // Default to average
        return ApplyPlayerCountScaling(CalculateAverageDifficulty());
    }
}

float MpDifficultyManager::GetCombinedDifficulty()
{
    // Check if the cache is valid
    if (isCacheValid) {
        return cachedCombinedDifficulty;
    }
    
    // Calculate the combined difficulty based on the method
    float combinedDifficulty = 0.0f;
    
    switch (params.method) {
    case MpDifficultyMethod::Average:
        combinedDifficulty = CalculateAverageDifficulty();
        break;
        
    case MpDifficultyMethod::Maximum:
        combinedDifficulty = CalculateMaximumDifficulty();
        break;
        
    case MpDifficultyMethod::Weighted:
        // For the combined difficulty, we can't use weighted without a specific monster
        // So fall back to average
        combinedDifficulty = CalculateAverageDifficulty();
        break;
        
    case MpDifficultyMethod::PerPlayer:
        // For the combined difficulty, we can't use per-player without a specific monster
        // So fall back to average
        combinedDifficulty = CalculateAverageDifficulty();
        break;
        
    case MpDifficultyMethod::Host:
        combinedDifficulty = GetHostDifficulty();
        break;
        
    default:
        // Default to average
        combinedDifficulty = CalculateAverageDifficulty();
        break;
    }
    
    // Apply player count scaling
    combinedDifficulty = ApplyPlayerCountScaling(combinedDifficulty);
    
    // Cache the result
    cachedCombinedDifficulty = combinedDifficulty;
    isCacheValid = true;
    
    return combinedDifficulty;
}

float MpDifficultyManager::GetPlayerDifficulty(uint8_t playerId)
{
    // Check if the player is valid
    if (playerId >= MAX_PLRS || !plr[playerId].plractive) {
        return 0.0f;
    }
    
    return DifficultyManager::GetInstance().GetCurrentDifficulty(plr[playerId]);
}

void MpDifficultyManager::SetParams(const MpDifficultyParams &newParams)
{
    params = newParams;
    
    // Invalidate the cache
    isCacheValid = false;
    
    LogVerbose("Multiplayer difficulty parameters updated");
}

const MpDifficultyParams& MpDifficultyManager::GetParams() const
{
    return params;
}

void MpDifficultyManager::SyncWithClients(bool isHost)
{
    // This is a placeholder for the actual networking code
    // In a real implementation, this would send difficulty data to other clients
    
    if (isHost) {
        // Host sends difficulty data to clients
        LogVerbose("Host sending difficulty data to clients");
    } else {
        // Clients receive difficulty data from host
        LogVerbose("Client receiving difficulty data from host");
    }
}

void MpDifficultyManager::OnPlayerJoin(uint8_t playerId)
{
    // Invalidate the cache
    isCacheValid = false;
    
    LogVerbose("Player {} joined, invalidating difficulty cache", playerId);
}

void MpDifficultyManager::OnPlayerLeave(uint8_t playerId)
{
    // Invalidate the cache
    isCacheValid = false;
    
    LogVerbose("Player {} left, invalidating difficulty cache", playerId);
}

std::string MpDifficultyManager::GetMpDifficultyExplanation()
{
    std::stringstream explanation;
    explanation << "Multiplayer Difficulty Explanation:\n";
    explanation << "==================================\n\n";
    
    // Add method information
    explanation << "Difficulty Calculation Method: ";
    switch (params.method) {
    case MpDifficultyMethod::Average:
        explanation << "Average of all player difficulties\n";
        break;
    case MpDifficultyMethod::Maximum:
        explanation << "Maximum of all player difficulties\n";
        break;
    case MpDifficultyMethod::Weighted:
        explanation << "Weighted average based on proximity\n";
        break;
    case MpDifficultyMethod::PerPlayer:
        explanation << "Per-player (zone-based) difficulty\n";
        break;
    case MpDifficultyMethod::Host:
        explanation << "Host's difficulty\n";
        break;
    }
    
    // Add player count scaling information
    explanation << "Player Count Scaling: " << params.playerCountScaling << " (0 = no scaling)\n";
    
    // Add variance information
    explanation << "Max Player Difficulty Variance: " << params.maxPlayerDiffVariance << "\n";
    
    // Add proximity weight information
    if (params.method == MpDifficultyMethod::Weighted) {
        explanation << "Proximity Weight: " << params.proximityWeight << "\n";
    }
    
    // Add sync information
    explanation << "Difficulty Synchronization: " << (params.syncDifficulty ? "Enabled" : "Disabled") << "\n\n";
    
    // Add player difficulties
    explanation << "Player Difficulties:\n";
    int activePlayers = 0;
    
    for (int i = 0; i < MAX_PLRS; i++) {
        if (plr[i].plractive) {
            float difficulty = DifficultyManager::GetInstance().GetCurrentDifficulty(plr[i]);
            explanation << "- Player " << i << " (" << plr[i]._pName << "): " << difficulty << "\n";
            activePlayers++;
        }
    }
    
    // Add combined difficulty
    explanation << "\nCombined Difficulty: " << GetCombinedDifficulty() << "\n";
    
    // Add player count scaling effect
    if (params.playerCountScaling > 0.0f) {
        float baseAverage = CalculateAverageDifficulty();
        float scaledAverage = ApplyPlayerCountScaling(baseAverage);
        explanation << "Player Count Scaling Effect: " << baseAverage << " -> " << scaledAverage
                   << " (+" << (scaledAverage - baseAverage) << " for " << activePlayers << " players)\n";
    }
    
    return explanation.str();
}

float MpDifficultyManager::CalculateDistance(const Player &player, const MonsterData &monster)
{
    // Calculate the distance between a player and a monster
    // This is a simplified version - in a real implementation, this would use the actual game coordinates
    
    float dx = player.position.x - monster.position.x;
    float dy = player.position.y - monster.position.y;
    
    return std::sqrt(dx * dx + dy * dy);
}

int MpDifficultyManager::FindNearestPlayer(const MonsterData &monster)
{
    int nearestPlayer = -1;
    float nearestDistance = std::numeric_limits<float>::max();
    
    // Find the nearest active player
    for (int i = 0; i < MAX_PLRS; i++) {
        if (plr[i].plractive) {
            float distance = CalculateDistance(plr[i], monster);
            
            if (distance < nearestDistance) {
                nearestDistance = distance;
                nearestPlayer = i;
            }
        }
    }
    
    return nearestPlayer;
}

float MpDifficultyManager::CalculateAverageDifficulty()
{
    float totalDifficulty = 0.0f;
    int activePlayers = 0;
    
    // Sum up the difficulties of all active players
    for (int i = 0; i < MAX_PLRS; i++) {
        if (plr[i].plractive) {
            totalDifficulty += DifficultyManager::GetInstance().GetCurrentDifficulty(plr[i]);
            activePlayers++;
        }
    }
    
    // Calculate the average
    return activePlayers > 0 ? totalDifficulty / activePlayers : 0.0f;
}

float MpDifficultyManager::CalculateMaximumDifficulty()
{
    float maxDifficulty = 0.0f;
    
    // Find the maximum difficulty among all active players
    for (int i = 0; i < MAX_PLRS; i++) {
        if (plr[i].plractive) {
            float difficulty = DifficultyManager::GetInstance().GetCurrentDifficulty(plr[i]);
            maxDifficulty = std::max(maxDifficulty, difficulty);
        }
    }
    
    return maxDifficulty;
}

float MpDifficultyManager::CalculateWeightedDifficulty(const MonsterData &monster)
{
    float totalWeight = 0.0f;
    float weightedDifficulty = 0.0f;
    
    // Calculate the weighted average based on proximity
    for (int i = 0; i < MAX_PLRS; i++) {
        if (plr[i].plractive) {
            float distance = CalculateDistance(plr[i], monster);
            
            // Calculate weight based on distance (closer = higher weight)
            // Add a small constant to avoid division by zero
            float weight = 1.0f / (distance + 1.0f);
            
            // Apply the proximity weight parameter
            weight = std::pow(weight, params.proximityWeight);
            
            // Add to the weighted sum
            float difficulty = DifficultyManager::GetInstance().GetCurrentDifficulty(plr[i]);
            weightedDifficulty += difficulty * weight;
            totalWeight += weight;
        }
    }
    
    // Calculate the weighted average
    return totalWeight > 0.0f ? weightedDifficulty / totalWeight : 0.0f;
}

float MpDifficultyManager::GetHostDifficulty()
{
    // Get the host's difficulty (player 0)
    if (plr[0].plractive) {
        return DifficultyManager::GetInstance().GetCurrentDifficulty(plr[0]);
    }
    
    // Fall back to average if host is not active
    return CalculateAverageDifficulty();
}

float MpDifficultyManager::ApplyPlayerCountScaling(float baseDifficulty)
{
    // Count active players
    int activePlayers = 0;
    for (int i = 0; i < MAX_PLRS; i++) {
        if (plr[i].plractive) {
            activePlayers++;
        }
    }
    
    // Apply scaling based on player count
    // Formula: baseDifficulty * (1 + (activePlayers - 1) * playerCountScaling)
    return baseDifficulty * (1.0f + (activePlayers - 1) * params.playerCountScaling);
}

void MpDifficultyManager::NormalizePlayerDifficulties()
{
    // Get all active player difficulties
    std::vector<float> difficulties;
    std::vector<int> playerIds;
    
    for (int i = 0; i < MAX_PLRS; i++) {
        if (plr[i].plractive) {
            difficulties.push_back(DifficultyManager::GetInstance().GetCurrentDifficulty(plr[i]));
            playerIds.push_back(i);
        }
    }
    
    // Check if we have at least two players
    if (difficulties.size() < 2) {
        return;
    }
    
    // Calculate the average difficulty
    float avgDifficulty = std::accumulate(difficulties.begin(), difficulties.end(), 0.0f) / difficulties.size();
    
    // Check if any player is too far from the average
    bool needsNormalization = false;
    for (float diff : difficulties) {
        if (std::abs(diff - avgDifficulty) > params.maxPlayerDiffVariance) {
            needsNormalization = true;
            break;
        }
    }
    
    // If no normalization is needed, return
    if (!needsNormalization) {
        return;
    }
    
    // Normalize the difficulties
    for (size_t i = 0; i < difficulties.size(); i++) {
        float diff = difficulties[i];
        int playerId = playerIds[i];
        
        // Calculate how far this player is from the average
        float variance = diff - avgDifficulty;
        
        // If the variance is within limits, skip this player
        if (std::abs(variance) <= params.maxPlayerDiffVariance) {
            continue;
        }
        
        // Clamp the difficulty to the allowed range
        float newDiff;
        if (variance > 0) {
            newDiff = avgDifficulty + params.maxPlayerDiffVariance;
        } else {
            newDiff = avgDifficulty - params.maxPlayerDiffVariance;
        }
        
        // Apply the new difficulty
        // In a real implementation, this would modify the player's difficulty directly
        // For now, we'll just log it
        LogVerbose("Normalized player {} difficulty: {} -> {}", playerId, diff, newDiff);
    }
}

} // namespace devilution
