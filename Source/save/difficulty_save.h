/**
 * @file difficulty_save.h
 *
 * Interface for saving and loading difficulty system state to/from save files.
 */
#pragma once

#include <cstdint>
#include <string>

#include "player.h"
#include "difficulty/difficulty_manager.h"

namespace devilution {

/**
 * @brief Structure for serialized difficulty state
 */
struct SerializedDifficultyState {
    float currentDifficulty;  // Current smoothed difficulty
    float targetDifficulty;   // Target difficulty based on gear level
    float rawGearLevel;       // Raw gear level before smoothing
    float lastUpdateTime;     // Last time the difficulty was updated
    float overpowerEndTime;   // Time when the overpower effect ends
    uint8_t historySize;      // Size of the difficulty history
    float difficultyHistory[20]; // History of difficulty values (max 20)
    
    // Default constructor
    SerializedDifficultyState()
        : currentDifficulty(1.0f), targetDifficulty(1.0f), rawGearLevel(1.0f),
          lastUpdateTime(0.0f), overpowerEndTime(0.0f), historySize(0)
    {
        for (int i = 0; i < 20; i++) {
            difficultyHistory[i] = 1.0f;
        }
    }
};

/**
 * @brief Class for saving and loading difficulty system state
 */
class DifficultySaveManager {
public:
    /**
     * @brief Gets the singleton instance
     * @return Reference to the singleton instance
     */
    static DifficultySaveManager& GetInstance();
    
    /**
     * @brief Initializes the difficulty save manager
     */
    void Initialize();
    
    /**
     * @brief Saves difficulty state to a save file
     * @param player The player to save state for
     * @param savePath The path to the save file
     * @return True if saving was successful, false otherwise
     */
    bool SaveDifficultyState(const Player &player, const std::string &savePath);
    
    /**
     * @brief Loads difficulty state from a save file
     * @param player The player to load state for
     * @param savePath The path to the save file
     * @return True if loading was successful, false otherwise
     */
    bool LoadDifficultyState(Player &player, const std::string &savePath);
    
    /**
     * @brief Checks if a save file contains difficulty state
     * @param savePath The path to the save file
     * @return True if the save file contains difficulty state, false otherwise
     */
    bool HasDifficultyState(const std::string &savePath);
    
    /**
     * @brief Initializes default difficulty state for a player
     * @param player The player to initialize state for
     */
    void InitializeDefaultState(Player &player);
    
    /**
     * @brief Migrates an old save file to the new format
     * @param savePath The path to the save file
     * @return True if migration was successful, false otherwise
     */
    bool MigrateSaveFile(const std::string &savePath);
    
    /**
     * @brief Gets the version of the difficulty system in a save file
     * @param savePath The path to the save file
     * @return The version number, or 0 if not found
     */
    uint32_t GetSaveFileVersion(const std::string &savePath);
    
    /**
     * @brief Gets the current difficulty system version
     * @return The current version number
     */
    uint32_t GetCurrentVersion() const;
    
private:
    DifficultySaveManager() = default;
    ~DifficultySaveManager() = default;
    
    DifficultySaveManager(const DifficultySaveManager&) = delete;
    DifficultySaveManager& operator=(const DifficultySaveManager&) = delete;
    
    /**
     * @brief Serializes difficulty state to a buffer
     * @param player The player to serialize state for
     * @param state The serialized state to fill
     */
    void SerializeDifficultyState(const Player &player, SerializedDifficultyState &state);
    
    /**
     * @brief Deserializes difficulty state from a buffer
     * @param player The player to deserialize state for
     * @param state The serialized state to read from
     */
    void DeserializeDifficultyState(Player &player, const SerializedDifficultyState &state);
    
    /**
     * @brief Reads difficulty state from a save file
     * @param savePath The path to the save file
     * @param state The serialized state to fill
     * @return True if reading was successful, false otherwise
     */
    bool ReadStateFromSaveFile(const std::string &savePath, SerializedDifficultyState &state);
    
    /**
     * @brief Writes difficulty state to a save file
     * @param savePath The path to the save file
     * @param state The serialized state to write
     * @return True if writing was successful, false otherwise
     */
    bool WriteStateToSaveFile(const std::string &savePath, const SerializedDifficultyState &state);
    
    // Current version of the difficulty system
    static const uint32_t DIFFICULTY_VERSION = 1;
    
    // Flag to track initialization
    bool initialized = false;
};

} // namespace devilution
