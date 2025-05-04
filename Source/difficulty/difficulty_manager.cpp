/**
 * @file difficulty_manager.cpp
 *
 * Implementation of the difficulty manager that handles smooth transitions between difficulty levels.
 */
#include "difficulty/difficulty_manager.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <sstream>

#include "utils/log.hpp"
#include "gear/gear_manager.h"

namespace devilution {

DifficultyManager& DifficultyManager::GetInstance()
{
    static DifficultyManager instance;
    return instance;
}

void DifficultyManager::Initialize()
{
    if (initialized) {
        return;
    }
    
    // Initialize the gear level manager
    GearLevelManager::GetInstance().Initialize();
    
    // Clear any existing data
    playerStates.clear();
    changeCallbacks.clear();
    
    // Set default transition parameters
    transitionParams = DifficultyTransitionParams();
    
    initialized = true;
    LogVerbose("Difficulty Manager initialized");
}

void DifficultyManager::UpdateDifficulty(const Player &player, float gameTime)
{
    // Get the player's difficulty state
    DifficultyState &state = GetPlayerState(player);
    
    // Get the player's raw gear level
    float rawGearLevel = GearLevelManager::GetInstance().GetCurrentGearLevel(player);
    
    // Store the raw gear level
    state.rawGearLevel = rawGearLevel;
    
    // Calculate the target difficulty based on the raw gear level
    // For now, we'll use a simple 1:1 mapping, but this could be more complex
    float targetDifficulty = rawGearLevel;
    
    // Store the old difficulty for event firing
    float oldDifficulty = state.currentDifficulty;
    
    // Check if this is the first update
    if (state.lastUpdateTime == 0.0f) {
        // Initialize the difficulty to the target
        state.currentDifficulty = targetDifficulty;
        state.targetDifficulty = targetDifficulty;
        
        // Initialize the history with the current difficulty
        state.difficultyHistory.clear();
        for (int i = 0; i < transitionParams.historySize; i++) {
            state.difficultyHistory.push_back(targetDifficulty);
        }
    } else {
        // Calculate the time delta
        float deltaTime = gameTime - state.lastUpdateTime;
        
        // Update the target difficulty
        state.targetDifficulty = targetDifficulty;
        
        // Determine if we're increasing or decreasing difficulty
        bool increasing = targetDifficulty > state.currentDifficulty;
        
        // Choose the appropriate rate
        float rate = increasing ? transitionParams.increaseRate : transitionParams.decreaseRate;
        
        // Adjust the rate based on the time delta
        float adjustedRate = rate * deltaTime;
        
        // Clamp the rate to prevent overshooting
        adjustedRate = std::clamp(adjustedRate, 0.0f, 1.0f);
        
        // Interpolate between current and target difficulty
        float newDifficulty = InterpolateDifficulty(
            state.currentDifficulty,
            targetDifficulty,
            adjustedRate,
            transitionParams.interpolationType
        );
        
        // Apply moving average for additional smoothing
        newDifficulty = ApplyMovingAverage(state, newDifficulty);
        
        // Check if we're in the overpowered state
        if (gameTime < state.overpowerEndTime) {
            // Calculate how far we are through the overpower duration
            float overpowerProgress = (gameTime - (state.overpowerEndTime - transitionParams.overpowerDuration)) / transitionParams.overpowerDuration;
            
            // Clamp to 0-1 range
            overpowerProgress = std::clamp(overpowerProgress, 0.0f, 1.0f);
            
            // Calculate the overpower factor (starts at max, decreases to 1.0)
            float currentOverpowerFactor = 1.0f + (transitionParams.overpowerFactor - 1.0f) * (1.0f - overpowerProgress);
            
            // Apply the overpower factor (reduces effective difficulty)
            newDifficulty /= currentOverpowerFactor;
        }
        
        // Update the current difficulty
        state.currentDifficulty = newDifficulty;
    }
    
    // Update the last update time
    state.lastUpdateTime = gameTime;
    
    // Fire change event if the difficulty changed significantly
    if (std::abs(oldDifficulty - state.currentDifficulty) > 0.01f) {
        FireChangeEvent(player.getId(), oldDifficulty, state.currentDifficulty);
    }
}

float DifficultyManager::GetCurrentDifficulty(const Player &player)
{
    // Get the player's difficulty state
    const DifficultyState &state = GetPlayerState(player);
    
    return state.currentDifficulty;
}

float DifficultyManager::GetTargetDifficulty(const Player &player)
{
    // Get the player's difficulty state
    const DifficultyState &state = GetPlayerState(player);
    
    return state.targetDifficulty;
}

float DifficultyManager::GetRawGearLevel(const Player &player)
{
    // Get the player's difficulty state
    const DifficultyState &state = GetPlayerState(player);
    
    return state.rawGearLevel;
}

bool DifficultyManager::IsPlayerOverpowered(const Player &player)
{
    // Get the player's difficulty state
    const DifficultyState &state = GetPlayerState(player);
    
    // Check if the current time is before the overpower end time
    return state.lastUpdateTime < state.overpowerEndTime;
}

float DifficultyManager::GetOverpowerFactor(const Player &player)
{
    // Get the player's difficulty state
    const DifficultyState &state = GetPlayerState(player);
    
    // Check if we're in the overpowered state
    if (state.lastUpdateTime < state.overpowerEndTime) {
        // Calculate how far we are through the overpower duration
        float overpowerProgress = (state.lastUpdateTime - (state.overpowerEndTime - transitionParams.overpowerDuration)) / transitionParams.overpowerDuration;
        
        // Clamp to 0-1 range
        overpowerProgress = std::clamp(overpowerProgress, 0.0f, 1.0f);
        
        // Calculate the overpower factor (starts at max, decreases to 1.0)
        return 1.0f + (transitionParams.overpowerFactor - 1.0f) * (1.0f - overpowerProgress);
    }
    
    // Not overpowered
    return 1.0f;
}

void DifficultyManager::SetTransitionParams(const DifficultyTransitionParams &params)
{
    transitionParams = params;
    
    // Update history size for all player states
    for (auto &pair : playerStates) {
        DifficultyState &state = pair.second;
        
        // Resize the history buffer
        if (state.difficultyHistory.size() > static_cast<size_t>(params.historySize)) {
            // Remove oldest entries
            while (state.difficultyHistory.size() > static_cast<size_t>(params.historySize)) {
                state.difficultyHistory.pop_front();
            }
        } else if (state.difficultyHistory.size() < static_cast<size_t>(params.historySize)) {
            // Add new entries with the current difficulty
            while (state.difficultyHistory.size() < static_cast<size_t>(params.historySize)) {
                state.difficultyHistory.push_back(state.currentDifficulty);
            }
        }
    }
    
    LogVerbose("Difficulty transition parameters updated");
}

const DifficultyTransitionParams& DifficultyManager::GetTransitionParams() const
{
    return transitionParams;
}

uint32_t DifficultyManager::RegisterChangeCallback(DifficultyChangeCallback callback)
{
    uint32_t callbackId = nextCallbackId++;
    changeCallbacks[callbackId] = callback;
    return callbackId;
}

bool DifficultyManager::UnregisterChangeCallback(uint32_t callbackId)
{
    auto it = changeCallbacks.find(callbackId);
    if (it != changeCallbacks.end()) {
        changeCallbacks.erase(it);
        return true;
    }
    return false;
}

void DifficultyManager::TriggerOverpower(const Player &player, float gameTime, float factor, float duration)
{
    // Get the player's difficulty state
    DifficultyState &state = GetPlayerState(player);
    
    // Use provided factor or default
    float overpowerFactor = factor > 0.0f ? factor : transitionParams.overpowerFactor;
    
    // Use provided duration or default
    float overpowerDuration = duration > 0.0f ? duration : transitionParams.overpowerDuration;
    
    // Set the overpower end time
    state.overpowerEndTime = gameTime + overpowerDuration;
    
    LogVerbose("Triggered overpower effect for player {}: factor={}, duration={}, end_time={}",
        player.getId(), overpowerFactor, overpowerDuration, state.overpowerEndTime);
}

std::string DifficultyManager::GetDifficultyExplanation(const Player &player)
{
    std::stringstream explanation;
    explanation << "Difficulty Explanation for " << player._pName << ":\n\n";
    
    // Get the player's difficulty state
    const DifficultyState &state = GetPlayerState(player);
    
    // Add basic information
    explanation << "Raw Gear Level: " << state.rawGearLevel << "\n";
    explanation << "Target Difficulty: " << state.targetDifficulty << "\n";
    explanation << "Current Difficulty: " << state.currentDifficulty << "\n";
    
    // Add overpower information
    if (IsPlayerOverpowered(player)) {
        float overpowerFactor = GetOverpowerFactor(player);
        float remainingTime = state.overpowerEndTime - state.lastUpdateTime;
        
        explanation << "\nOverpower Status: Active\n";
        explanation << "Overpower Factor: " << overpowerFactor << "x\n";
        explanation << "Remaining Duration: " << remainingTime << " time units\n";
        explanation << "Effective Difficulty: " << (state.currentDifficulty / overpowerFactor) << " (reduced by overpower)\n";
    } else {
        explanation << "\nOverpower Status: Inactive\n";
    }
    
    // Add transition parameters
    explanation << "\nTransition Parameters:\n";
    explanation << "Increase Rate: " << transitionParams.increaseRate << " (lower is slower)\n";
    explanation << "Decrease Rate: " << transitionParams.decreaseRate << " (lower is slower)\n";
    explanation << "Overpower Factor: " << transitionParams.overpowerFactor << "x\n";
    explanation << "Overpower Duration: " << transitionParams.overpowerDuration << " time units\n";
    explanation << "History Size: " << transitionParams.historySize << " samples\n";
    explanation << "Interpolation Type: ";
    
    switch (transitionParams.interpolationType) {
    case InterpolationType::Linear:
        explanation << "Linear\n";
        break;
    case InterpolationType::Sigmoid:
        explanation << "Sigmoid\n";
        break;
    case InterpolationType::Exponential:
        explanation << "Exponential\n";
        break;
    case InterpolationType::Logarithmic:
        explanation << "Logarithmic\n";
        break;
    }
    
    // Add history information
    explanation << "\nDifficulty History (newest first):\n";
    int count = 0;
    for (auto it = state.difficultyHistory.rbegin(); it != state.difficultyHistory.rend(); ++it) {
        explanation << count++ << ": " << *it << "\n";
    }
    
    // Add difficulty interpretation
    explanation << "\nDifficulty Interpretation:\n";
    if (state.currentDifficulty < 20.0f) {
        explanation << "- Beginner level difficulty (< 20)\n";
        explanation << "- Monsters will be relatively easy\n";
    } else if (state.currentDifficulty < 40.0f) {
        explanation << "- Novice level difficulty (20-40)\n";
        explanation << "- Monsters will provide a moderate challenge\n";
    } else if (state.currentDifficulty < 60.0f) {
        explanation << "- Intermediate level difficulty (40-60)\n";
        explanation << "- Monsters will be challenging\n";
    } else if (state.currentDifficulty < 80.0f) {
        explanation << "- Advanced level difficulty (60-80)\n";
        explanation << "- Monsters will be very challenging\n";
    } else {
        explanation << "- Expert level difficulty (80+)\n";
        explanation << "- Monsters will be extremely challenging\n";
    }
    
    return explanation.str();
}

DifficultyState& DifficultyManager::GetPlayerState(const Player &player)
{
    uint8_t playerId = player.getId();
    
    // Check if the player state exists
    auto it = playerStates.find(playerId);
    if (it != playerStates.end()) {
        return it->second;
    }
    
    // Create a new state
    DifficultyState &state = playerStates[playerId];
    
    // Initialize the history with the default difficulty
    state.difficultyHistory.clear();
    for (int i = 0; i < transitionParams.historySize; i++) {
        state.difficultyHistory.push_back(1.0f);
    }
    
    return state;
}

float DifficultyManager::InterpolateDifficulty(float current, float target, float rate, InterpolationType type)
{
    switch (type) {
    case InterpolationType::Linear:
        // Linear interpolation: current + (target - current) * rate
        return current + (target - current) * rate;
        
    case InterpolationType::Sigmoid:
        // Sigmoid interpolation for smoother transitions near the extremes
        {
            // Convert the linear rate to a sigmoid rate
            // This ensures the middle of the transition moves at the expected rate
            float sigmoidRate = std::atan(rate * 10.0f - 5.0f) / 3.14159f + 0.5f;
            return current + (target - current) * sigmoidRate;
        }
        
    case InterpolationType::Exponential:
        // Exponential interpolation: current * (target/current)^rate
        // Prevents negative values and provides faster approach to target
        {
            if (current <= 0.0f || target <= 0.0f) {
                // Fall back to linear for zero or negative values
                return current + (target - current) * rate;
            }
            
            return current * std::pow(target / current, rate);
        }
        
    case InterpolationType::Logarithmic:
        // Logarithmic interpolation for slower approach to target
        {
            if (current <= 0.0f || target <= 0.0f) {
                // Fall back to linear for zero or negative values
                return current + (target - current) * rate;
            }
            
            // Use log interpolation: current * (1 + log(1 + rate * (target/current - 1)))
            return current * (1.0f + std::log(1.0f + rate * (target / current - 1.0f)));
        }
        
    default:
        // Default to linear
        return current + (target - current) * rate;
    }
}

float DifficultyManager::ApplyMovingAverage(DifficultyState &state, float newDifficulty)
{
    // Add the new difficulty to the history
    state.difficultyHistory.push_back(newDifficulty);
    
    // Remove the oldest entry if the history is too large
    if (state.difficultyHistory.size() > static_cast<size_t>(transitionParams.historySize)) {
        state.difficultyHistory.pop_front();
    }
    
    // Calculate the moving average
    float sum = std::accumulate(state.difficultyHistory.begin(), state.difficultyHistory.end(), 0.0f);
    return sum / state.difficultyHistory.size();
}

void DifficultyManager::FireChangeEvent(uint8_t playerId, float oldDifficulty, float newDifficulty)
{
    for (const auto &pair : changeCallbacks) {
        pair.second(playerId, oldDifficulty, newDifficulty);
    }
    
    // Log the change
    LogVerbose("Difficulty changed for player {}: {:.2f} -> {:.2f}", playerId, oldDifficulty, newDifficulty);
}

} // namespace devilution
