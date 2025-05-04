/**
 * @file difficulty_save.cpp
 *
 * Implementation of saving and loading difficulty system state to/from save files.
 */
#include "save/difficulty_save.h"

#include <fstream>
#include <cstring>

#include "utils/log.hpp"
#include "utils/file_util.h"
#include "difficulty/difficulty_manager.h"

namespace devilution {

DifficultySaveManager& DifficultySaveManager::GetInstance()
{
    static DifficultySaveManager instance;
    return instance;
}

void DifficultySaveManager::Initialize()
{
    if (initialized) {
        return;
    }
    
    // Initialize the difficulty manager
    DifficultyManager::GetInstance().Initialize();
    
    initialized = true;
    LogVerbose("Difficulty Save Manager initialized");
}

bool DifficultySaveManager::SaveDifficultyState(const Player &player, const std::string &savePath)
{
    // Serialize the difficulty state
    SerializedDifficultyState state;
    SerializeDifficultyState(player, state);
    
    // Write the state to the save file
    return WriteStateToSaveFile(savePath, state);
}

bool DifficultySaveManager::LoadDifficultyState(Player &player, const std::string &savePath)
{
    // Check if the save file has difficulty state
    if (!HasDifficultyState(savePath)) {
        // Initialize default state
        InitializeDefaultState(player);
        return true;
    }
    
    // Read the state from the save file
    SerializedDifficultyState state;
    if (!ReadStateFromSaveFile(savePath, state)) {
        // Failed to read state, initialize default state
        InitializeDefaultState(player);
        return false;
    }
    
    // Deserialize the difficulty state
    DeserializeDifficultyState(player, state);
    
    return true;
}

bool DifficultySaveManager::HasDifficultyState(const std::string &savePath)
{
    // Check if the save file exists
    if (!FileExists(savePath)) {
        return false;
    }
    
    // Get the version of the difficulty system in the save file
    uint32_t version = GetSaveFileVersion(savePath);
    
    // If the version is 0, the save file doesn't have difficulty state
    return version > 0;
}

void DifficultySaveManager::InitializeDefaultState(Player &player)
{
    // Get the current game time
    float gameTime = 0.0f; // TODO: Get actual game time
    
    // Update the difficulty with default values
    DifficultyManager::GetInstance().UpdateDifficulty(player, gameTime);
    
    LogVerbose("Initialized default difficulty state for player {}", player.getId());
}

bool DifficultySaveManager::MigrateSaveFile(const std::string &savePath)
{
    // Check if the save file exists
    if (!FileExists(savePath)) {
        return false;
    }
    
    // Get the version of the difficulty system in the save file
    uint32_t version = GetSaveFileVersion(savePath);
    
    // If the version is current, no migration needed
    if (version == DIFFICULTY_VERSION) {
        return true;
    }
    
    // If the version is 0, the save file doesn't have difficulty state
    if (version == 0) {
        // Create a default state
        SerializedDifficultyState state;
        
        // Write the state to the save file
        return WriteStateToSaveFile(savePath, state);
    }
    
    // For future migrations between different versions
    // This would handle migrations from version 1 to 2, 2 to 3, etc.
    
    return false;
}

uint32_t DifficultySaveManager::GetSaveFileVersion(const std::string &savePath)
{
    // Check if the save file exists
    if (!FileExists(savePath)) {
        return 0;
    }
    
    // Open the save file
    std::ifstream file(savePath, std::ios::binary);
    if (!file.is_open()) {
        return 0;
    }
    
    // Read the header
    char header[16];
    file.read(header, sizeof(header));
    
    // Check if the header is valid
    if (std::strncmp(header, "DYNDIFF", 7) != 0) {
        return 0;
    }
    
    // Read the version
    uint32_t version;
    file.read(reinterpret_cast<char*>(&version), sizeof(version));
    
    return version;
}

uint32_t DifficultySaveManager::GetCurrentVersion() const
{
    return DIFFICULTY_VERSION;
}

void DifficultySaveManager::SerializeDifficultyState(const Player &player, SerializedDifficultyState &state)
{
    // Get the difficulty manager
    const DifficultyManager &manager = DifficultyManager::GetInstance();
    
    // Get the difficulty state
    state.currentDifficulty = manager.GetCurrentDifficulty(player);
    state.targetDifficulty = manager.GetTargetDifficulty(player);
    state.rawGearLevel = manager.GetRawGearLevel(player);
    
    // Get the transition parameters
    const DifficultyTransitionParams &params = manager.GetTransitionParams();
    
    // Set the history size (capped at 20)
    state.historySize = std::min(params.historySize, 20);
    
    // TODO: Get the actual history, last update time, and overpower end time
    // These would require exposing more of the internal state of the difficulty manager
    state.lastUpdateTime = 0.0f;
    state.overpowerEndTime = 0.0f;
    
    // Fill the history with the current difficulty
    for (int i = 0; i < state.historySize; i++) {
        state.difficultyHistory[i] = state.currentDifficulty;
    }
    
    LogVerbose("Serialized difficulty state for player {}: current={}, target={}, raw={}",
        player.getId(), state.currentDifficulty, state.targetDifficulty, state.rawGearLevel);
}

void DifficultySaveManager::DeserializeDifficultyState(Player &player, const SerializedDifficultyState &state)
{
    // TODO: Restore the difficulty state in the difficulty manager
    // This would require exposing more of the internal state of the difficulty manager
    
    // For now, we'll just update the difficulty with the current values
    float gameTime = 0.0f; // TODO: Get actual game time
    DifficultyManager::GetInstance().UpdateDifficulty(player, gameTime);
    
    LogVerbose("Deserialized difficulty state for player {}: current={}, target={}, raw={}",
        player.getId(), state.currentDifficulty, state.targetDifficulty, state.rawGearLevel);
}

bool DifficultySaveManager::ReadStateFromSaveFile(const std::string &savePath, SerializedDifficultyState &state)
{
    // Check if the save file exists
    if (!FileExists(savePath)) {
        return false;
    }
    
    // Open the save file
    std::ifstream file(savePath, std::ios::binary);
    if (!file.is_open()) {
        LogError("Failed to open save file for reading: {}", savePath);
        return false;
    }
    
    // Read the header
    char header[16];
    file.read(header, sizeof(header));
    
    // Check if the header is valid
    if (std::strncmp(header, "DYNDIFF", 7) != 0) {
        LogError("Invalid header in save file: {}", savePath);
        return false;
    }
    
    // Read the version
    uint32_t version;
    file.read(reinterpret_cast<char*>(&version), sizeof(version));
    
    // Check if the version is supported
    if (version > DIFFICULTY_VERSION) {
        LogError("Unsupported version in save file: {} (current: {})", version, DIFFICULTY_VERSION);
        return false;
    }
    
    // Read the state
    file.read(reinterpret_cast<char*>(&state), sizeof(state));
    
    return true;
}

bool DifficultySaveManager::WriteStateToSaveFile(const std::string &savePath, const SerializedDifficultyState &state)
{
    // Open the save file
    std::ofstream file(savePath, std::ios::binary);
    if (!file.is_open()) {
        LogError("Failed to open save file for writing: {}", savePath);
        return false;
    }
    
    // Write the header
    const char header[16] = "DYNDIFF";
    file.write(header, sizeof(header));
    
    // Write the version
    uint32_t version = DIFFICULTY_VERSION;
    file.write(reinterpret_cast<const char*>(&version), sizeof(version));
    
    // Write the state
    file.write(reinterpret_cast<const char*>(&state), sizeof(state));
    
    return true;
}

} // namespace devilution
