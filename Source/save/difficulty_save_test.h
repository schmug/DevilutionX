/**
 * @file difficulty_save_test.h
 *
 * Utilities for testing save game compatibility with the difficulty system.
 */
#pragma once

#include <string>
#include <vector>

#include "player.h"
#include "save/difficulty_save.h"

namespace devilution {

/**
 * @brief Structure for save file test result
 */
struct SaveFileTestResult {
    std::string savePath;
    bool hasState;
    uint32_t version;
    bool loadSuccess;
    bool saveSuccess;
    bool migrationSuccess;
    SerializedDifficultyState state;
    std::string error;
};

/**
 * @brief Class for testing save game compatibility
 */
class DifficultySaveTest {
public:
    /**
     * @brief Gets the singleton instance
     * @return Reference to the singleton instance
     */
    static DifficultySaveTest& GetInstance();
    
    /**
     * @brief Initializes the test utilities
     */
    void Initialize();
    
    /**
     * @brief Tests loading a save file
     * @param savePath The path to the save file
     * @return The test result
     */
    SaveFileTestResult TestLoadSaveFile(const std::string &savePath);
    
    /**
     * @brief Tests saving to a save file
     * @param player The player to save state for
     * @param savePath The path to the save file
     * @return The test result
     */
    SaveFileTestResult TestSaveSaveFile(const Player &player, const std::string &savePath);
    
    /**
     * @brief Tests migrating a save file
     * @param savePath The path to the save file
     * @return The test result
     */
    SaveFileTestResult TestMigrateSaveFile(const std::string &savePath);
    
    /**
     * @brief Creates a test save file with the specified version
     * @param savePath The path to create the save file at
     * @param version The version to use (0 for no difficulty state)
     * @return True if creation was successful, false otherwise
     */
    bool CreateTestSaveFile(const std::string &savePath, uint32_t version);
    
    /**
     * @brief Scans a directory for save files
     * @param directory The directory to scan
     * @return A list of save file paths
     */
    std::vector<std::string> ScanForSaveFiles(const std::string &directory);
    
    /**
     * @brief Tests all save files in a directory
     * @param directory The directory to scan
     * @return A list of test results
     */
    std::vector<SaveFileTestResult> TestAllSaveFiles(const std::string &directory);
    
    /**
     * @brief Generates a report of test results
     * @param results The test results
     * @return A text report
     */
    std::string GenerateTestReport(const std::vector<SaveFileTestResult> &results);
    
private:
    DifficultySaveTest() = default;
    ~DifficultySaveTest() = default;
    
    DifficultySaveTest(const DifficultySaveTest&) = delete;
    DifficultySaveTest& operator=(const DifficultySaveTest&) = delete;
    
    /**
     * @brief Creates a mock player for testing
     * @return A mock player
     */
    Player CreateMockPlayer();
    
    // Flag to track initialization
    bool initialized = false;
};

} // namespace devilution
