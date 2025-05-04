/**
 * @file mp_difficulty.h
 *
 * Interface for the multiplayer difficulty system that handles difficulty scaling in multiplayer games.
 */
#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "player.h"
#include "difficulty/difficulty_manager.h"

namespace devilution {

/**
 * @brief Enum for different multiplayer difficulty calculation methods
 */
enum class MpDifficultyMethod {
    Average,    // Average of all player difficulties
    Maximum,    // Maximum of all player difficulties
    Weighted,   // Weighted average based on proximity
    PerPlayer,  // Each player gets their own difficulty (zone-based)
    Host        // Use the host's difficulty
};

/**
 * @brief Structure for multiplayer difficulty parameters
 */
struct MpDifficultyParams {
    MpDifficultyMethod method;       // Method to use for calculating difficulty
    float playerCountScaling;        // Factor to scale difficulty by player count (1.0 = no scaling)
    float maxPlayerDiffVariance;     // Maximum allowed variance between player difficulties
    float proximityWeight;           // Weight for proximity-based calculations
    bool syncDifficulty;             // Whether to synchronize difficulty across clients
    
    // Default constructor
    MpDifficultyParams()
        : method(MpDifficultyMethod::Weighted), playerCountScaling(0.1f),
          maxPlayerDiffVariance(20.0f), proximityWeight(0.5f), syncDifficulty(true)
    {
    }
};

/**
 * @brief Class for managing difficulty in multiplayer games
 */
class MpDifficultyManager {
public:
    /**
     * @brief Gets the singleton instance
     * @return Reference to the singleton instance
     */
    static MpDifficultyManager& GetInstance();
    
    /**
     * @brief Initializes the multiplayer difficulty manager
     */
    void Initialize();
    
    /**
     * @brief Updates the multiplayer difficulty
     * @param gameTime The current game time
     */
    void Update(float gameTime);
    
    /**
     * @brief Gets the effective difficulty for a monster
     * @param monster The monster to get the difficulty for
     * @param nearestPlayerId The ID of the nearest player (or -1 if unknown)
     * @return The effective difficulty
     */
    float GetEffectiveDifficulty(const MonsterData &monster, int nearestPlayerId = -1);
    
    /**
     * @brief Gets the combined difficulty for all players
     * @return The combined difficulty
     */
    float GetCombinedDifficulty();
    
    /**
     * @brief Gets the difficulty for a specific player
     * @param playerId The ID of the player
     * @return The player's difficulty
     */
    float GetPlayerDifficulty(uint8_t playerId);
    
    /**
     * @brief Sets the multiplayer difficulty parameters
     * @param params The new parameters
     */
    void SetParams(const MpDifficultyParams &params);
    
    /**
     * @brief Gets the current multiplayer difficulty parameters
     * @return The current parameters
     */
    const MpDifficultyParams& GetParams() const;
    
    /**
     * @brief Synchronizes difficulty data with other clients
     * @param isHost Whether this client is the host
     */
    void SyncWithClients(bool isHost);
    
    /**
     * @brief Handles a player joining the game
     * @param playerId The ID of the player who joined
     */
    void OnPlayerJoin(uint8_t playerId);
    
    /**
     * @brief Handles a player leaving the game
     * @param playerId The ID of the player who left
     */
    void OnPlayerLeave(uint8_t playerId);
    
    /**
     * @brief Gets a text explanation of the multiplayer difficulty
     * @return A text explanation of the multiplayer difficulty
     */
    std::string GetMpDifficultyExplanation();
    
    /**
     * @brief Calculates the distance between a player and a monster
     * @param player The player
     * @param monster The monster
     * @return The distance between the player and monster
     */
    float CalculateDistance(const Player &player, const MonsterData &monster);
    
    /**
     * @brief Finds the nearest player to a monster
     * @param monster The monster
     * @return The ID of the nearest player, or -1 if no players are found
     */
    int FindNearestPlayer(const MonsterData &monster);
    
private:
    MpDifficultyManager() = default;
    ~MpDifficultyManager() = default;
    
    MpDifficultyManager(const MpDifficultyManager&) = delete;
    MpDifficultyManager& operator=(const MpDifficultyManager&) = delete;
    
    /**
     * @brief Calculates the average difficulty of all players
     * @return The average difficulty
     */
    float CalculateAverageDifficulty();
    
    /**
     * @brief Calculates the maximum difficulty of all players
     * @return The maximum difficulty
     */
    float CalculateMaximumDifficulty();
    
    /**
     * @brief Calculates a weighted difficulty based on player proximity to a monster
     * @param monster The monster
     * @return The weighted difficulty
     */
    float CalculateWeightedDifficulty(const MonsterData &monster);
    
    /**
     * @brief Gets the host player's difficulty
     * @return The host's difficulty
     */
    float GetHostDifficulty();
    
    /**
     * @brief Applies player count scaling to a difficulty value
     * @param baseDifficulty The base difficulty
     * @return The scaled difficulty
     */
    float ApplyPlayerCountScaling(float baseDifficulty);
    
    /**
     * @brief Normalizes player difficulties to reduce variance
     */
    void NormalizePlayerDifficulties();
    
    // Multiplayer difficulty parameters
    MpDifficultyParams params;
    
    // Combined difficulty cache
    float cachedCombinedDifficulty = 0.0f;
    
    // Flag to track if the cache is valid
    bool isCacheValid = false;
    
    // Last update time
    float lastUpdateTime = 0.0f;
    
    // Flag to track initialization
    bool initialized = false;
};

} // namespace devilution
