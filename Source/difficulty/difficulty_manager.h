/**
 * @file difficulty_manager.h
 *
 * Interface for the difficulty manager that handles smooth transitions between difficulty levels.
 */
#pragma once

#include <cstdint>
#include <deque>
#include <unordered_map>
#include <functional>

#include "player.h"
#include "gear/gear_manager.h"

namespace devilution {

/**
 * @brief Enum for different interpolation types
 */
enum class InterpolationType {
    Linear,     // Linear interpolation
    Sigmoid,    // Sigmoid (S-curve) interpolation
    Exponential, // Exponential interpolation
    Logarithmic  // Logarithmic interpolation
};

/**
 * @brief Structure for difficulty transition parameters
 */
struct DifficultyTransitionParams {
    float increaseRate;      // Rate at which difficulty increases (0-1, lower is slower)
    float decreaseRate;      // Rate at which difficulty decreases (0-1, lower is slower)
    float overpowerFactor;   // Factor for temporary overpowered feeling after upgrades (>1)
    float overpowerDuration; // Duration of overpowered feeling in game time units
    int historySize;         // Size of the history buffer for moving averages
    InterpolationType interpolationType; // Type of interpolation to use
    
    // Default constructor
    DifficultyTransitionParams()
        : increaseRate(0.1f), decreaseRate(0.2f), overpowerFactor(1.5f),
          overpowerDuration(300.0f), historySize(10), interpolationType(InterpolationType::Sigmoid)
    {
    }
};

/**
 * @brief Structure for difficulty state
 */
struct DifficultyState {
    float currentDifficulty;  // Current smoothed difficulty
    float targetDifficulty;   // Target difficulty based on gear level
    float rawGearLevel;       // Raw gear level before smoothing
    float lastUpdateTime;     // Last time the difficulty was updated
    float overpowerEndTime;   // Time when the overpower effect ends
    std::deque<float> difficultyHistory; // History of difficulty values for moving average
    
    // Default constructor
    DifficultyState()
        : currentDifficulty(1.0f), targetDifficulty(1.0f), rawGearLevel(1.0f),
          lastUpdateTime(0.0f), overpowerEndTime(0.0f)
    {
    }
};

/**
 * @brief Callback type for difficulty change events
 */
using DifficultyChangeCallback = std::function<void(uint8_t playerId, float oldDifficulty, float newDifficulty)>;

/**
 * @brief Manager class for difficulty transitions and smoothing
 */
class DifficultyManager {
public:
    /**
     * @brief Gets the singleton instance
     * @return Reference to the singleton instance
     */
    static DifficultyManager& GetInstance();
    
    /**
     * @brief Initializes the difficulty manager
     */
    void Initialize();
    
    /**
     * @brief Updates the difficulty for a player
     * @param player The player to update
     * @param gameTime The current game time
     */
    void UpdateDifficulty(const Player &player, float gameTime);
    
    /**
     * @brief Gets the current smoothed difficulty for a player
     * @param player The player to get the difficulty for
     * @return The current smoothed difficulty
     */
    float GetCurrentDifficulty(const Player &player);
    
    /**
     * @brief Gets the target difficulty for a player
     * @param player The player to get the difficulty for
     * @return The target difficulty
     */
    float GetTargetDifficulty(const Player &player);
    
    /**
     * @brief Gets the raw gear level for a player
     * @param player The player to get the gear level for
     * @return The raw gear level
     */
    float GetRawGearLevel(const Player &player);
    
    /**
     * @brief Checks if a player is in the overpowered state
     * @param player The player to check
     * @return True if the player is overpowered, false otherwise
     */
    bool IsPlayerOverpowered(const Player &player);
    
    /**
     * @brief Gets the overpower factor for a player
     * @param player The player to get the factor for
     * @return The current overpower factor (1.0 if not overpowered)
     */
    float GetOverpowerFactor(const Player &player);
    
    /**
     * @brief Sets the transition parameters
     * @param params The new transition parameters
     */
    void SetTransitionParams(const DifficultyTransitionParams &params);
    
    /**
     * @brief Gets the current transition parameters
     * @return The current transition parameters
     */
    const DifficultyTransitionParams& GetTransitionParams() const;
    
    /**
     * @brief Registers a callback for difficulty changes
     * @param callback The callback function to register
     * @return An ID that can be used to unregister the callback
     */
    uint32_t RegisterChangeCallback(DifficultyChangeCallback callback);
    
    /**
     * @brief Unregisters a callback for difficulty changes
     * @param callbackId The ID of the callback to unregister
     * @return True if the callback was unregistered, false if not found
     */
    bool UnregisterChangeCallback(uint32_t callbackId);
    
    /**
     * @brief Triggers an overpower effect for a player
     * @param player The player to trigger the effect for
     * @param gameTime The current game time
     * @param factor Optional custom overpower factor (uses default if not specified)
     * @param duration Optional custom duration (uses default if not specified)
     */
    void TriggerOverpower(const Player &player, float gameTime, float factor = 0.0f, float duration = 0.0f);
    
    /**
     * @brief Gets a text explanation of the difficulty calculation
     * @param player The player to explain
     * @return A text explanation of the difficulty calculation
     */
    std::string GetDifficultyExplanation(const Player &player);
    
private:
    DifficultyManager() = default;
    ~DifficultyManager() = default;
    
    DifficultyManager(const DifficultyManager&) = delete;
    DifficultyManager& operator=(const DifficultyManager&) = delete;
    
    /**
     * @brief Gets the difficulty state for a player
     * @param player The player to get the state for
     * @return Reference to the player's difficulty state
     */
    DifficultyState& GetPlayerState(const Player &player);
    
    /**
     * @brief Interpolates between current and target difficulty
     * @param current The current difficulty
     * @param target The target difficulty
     * @param rate The interpolation rate
     * @param type The interpolation type
     * @return The interpolated difficulty
     */
    float InterpolateDifficulty(float current, float target, float rate, InterpolationType type);
    
    /**
     * @brief Applies the moving average to the difficulty
     * @param state The difficulty state to update
     * @param newDifficulty The new difficulty value
     * @return The smoothed difficulty
     */
    float ApplyMovingAverage(DifficultyState &state, float newDifficulty);
    
    /**
     * @brief Fires a difficulty change event
     * @param playerId The ID of the player whose difficulty changed
     * @param oldDifficulty The old difficulty
     * @param newDifficulty The new difficulty
     */
    void FireChangeEvent(uint8_t playerId, float oldDifficulty, float newDifficulty);
    
    // Difficulty states by player ID
    std::unordered_map<uint8_t, DifficultyState> playerStates;
    
    // Transition parameters
    DifficultyTransitionParams transitionParams;
    
    // Callbacks for difficulty changes
    std::unordered_map<uint32_t, DifficultyChangeCallback> changeCallbacks;
    uint32_t nextCallbackId = 1;
    
    // Flag to track initialization
    bool initialized = false;
};

} // namespace devilution
