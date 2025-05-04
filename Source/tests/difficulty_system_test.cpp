#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <memory>

#include "player.h"
#include "monsters.h"
#include "gear/gear_score.h"
#include "gear/gear_config.h"
#include "difficulty/difficulty_manager.h"
#include "difficulty/difficulty_wave.h"
#include "monsters/monster_scaling.h"
#include "utils/log.hpp"

namespace devilution {
namespace {

// Mock player for testing
class MockPlayer : public Player {
public:
    MockPlayer(int level, float gearScore)
        : level(level), gearScore(gearScore)
    {
        _pLevel = level;
    }
    
    int GetLevel() const { return level; }
    float GetGearScore() const { return gearScore; }
    
private:
    int level;
    float gearScore;
};

class DifficultySystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize logging for tests
        LogInit();
        
        // Initialize the difficulty manager
        difficultyManager = &DifficultyManager::GetInstance();
        difficultyManager->Initialize();
        
        // Initialize the difficulty wave system
        difficultyWave = &DifficultyWave::GetInstance();
        difficultyWave->Initialize();
        
        // Initialize the monster scaling system
        monsterScaling = &MonsterScaling::GetInstance();
        monsterScaling->Initialize();
        
        // Create test players with different gear levels
        players.push_back(std::make_unique<MockPlayer>(1, 10.0f));  // Low level, low gear
        players.push_back(std::make_unique<MockPlayer>(10, 30.0f)); // Mid level, mid gear
        players.push_back(std::make_unique<MockPlayer>(30, 80.0f)); // High level, high gear
        players.push_back(std::make_unique<MockPlayer>(5, 50.0f));  // Low level, high gear (outlier)
        players.push_back(std::make_unique<MockPlayer>(25, 20.0f)); // High level, low gear (outlier)
    }

    void TearDown() override {
        // Clean up after tests
        players.clear();
        LogClose();
    }
    
    // Helper method to get a test player
    MockPlayer* GetPlayer(int index) {
        if (index >= 0 && index < static_cast<int>(players.size())) {
            return static_cast<MockPlayer*>(players[index].get());
        }
        return nullptr;
    }

    DifficultyManager* difficultyManager;
    DifficultyWave* difficultyWave;
    MonsterScaling* monsterScaling;
    std::vector<std::unique_ptr<MockPlayer>> players;
};

TEST_F(DifficultySystemTest, CalculateDifficultyReturnsExpectedValues) {
    // Test that the difficulty calculation returns expected values for different players
    
    // Low level, low gear player should have low difficulty
    float difficulty1 = difficultyManager->CalculateDifficulty(*GetPlayer(0));
    EXPECT_LT(difficulty1, 20.0f) << "Low level, low gear player should have low difficulty";
    
    // Mid level, mid gear player should have medium difficulty
    float difficulty2 = difficultyManager->CalculateDifficulty(*GetPlayer(1));
    EXPECT_GE(difficulty2, 20.0f);
    EXPECT_LT(difficulty2, 60.0f) << "Mid level, mid gear player should have medium difficulty";
    
    // High level, high gear player should have high difficulty
    float difficulty3 = difficultyManager->CalculateDifficulty(*GetPlayer(2));
    EXPECT_GE(difficulty3, 60.0f) << "High level, high gear player should have high difficulty";
    
    // Low level, high gear player (outlier) should have higher difficulty than expected for level
    float difficulty4 = difficultyManager->CalculateDifficulty(*GetPlayer(3));
    EXPECT_GT(difficulty4, difficulty1) << "Low level, high gear player should have higher difficulty than low level, low gear player";
    
    // High level, low gear player (outlier) should have lower difficulty than expected for level
    float difficulty5 = difficultyManager->CalculateDifficulty(*GetPlayer(4));
    EXPECT_LT(difficulty5, difficulty3) << "High level, low gear player should have lower difficulty than high level, high gear player";
}

TEST_F(DifficultySystemTest, DifficultyTransitionSmoothingWorks) {
    // Test that difficulty transitions are smoothed
    
    // Get a test player
    MockPlayer* player = GetPlayer(1);
    
    // Set initial difficulty
    float initialDifficulty = 30.0f;
    difficultyManager->SetCurrentDifficulty(*player, initialDifficulty);
    
    // Trigger a difficulty update with a higher target difficulty
    float targetDifficulty = 50.0f;
    difficultyManager->SetTargetDifficulty(*player, targetDifficulty);
    
    // Update difficulty with a small time step
    float gameTime = 1.0f;
    difficultyManager->UpdateDifficulty(*player, gameTime);
    
    // Get the current difficulty after the update
    float currentDifficulty = difficultyManager->GetCurrentDifficulty(*player);
    
    // The current difficulty should be between the initial and target difficulties
    EXPECT_GT(currentDifficulty, initialDifficulty) << "Difficulty should increase after update";
    EXPECT_LT(currentDifficulty, targetDifficulty) << "Difficulty should not reach target immediately";
}

TEST_F(DifficultySystemTest, DifficultyWaveEffectsModifyDifficulty) {
    // Test that difficulty wave effects modify the base difficulty
    
    // Set up wave parameters for testing
    WaveParameters waveParams;
    waveParams.patternType = WavePatternType::Sine;
    waveParams.amplitude = 0.2f;
    waveParams.frequency = 1.0f; // Higher frequency for faster testing
    waveParams.phase = 0.0f;
    waveParams.baseline = 1.0f;
    difficultyWave->SetWaveParameters(waveParams);
    
    // Base difficulty to test with
    float baseDifficulty = 50.0f;
    
    // Get wave-modified difficulty at different time points
    float gameTime1 = 0.0f;
    float modifiedDifficulty1 = difficultyWave->ApplyWaveEffect(baseDifficulty, gameTime1);
    
    float gameTime2 = 0.25f; // Quarter of a sine wave cycle
    float modifiedDifficulty2 = difficultyWave->ApplyWaveEffect(baseDifficulty, gameTime2);
    
    float gameTime3 = 0.5f; // Half of a sine wave cycle
    float modifiedDifficulty3 = difficultyWave->ApplyWaveEffect(baseDifficulty, gameTime3);
    
    // Verify that the difficulty varies over time
    EXPECT_NE(modifiedDifficulty1, modifiedDifficulty2) << "Difficulty should change over time with wave effects";
    EXPECT_NE(modifiedDifficulty2, modifiedDifficulty3) << "Difficulty should change over time with wave effects";
    
    // For a sine wave, the difficulty at time 0.5 should be less than at time 0
    EXPECT_LT(modifiedDifficulty3, modifiedDifficulty1) << "Difficulty should decrease at half cycle for sine wave";
}

TEST_F(DifficultySystemTest, MonsterScalingAdjustsMonsterStats) {
    // Test that monster stats are adjusted based on difficulty level
    
    // Create a test monster
    MonsterData monster;
    monster._mMaxHP = 100;
    monster._mHitPoints = 100;
    monster._mMinDamage = 10;
    monster._mMaxDamage = 20;
    monster._mAC = 50;
    
    // Scale the monster at different difficulty levels
    float lowDifficulty = 10.0f;
    MonsterData scaledMonsterLow = monsterScaling->ScaleMonster(monster, lowDifficulty);
    
    float highDifficulty = 80.0f;
    MonsterData scaledMonsterHigh = monsterScaling->ScaleMonster(monster, highDifficulty);
    
    // Verify that higher difficulty results in stronger monsters
    EXPECT_GT(scaledMonsterHigh._mMaxHP, scaledMonsterLow._mMaxHP) << "Higher difficulty should increase monster HP";
    EXPECT_GT(scaledMonsterHigh._mMinDamage, scaledMonsterLow._mMinDamage) << "Higher difficulty should increase monster min damage";
    EXPECT_GT(scaledMonsterHigh._mMaxDamage, scaledMonsterLow._mMaxDamage) << "Higher difficulty should increase monster max damage";
    EXPECT_GT(scaledMonsterHigh._mAC, scaledMonsterLow._mAC) << "Higher difficulty should increase monster armor class";
}

TEST_F(DifficultySystemTest, IntegrationTest) {
    // Test that all components work together correctly
    
    // Get a test player
    MockPlayer* player = GetPlayer(2); // High level, high gear player
    
    // Calculate the player's difficulty
    float difficulty = difficultyManager->CalculateDifficulty(*player);
    
    // Apply wave effects to the difficulty
    float gameTime = 1.0f;
    float modifiedDifficulty = difficultyWave->ApplyWaveEffect(difficulty, gameTime);
    
    // Create a test monster
    MonsterData monster;
    monster._mMaxHP = 100;
    monster._mHitPoints = 100;
    monster._mMinDamage = 10;
    monster._mMaxDamage = 20;
    monster._mAC = 50;
    
    // Scale the monster based on the modified difficulty
    MonsterData scaledMonster = monsterScaling->ScaleMonster(monster, modifiedDifficulty);
    
    // Verify that the monster has been scaled
    EXPECT_NE(scaledMonster._mMaxHP, monster._mMaxHP) << "Monster HP should be scaled based on difficulty";
    EXPECT_NE(scaledMonster._mMinDamage, monster._mMinDamage) << "Monster min damage should be scaled based on difficulty";
    EXPECT_NE(scaledMonster._mMaxDamage, monster._mMaxDamage) << "Monster max damage should be scaled based on difficulty";
    EXPECT_NE(scaledMonster._mAC, monster._mAC) << "Monster armor class should be scaled based on difficulty";
}

} // namespace
} // namespace devilution
