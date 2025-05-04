/**
 * @file difficulty_save_test.cpp
 *
 * Implementation of utilities for testing save game compatibility with the difficulty system.
 */
#include "save/difficulty_save_test.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "utils/log.hpp"
#include "utils/file_util.h"

namespace devilution {

DifficultySaveTest& DifficultySaveTest::GetInstance()
{
    static DifficultySaveTest instance;
    return instance;
}

void DifficultySaveTest::Initialize()
{
    if (initialized) {
        return;
    }
    
    // Initialize the difficulty save manager
    DifficultySaveManager::GetInstance().Initialize();
    
    initialized = true;
    LogVerbose("Difficulty Save Test utilities initialized");
}

SaveFileTestResult DifficultySaveTest::TestLoadSaveFile(const std::string &savePath)
{
    SaveFileTestResult result;
    result.savePath = savePath;
    
    // Check if the save file has difficulty state
    DifficultySaveManager &manager = DifficultySaveManager::GetInstance();
    result.hasState = manager.HasDifficultyState(savePath);
    
    // Get the version
    result.version = manager.GetSaveFileVersion(savePath);
    
    // Create a mock player
    Player player = CreateMockPlayer();
    
    // Try to load the state
    try {
        result.loadSuccess = manager.LoadDifficultyState(player, savePath);
        
        // If loading was successful, get the state
        if (result.loadSuccess) {
            SerializedDifficultyState state;
            manager.SerializeDifficultyState(player, state);
            result.state = state;
        }
    } catch (const std::exception &e) {
        result.loadSuccess = false;
        result.error = e.what();
    }
    
    return result;
}

SaveFileTestResult DifficultySaveTest::TestSaveSaveFile(const Player &player, const std::string &savePath)
{
    SaveFileTestResult result;
    result.savePath = savePath;
    
    // Try to save the state
    DifficultySaveManager &manager = DifficultySaveManager::GetInstance();
    
    try {
        result.saveSuccess = manager.SaveDifficultyState(player, savePath);
        
        // If saving was successful, check if the file has state
        if (result.saveSuccess) {
            result.hasState = manager.HasDifficultyState(savePath);
            result.version = manager.GetSaveFileVersion(savePath);
            
            // Get the state
            SerializedDifficultyState state;
            manager.SerializeDifficultyState(player, state);
            result.state = state;
        }
    } catch (const std::exception &e) {
        result.saveSuccess = false;
        result.error = e.what();
    }
    
    return result;
}

SaveFileTestResult DifficultySaveTest::TestMigrateSaveFile(const std::string &savePath)
{
    SaveFileTestResult result;
    result.savePath = savePath;
    
    // Check if the save file has difficulty state
    DifficultySaveManager &manager = DifficultySaveManager::GetInstance();
    result.hasState = manager.HasDifficultyState(savePath);
    
    // Get the version
    result.version = manager.GetSaveFileVersion(savePath);
    
    // Try to migrate the save file
    try {
        result.migrationSuccess = manager.MigrateSaveFile(savePath);
        
        // If migration was successful, check the new version
        if (result.migrationSuccess) {
            result.version = manager.GetSaveFileVersion(savePath);
            result.hasState = manager.HasDifficultyState(savePath);
        }
    } catch (const std::exception &e) {
        result.migrationSuccess = false;
        result.error = e.what();
    }
    
    return result;
}

bool DifficultySaveTest::CreateTestSaveFile(const std::string &savePath, uint32_t version)
{
    // Create the directory if it doesn't exist
    std::filesystem::path path(savePath);
    std::filesystem::create_directories(path.parent_path());
    
    // Open the save file
    std::ofstream file(savePath, std::ios::binary);
    if (!file.is_open()) {
        LogError("Failed to create test save file: {}", savePath);
        return false;
    }
    
    // If version is 0, create a save file without difficulty state
    if (version == 0) {
        // Write some dummy data
        const char dummy[16] = "DUMMY";
        file.write(dummy, sizeof(dummy));
        return true;
    }
    
    // Write the header
    const char header[16] = "DYNDIFF";
    file.write(header, sizeof(header));
    
    // Write the version
    file.write(reinterpret_cast<const char*>(&version), sizeof(version));
    
    // Write a dummy state
    SerializedDifficultyState state;
    state.currentDifficulty = 10.0f;
    state.targetDifficulty = 15.0f;
    state.rawGearLevel = 12.0f;
    state.lastUpdateTime = 100.0f;
    state.overpowerEndTime = 0.0f;
    state.historySize = 5;
    
    for (int i = 0; i < state.historySize; i++) {
        state.difficultyHistory[i] = 10.0f + i;
    }
    
    file.write(reinterpret_cast<const char*>(&state), sizeof(state));
    
    return true;
}

std::vector<std::string> DifficultySaveTest::ScanForSaveFiles(const std::string &directory)
{
    std::vector<std::string> saveFiles;
    
    // Check if the directory exists
    if (!std::filesystem::exists(directory)) {
        LogError("Directory does not exist: {}", directory);
        return saveFiles;
    }
    
    // Scan the directory for save files
    for (const auto &entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string extension = entry.path().extension().string();
            
            // Check if this is a save file (adjust as needed)
            if (extension == ".sv" || extension == ".sav" || extension == ".save") {
                saveFiles.push_back(entry.path().string());
            }
        }
    }
    
    return saveFiles;
}

std::vector<SaveFileTestResult> DifficultySaveTest::TestAllSaveFiles(const std::string &directory)
{
    std::vector<SaveFileTestResult> results;
    
    // Scan for save files
    std::vector<std::string> saveFiles = ScanForSaveFiles(directory);
    
    // Test each save file
    for (const auto &savePath : saveFiles) {
        // Test loading
        SaveFileTestResult loadResult = TestLoadSaveFile(savePath);
        results.push_back(loadResult);
        
        // If loading failed, try migration
        if (!loadResult.loadSuccess) {
            SaveFileTestResult migrateResult = TestMigrateSaveFile(savePath);
            results.push_back(migrateResult);
            
            // If migration succeeded, try loading again
            if (migrateResult.migrationSuccess) {
                SaveFileTestResult reloadResult = TestLoadSaveFile(savePath);
                results.push_back(reloadResult);
            }
        }
    }
    
    return results;
}

std::string DifficultySaveTest::GenerateTestReport(const std::vector<SaveFileTestResult> &results)
{
    std::stringstream report;
    report << "Save File Compatibility Test Report\n";
    report << "==================================\n\n";
    
    // Count successes and failures
    int totalFiles = 0;
    int hasStateCount = 0;
    int loadSuccessCount = 0;
    int saveSuccessCount = 0;
    int migrationSuccessCount = 0;
    int migrationCount = 0;
    
    for (const auto &result : results) {
        if (result.savePath.find("TestLoad") != std::string::npos) {
            totalFiles++;
            
            if (result.hasState) {
                hasStateCount++;
            }
            
            if (result.loadSuccess) {
                loadSuccessCount++;
            }
        } else if (result.savePath.find("TestSave") != std::string::npos) {
            if (result.saveSuccess) {
                saveSuccessCount++;
            }
        } else if (result.savePath.find("TestMigrate") != std::string::npos) {
            migrationCount++;
            
            if (result.migrationSuccess) {
                migrationSuccessCount++;
            }
        }
    }
    
    // Add summary
    report << "Summary:\n";
    report << "- Total save files tested: " << totalFiles << "\n";
    report << "- Files with difficulty state: " << hasStateCount << " (" << (hasStateCount * 100 / std::max(1, totalFiles)) << "%)\n";
    report << "- Successful loads: " << loadSuccessCount << " (" << (loadSuccessCount * 100 / std::max(1, totalFiles)) << "%)\n";
    report << "- Successful saves: " << saveSuccessCount << "\n";
    report << "- Migrations attempted: " << migrationCount << "\n";
    report << "- Successful migrations: " << migrationSuccessCount << " (" << (migrationSuccessCount * 100 / std::max(1, migrationCount)) << "%)\n\n";
    
    // Add details for each file
    report << "Detailed Results:\n";
    
    for (const auto &result : results) {
        report << "File: " << result.savePath << "\n";
        report << "- Has state: " << (result.hasState ? "Yes" : "No") << "\n";
        report << "- Version: " << result.version << "\n";
        
        if (result.savePath.find("TestLoad") != std::string::npos) {
            report << "- Load success: " << (result.loadSuccess ? "Yes" : "No") << "\n";
            
            if (!result.loadSuccess && !result.error.empty()) {
                report << "- Error: " << result.error << "\n";
            }
            
            if (result.loadSuccess) {
                report << "- Current difficulty: " << result.state.currentDifficulty << "\n";
                report << "- Target difficulty: " << result.state.targetDifficulty << "\n";
                report << "- Raw gear level: " << result.state.rawGearLevel << "\n";
            }
        } else if (result.savePath.find("TestSave") != std::string::npos) {
            report << "- Save success: " << (result.saveSuccess ? "Yes" : "No") << "\n";
            
            if (!result.saveSuccess && !result.error.empty()) {
                report << "- Error: " << result.error << "\n";
            }
        } else if (result.savePath.find("TestMigrate") != std::string::npos) {
            report << "- Migration success: " << (result.migrationSuccess ? "Yes" : "No") << "\n";
            
            if (!result.migrationSuccess && !result.error.empty()) {
                report << "- Error: " << result.error << "\n";
            }
            
            if (result.migrationSuccess) {
                report << "- New version: " << result.version << "\n";
            }
        }
        
        report << "\n";
    }
    
    return report.str();
}

Player DifficultySaveTest::CreateMockPlayer()
{
    // Create a basic player for testing
    Player player;
    
    // Set a unique ID
    player._pName = "TestPlayer";
    
    return player;
}

} // namespace devilution
