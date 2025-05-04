#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>

#include "player.h"
#include "monsters.h"
#include "gear/gear_score.h"
#include "gear/gear_config.h"
#include "difficulty/difficulty_manager.h"
#include "difficulty/difficulty_wave.h"
#include "difficulty/difficulty_debug.h"
#include "monsters/monster_scaling.h"
#include "multiplayer/mp_difficulty.h"
#include "save/difficulty_save.h"
#include "utils/log.hpp"

namespace devilution {
namespace {

// Mock player for testing
class MockPlayer : public Player {
public:
    MockPlayer(int level, float gearScore, int playerId = 0)
        : level(level), gearScore(gearScore), playerId(playerId)
    {
        _pLevel = level;
        _pName = "TestPlayer" + std::to_string(playerId);
    }
    
    int GetLevel() const { return level; }
    float GetGearScore() const { return gearScore; }
    int GetPlayerId() const { return playerId; }
    
    void SetGearScore(float newGearScore) { gearScore = newGearScore; }
    
private:
    int level;
    float gearScore;
    int playerId;
};

// Mock monster for testing
class MockMonster : public MonsterData {
public:
    MockMonster(monster_id type, int baseHP, int baseDamage, int baseAC)
    {
        _mtype = type;
        _mMaxHP = baseHP;
        _mHitPoints = baseHP;
        _mMinDamage = baseDamage / 2;
        _mMaxDamage = baseDamage;
        _mAC = baseAC;
    }
};

class DifficultyIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize logging for tests
        LogInit();
        
        // Initialize all subsystems
        gearManager = &GearLevelManager::GetInstance();
        gearManager->Initialize();
        
        difficultyManager = &DifficultyManager::GetInstance();
        difficultyManager->Initialize();
        
        difficultyWave = &DifficultyWave::GetInstance();
        difficultyWave->Initialize();
        
        monsterScaling = &MonsterScaling::GetInstance();
        monsterScaling->Initialize();
        
        mpDifficultyManager = &MpDifficultyManager::GetInstance();
        mpDifficultyManager->Initialize();
        
        difficultySaveManager = &DifficultySaveManager::GetInstance();
        
        // Create test players with different gear levels
        players.push_back(std::make_unique<MockPlayer>(1, 10.0f, 0));  // Low level, low gear
        players.push_back(std::make_unique<MockPlayer>(10, 30.0f, 1)); // Mid level, mid gear
        players.push_back(std::make_unique<MockPlayer>(30, 80.0f, 2)); // High level, high gear
        players.push_back(std::make_unique<MockPlayer>(5, 50.0f, 3));  // Low level, high gear (outlier)
        
        // Create test monsters
        monsters.push_back(std::make_unique<MockMonster>(MT_ZOMBIE, 100, 20, 10));      // Basic monster
        monsters.push_back(std::make_unique<MockMonster>(MT_GOLEM, 300, 40, 30));       // Mid-tier monster
        monsters.push_back(std::make_unique<MockMonster>(MT_DIABLO, 1000, 100, 100));   // Boss monster
    }

    void TearDown() override {
        // Clean up after tests
        players.clear();
        monsters.clear();
        LogClose();
    }
    
    // Helper method to get a test player
    MockPlayer* GetPlayer(int index) {
        if (index >= 0 && index < static_cast<int>(players.size())) {
            return static_cast<MockPlayer*>(players[index].get());
        }
        return nullptr;
    }
    
    // Helper method to get a test monster
    MockMonster* GetMonster(int index) {
        if (index >= 0 && index < static_cast<int>(monsters.size())) {
            return static_cast<MockMonster*>(monsters[index].get());
        }
        return nullptr;
    }
    
    // Helper method to simulate game time passing
    void SimulateGameTime(float seconds) {
        // Update difficulty for all players
        float timeStep = 0.1f; // 100ms time step
        int steps = static_cast<int>(seconds / timeStep);
        
        for (int i = 0; i < steps; i++) {
            float gameTime = currentGameTime + (i * timeStep);
            
            // Update difficulty for all players
            for (auto& player : players) {
                difficultyManager->UpdateDifficulty(*player, gameTime);
            }
            
            // Update wave effects
            difficultyWave->Update(gameTime);
            
            // Small sleep to simulate real time passing
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        currentGameTime += seconds;
    }

    GearLevelManager* gearManager;
    DifficultyManager* difficultyManager;
    DifficultyWave* difficultyWave;
    MonsterScaling* monsterScaling;
    MpDifficultyManager* mpDifficultyManager;
    DifficultySaveManager* difficultySaveManager;
    
    std::vector<std::unique_ptr<MockPlayer>> players;
    std::vector<std::unique_ptr<MockMonster>> monsters;
    
    float currentGameTime = 0.0f;
};

TEST_F(DifficultyIntegrationTest, GearChangesAffectDifficulty) {
    // Test that changes in gear level affect difficulty
    
    // Get a test player
    MockPlayer* player = GetPlayer(1); // Mid level, mid gear player
    
    // Get initial difficulty
    float initialDifficulty = difficultyManager->CalculateDifficulty(*player);
    
    // Increase gear score
    float newGearScore = player->GetGearScore() * 2.0f;
    player->SetGearScore(newGearScore);
    
    // Allow time for difficulty to transition
    SimulateGameTime(5.0f);
    
    // Get new difficulty
    float newDifficulty = difficultyManager->GetCurrentDifficulty(*player);
    
    // Verify that difficulty increased
    EXPECT_GT(newDifficulty, initialDifficulty) << "Increasing gear score should increase difficulty";
}

TEST_F(DifficultyIntegrationTest, DifficultyAffectsMonsterScaling) {
    // Test that difficulty level affects monster scaling
    
    // Get a test player and monster
    MockPlayer* player = GetPlayer(1); // Mid level, mid gear player
    MockMonster* monster = GetMonster(0); // Basic monster
    
    // Get initial monster stats
    int initialHP = monster->_mMaxHP;
    int initialMinDamage = monster->_mMinDamage;
    int initialMaxDamage = monster->_mMaxDamage;
    int initialAC = monster->_mAC;
    
    // Calculate player difficulty
    float difficulty = difficultyManager->CalculateDifficulty(*player);
    
    // Scale monster based on difficulty
    MonsterData scaledMonster = monsterScaling->ScaleMonster(*monster, difficulty);
    
    // Verify that monster stats were scaled
    EXPECT_GT(scaledMonster._mMaxHP, initialHP) << "Monster HP should increase with difficulty";
    EXPECT_GT(scaledMonster._mMinDamage, initialMinDamage) << "Monster min damage should increase with difficulty";
    EXPECT_GT(scaledMonster._mMaxDamage, initialMaxDamage) << "Monster max damage should increase with difficulty";
    EXPECT_GT(scaledMonster._mAC, initialAC) << "Monster armor class should increase with difficulty";
}

TEST_F(DifficultyIntegrationTest, WaveEffectsModifyDifficulty) {
    // Test that wave effects modify difficulty over time
    
    // Get a test player
    MockPlayer* player = GetPlayer(1); // Mid level, mid gear player
    
    // Set up wave parameters for testing
    WaveParameters waveParams;
    waveParams.patternType = WavePatternType::Sine;
    waveParams.amplitude = 0.3f;
    waveParams.frequency = 0.5f;
    waveParams.phase = 0.0f;
    waveParams.baseline = 1.0f;
    difficultyWave->SetWaveParameters(waveParams);
    
    // Calculate base difficulty
    float baseDifficulty = difficultyManager->CalculateDifficulty(*player);
    
    // Sample difficulty at different time points
    std::vector<float> difficulties;
    for (int i = 0; i < 5; i++) {
        float gameTime = i * 0.5f; // Sample every half second
        float modifiedDifficulty = difficultyWave->ApplyWaveEffect(baseDifficulty, gameTime);
        difficulties.push_back(modifiedDifficulty);
    }
    
    // Verify that difficulty varies over time
    bool difficultyVaries = false;
    for (size_t i = 1; i < difficulties.size(); i++) {
        if (std::abs(difficulties[i] - difficulties[i-1]) > 1.0f) {
            difficultyVaries = true;
            break;
        }
    }
    
    EXPECT_TRUE(difficultyVaries) << "Difficulty should vary over time due to wave effects";
}

TEST_F(DifficultyIntegrationTest, MultiplayerDifficultyCalculation) {
    // Test multiplayer difficulty calculation
    
    // Set up multiplayer session with multiple players
    std::vector<Player*> mpPlayers;
    for (auto& player : players) {
        mpPlayers.push_back(player.get());
    }
    
    // Calculate multiplayer difficulty using different methods
    float avgDifficulty = mpDifficultyManager->CalculateAverageDifficulty(mpPlayers);
    float maxDifficulty = mpDifficultyManager->CalculateMaximumDifficulty(mpPlayers);
    float weightedDifficulty = mpDifficultyManager->CalculateWeightedDifficulty(mpPlayers);
    
    // Verify that multiplayer difficulty is calculated correctly
    EXPECT_GT(avgDifficulty, 0.0f) << "Average difficulty should be positive";
    EXPECT_GT(maxDifficulty, avgDifficulty) << "Maximum difficulty should be greater than average difficulty";
    EXPECT_GT(weightedDifficulty, 0.0f) << "Weighted difficulty should be positive";
    
    // Verify that adding a high-level player increases difficulty
    MockPlayer highLevelPlayer(50, 100.0f, 4); // Very high level, very high gear
    mpPlayers.push_back(&highLevelPlayer);
    
    float newAvgDifficulty = mpDifficultyManager->CalculateAverageDifficulty(mpPlayers);
    float newMaxDifficulty = mpDifficultyManager->CalculateMaximumDifficulty(mpPlayers);
    float newWeightedDifficulty = mpDifficultyManager->CalculateWeightedDifficulty(mpPlayers);
    
    EXPECT_GT(newAvgDifficulty, avgDifficulty) << "Adding a high-level player should increase average difficulty";
    EXPECT_GT(newMaxDifficulty, maxDifficulty) << "Adding a high-level player should increase maximum difficulty";
    EXPECT_GT(newWeightedDifficulty, weightedDifficulty) << "Adding a high-level player should increase weighted difficulty";
}

TEST_F(DifficultyIntegrationTest, SaveAndLoadDifficultyState) {
    // Test saving and loading difficulty state
    
    // Get a test player
    MockPlayer* player = GetPlayer(2); // High level, high gear player
    
    // Set up initial difficulty state
    float initialDifficulty = 50.0f;
    difficultyManager->SetCurrentDifficulty(*player, initialDifficulty);
    
    // Create a temporary save file
    std::string savePath = "temp_difficulty_save.dat";
    
    // Save difficulty state
    bool saveSuccess = difficultySaveManager->SaveDifficultyState(*player, savePath);
    EXPECT_TRUE(saveSuccess) << "Saving difficulty state should succeed";
    
    // Change difficulty
    difficultyManager->SetCurrentDifficulty(*player, initialDifficulty * 2.0f);
    
    // Load difficulty state
    bool loadSuccess = difficultySaveManager->LoadDifficultyState(*player, savePath);
    EXPECT_TRUE(loadSuccess) << "Loading difficulty state should succeed";
    
    // Verify that difficulty was restored
    float loadedDifficulty = difficultyManager->GetCurrentDifficulty(*player);
    EXPECT_NEAR(loadedDifficulty, initialDifficulty, 0.1f) << "Loaded difficulty should match saved difficulty";
    
    // Clean up temporary file
    std::remove(savePath.c_str());
}

TEST_F(DifficultyIntegrationTest, CompleteSystemIntegration) {
    // Test the complete system integration
    
    // Get test players and monsters
    MockPlayer* player1 = GetPlayer(0); // Low level, low gear
    MockPlayer* player2 = GetPlayer(2); // High level, high gear
    MockMonster* basicMonster = GetMonster(0); // Basic monster
    MockMonster* bossMonster = GetMonster(2); // Boss monster
    
    // Set up wave parameters
    WaveParameters waveParams;
    waveParams.patternType = WavePatternType::Compound;
    waveParams.amplitude = 0.2f;
    waveParams.frequency = 0.1f;
    waveParams.phase = 0.0f;
    waveParams.baseline = 1.0f;
    difficultyWave->SetWaveParameters(waveParams);
    
    // Calculate initial difficulties
    float difficulty1 = difficultyManager->CalculateDifficulty(*player1);
    float difficulty2 = difficultyManager->CalculateDifficulty(*player2);
    
    // Apply wave effects
    float modifiedDifficulty1 = difficultyWave->ApplyWaveEffect(difficulty1, currentGameTime);
    float modifiedDifficulty2 = difficultyWave->ApplyWaveEffect(difficulty2, currentGameTime);
    
    // Scale monsters for each player
    MonsterData scaledBasicMonster1 = monsterScaling->ScaleMonster(*basicMonster, modifiedDifficulty1);
    MonsterData scaledBasicMonster2 = monsterScaling->ScaleMonster(*basicMonster, modifiedDifficulty2);
    MonsterData scaledBossMonster1 = monsterScaling->ScaleMonster(*bossMonster, modifiedDifficulty1);
    MonsterData scaledBossMonster2 = monsterScaling->ScaleMonster(*bossMonster, modifiedDifficulty2);
    
    // Verify that monsters are scaled appropriately for each player
    EXPECT_GT(scaledBasicMonster2._mMaxHP, scaledBasicMonster1._mMaxHP) << "Basic monster should be stronger for high-level player";
    EXPECT_GT(scaledBossMonster2._mMaxHP, scaledBossMonster1._mMaxHP) << "Boss monster should be stronger for high-level player";
    
    // Simulate multiplayer session
    std::vector<Player*> mpPlayers = {player1, player2};
    float mpDifficulty = mpDifficultyManager->CalculateWeightedDifficulty(mpPlayers);
    
    // Scale monsters for multiplayer
    MonsterData mpScaledBasicMonster = monsterScaling->ScaleMonster(*basicMonster, mpDifficulty);
    MonsterData mpScaledBossMonster = monsterScaling->ScaleMonster(*bossMonster, mpDifficulty);
    
    // Verify that multiplayer scaling is between individual player scalings
    EXPECT_GE(mpScaledBasicMonster._mMaxHP, scaledBasicMonster1._mMaxHP) << "Multiplayer basic monster should be at least as strong as for low-level player";
    EXPECT_LE(mpScaledBasicMonster._mMaxHP, scaledBasicMonster2._mMaxHP) << "Multiplayer basic monster should be at most as strong as for high-level player";
    
    // Simulate game time passing and difficulty transitions
    player1->SetGearScore(player1->GetGearScore() * 1.5f); // Increase gear score
    SimulateGameTime(10.0f);
    
    // Verify that difficulty has transitioned
    float newDifficulty1 = difficultyManager->GetCurrentDifficulty(*player1);
    EXPECT_GT(newDifficulty1, difficulty1) << "Difficulty should increase after gear score increase";
    
    // Save and load difficulty state
    std::string savePath = "temp_mp_difficulty_save.dat";
    difficultySaveManager->SaveDifficultyState(*player1, savePath);
    
    // Change difficulty
    difficultyManager->SetCurrentDifficulty(*player1, difficulty1);
    
    // Load saved state
    difficultySaveManager->LoadDifficultyState(*player1, savePath);
    
    // Verify that loaded state matches saved state
    float loadedDifficulty1 = difficultyManager->GetCurrentDifficulty(*player1);
    EXPECT_NEAR(loadedDifficulty1, newDifficulty1, 0.1f) << "Loaded difficulty should match saved difficulty";
    
    // Clean up temporary file
    std::remove(savePath.c_str());
}

} // namespace
} // namespace devilution
