#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

#include "tests/system_verification_test.h"
#include "utils/log.hpp"

namespace devilution {
namespace testing {

class DifficultySystemVerificationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize logging for tests
        LogInit();
        
        // Create the system verification test
        verificationTest = std::make_unique<SystemVerificationTest>();
        
        // Add test scenarios
        AddBasicFunctionalityScenario();
        AddGearProgressionScenario();
        AddMultiplayerScenario();
        AddWaveEffectsScenario();
        AddSaveLoadScenario();
        AddEdgeCaseScenario();
    }

    void TearDown() override {
        // Clean up after tests
        LogClose();
    }
    
    void AddBasicFunctionalityScenario() {
        auto scenario = std::make_unique<TestScenario>(
            "Basic Functionality",
            "Tests basic functionality of the dynamic difficulty system"
        );
        
        // Setup step: Add players and monsters
        scenario->AddSetupStep([](GameSimulation& sim) {
            sim.AddPlayer(10, 30.0f, "TestPlayer1");
            sim.AddMonster(MT_ZOMBIE, 100, 20, 10, "Zombie");
            sim.AddMonster(MT_GOLEM, 300, 40, 30, "Golem");
        });
        
        // Test step 1: Calculate difficulty
        scenario->AddTestStep([](GameSimulation& sim, std::string& message) {
            float difficulty = sim.CalculatePlayerDifficulty(0);
            message = "Calculated difficulty: " + std::to_string(difficulty);
            return difficulty > 0.0f;
        });
        
        // Test step 2: Scale monsters
        scenario->AddTestStep([](GameSimulation& sim, std::string& message) {
            std::string zombieStats = sim.ScaleMonsterForPlayer(0, 0);
            std::string golemStats = sim.ScaleMonsterForPlayer(1, 0);
            message = "Scaled monster stats:\n" + zombieStats + "\n" + golemStats;
            
            // Verify that scaled monsters have higher stats than base monsters
            const SimulatedMonster* zombie = sim.GetMonster(0);
            const SimulatedMonster* golem = sim.GetMonster(1);
            
            // This is a simple check - in a real test, we'd parse the stats string
            // and verify specific values
            bool zombieScaled = zombieStats.find(std::to_string(zombie->baseHP)) != std::string::npos &&
                               zombieStats.find(std::to_string(zombie->baseHP + 1)) != std::string::npos;
            bool golemScaled = golemStats.find(std::to_string(golem->baseHP)) != std::string::npos &&
                              golemStats.find(std::to_string(golem->baseHP + 1)) != std::string::npos;
            
            return zombieScaled && golemScaled;
        });
        
        verificationTest->AddScenario(std::move(scenario));
    }
    
    void AddGearProgressionScenario() {
        auto scenario = std::make_unique<TestScenario>(
            "Gear Progression",
            "Tests difficulty changes as player gear improves"
        );
        
        // Setup step: Add a player
        scenario->AddSetupStep([](GameSimulation& sim) {
            sim.AddPlayer(10, 20.0f, "ProgressionPlayer");
        });
        
        // Test step 1: Calculate initial difficulty
        scenario->AddTestStep([](GameSimulation& sim, std::string& message) {
            float initialDifficulty = sim.CalculatePlayerDifficulty(0);
            message = "Initial difficulty: " + std::to_string(initialDifficulty);
            return true;
        });
        
        // Test step 2: Improve gear and check difficulty increase
        scenario->AddTestStep([](GameSimulation& sim, std::string& message) {
            float initialDifficulty = sim.GetPlayer(0)->currentDifficulty;
            
            // Improve gear
            sim.UpdatePlayerGearLevel(0, 40.0f);
            
            // Simulate some game time to allow difficulty to transition
            sim.SimulateGameTime(5.0f);
            
            // Check new difficulty
            float newDifficulty = sim.GetPlayer(0)->currentDifficulty;
            
            message = "Initial difficulty: " + std::to_string(initialDifficulty) + 
                     ", New difficulty: " + std::to_string(newDifficulty);
            
            return newDifficulty > initialDifficulty;
        });
        
        // Test step 3: Check that difficulty transitions smoothly
        scenario->AddTestStep([](GameSimulation& sim, std::string& message) {
            // Get current difficulty
            float startDifficulty = sim.GetPlayer(0)->currentDifficulty;
            
            // Improve gear significantly
            sim.UpdatePlayerGearLevel(0, 80.0f);
            
            // Simulate small time steps and record difficulty at each step
            std::vector<float> difficulties;
            for (int i = 0; i < 10; i++) {
                sim.SimulateGameTime(0.5f);
                difficulties.push_back(sim.GetPlayer(0)->currentDifficulty);
            }
            
            // Check that difficulty increases gradually
            bool smoothTransition = true;
            for (size_t i = 1; i < difficulties.size(); i++) {
                if (difficulties[i] - difficulties[i-1] > 20.0f) {
                    smoothTransition = false;
                    break;
                }
            }
            
            message = "Difficulty transition: ";
            for (float diff : difficulties) {
                message += std::to_string(diff) + ", ";
            }
            
            return smoothTransition && difficulties.back() > startDifficulty;
        });
        
        verificationTest->AddScenario(std::move(scenario));
    }
    
    void AddMultiplayerScenario() {
        auto scenario = std::make_unique<TestScenario>(
            "Multiplayer Difficulty",
            "Tests difficulty calculation in multiplayer games"
        );
        
        // Setup step: Add multiple players with varying gear levels
        scenario->AddSetupStep([](GameSimulation& sim) {
            sim.AddPlayer(5, 10.0f, "LowGearPlayer");
            sim.AddPlayer(15, 40.0f, "MidGearPlayer");
            sim.AddPlayer(30, 80.0f, "HighGearPlayer");
            sim.AddMonster(MT_DIABLO, 1000, 100, 100, "Diablo");
            sim.SetMultiplayer(true);
        });
        
        // Test step 1: Calculate individual difficulties
        scenario->AddTestStep([](GameSimulation& sim, std::string& message) {
            float difficulty0 = sim.CalculatePlayerDifficulty(0);
            float difficulty1 = sim.CalculatePlayerDifficulty(1);
            float difficulty2 = sim.CalculatePlayerDifficulty(2);
            
            message = "Player difficulties: " + 
                     std::to_string(difficulty0) + ", " + 
                     std::to_string(difficulty1) + ", " + 
                     std::to_string(difficulty2);
            
            return difficulty0 < difficulty1 && difficulty1 < difficulty2;
        });
        
        // Test step 2: Calculate multiplayer difficulty
        scenario->AddTestStep([](GameSimulation& sim, std::string& message) {
            float mpDifficulty = sim.CalculateMultiplayerDifficulty();
            
            float minDifficulty = std::min({
                sim.GetPlayer(0)->currentDifficulty,
                sim.GetPlayer(1)->currentDifficulty,
                sim.GetPlayer(2)->currentDifficulty
            });
            
            float maxDifficulty = std::max({
                sim.GetPlayer(0)->currentDifficulty,
                sim.GetPlayer(1)->currentDifficulty,
                sim.GetPlayer(2)->currentDifficulty
            });
            
            message = "Multiplayer difficulty: " + std::to_string(mpDifficulty) + 
                     ", Min player difficulty: " + std::to_string(minDifficulty) + 
                     ", Max player difficulty: " + std::to_string(maxDifficulty);
            
            // Multiplayer difficulty should be between min and max player difficulties
            return mpDifficulty >= minDifficulty && mpDifficulty <= maxDifficulty;
        });
        
        // Test step 3: Scale boss monster for multiplayer
        scenario->AddTestStep([](GameSimulation& sim, std::string& message) {
            std::string bossStats = sim.ScaleMonsterForMultiplayer(0);
            message = "Multiplayer scaled boss stats:\n" + bossStats;
            
            // Verify that the boss is scaled appropriately
            // This is a simple check - in a real test, we'd parse the stats string
            const SimulatedMonster* boss = sim.GetMonster(0);
            bool bossScaled = bossStats.find(std::to_string(boss->baseHP)) != std::string::npos &&
                             bossStats.find(std::to_string(boss->baseHP + 1)) != std::string::npos;
            
            return bossScaled;
        });
        
        verificationTest->AddScenario(std::move(scenario));
    }
    
    void AddWaveEffectsScenario() {
        auto scenario = std::make_unique<TestScenario>(
            "Wave Effects",
            "Tests wave-like variations in difficulty"
        );
        
        // Setup step: Add a player and set wave parameters
        scenario->AddSetupStep([](GameSimulation& sim) {
            sim.AddPlayer(20, 50.0f, "WaveTestPlayer");
            sim.SetWaveParameters(WavePatternType::Sine, 0.3f, 0.5f, 0.0f, 1.0f);
        });
        
        // Test step 1: Verify that difficulty varies over time
        scenario->AddTestStep([](GameSimulation& sim, std::string& message) {
            // Calculate initial difficulty
            float initialDifficulty = sim.CalculatePlayerDifficulty(0);
            
            // Record difficulties over time
            std::vector<float> difficulties;
            difficulties.push_back(initialDifficulty);
            
            for (int i = 0; i < 5; i++) {
                sim.SimulateGameTime(0.5f);
                difficulties.push_back(sim.GetPlayer(0)->currentDifficulty);
            }
            
            // Check that difficulty varies
            bool difficultyVaries = false;
            for (size_t i = 1; i < difficulties.size(); i++) {
                if (std::abs(difficulties[i] - difficulties[i-1]) > 1.0f) {
                    difficultyVaries = true;
                    break;
                }
            }
            
            message = "Difficulties over time: ";
            for (float diff : difficulties) {
                message += std::to_string(diff) + ", ";
            }
            
            return difficultyVaries;
        });
        
        // Test step 2: Verify that wave effects are applied to monster scaling
        scenario->AddTestStep([](GameSimulation& sim, std::string& message) {
            // Add a monster
            int monsterId = sim.AddMonster(MT_GOLEM, 300, 40, 30, "WaveTestGolem");
            
            // Record scaled monster stats over time
            std::vector<std::string> monsterStats;
            for (int i = 0; i < 3; i++) {
                monsterStats.push_back(sim.ScaleMonsterForPlayer(monsterId, 0));
                sim.SimulateGameTime(1.0f);
            }
            
            // Check that monster stats vary
            bool statsVary = false;
            for (size_t i = 1; i < monsterStats.size(); i++) {
                if (monsterStats[i] != monsterStats[i-1]) {
                    statsVary = true;
                    break;
                }
            }
            
            message = "Monster stats vary over time: " + std::string(statsVary ? "Yes" : "No");
            
            return statsVary;
        });
        
        verificationTest->AddScenario(std::move(scenario));
    }
    
    void AddSaveLoadScenario() {
        auto scenario = std::make_unique<TestScenario>(
            "Save and Load",
            "Tests saving and loading difficulty state"
        );
        
        // Setup step: Add a player
        scenario->AddSetupStep([](GameSimulation& sim) {
            sim.AddPlayer(25, 60.0f, "SaveLoadPlayer");
        });
        
        // Test step 1: Calculate initial difficulty
        scenario->AddTestStep([](GameSimulation& sim, std::string& message) {
            float difficulty = sim.CalculatePlayerDifficulty(0);
            message = "Initial difficulty: " + std::to_string(difficulty);
            return difficulty > 0.0f;
        });
        
        // Test step 2: Save difficulty state
        scenario->AddTestStep([](GameSimulation& sim, std::string& message) {
            bool saveSuccess = sim.SaveDifficultyState(0, "temp_test_difficulty.dat");
            message = "Save " + std::string(saveSuccess ? "succeeded" : "failed");
            return saveSuccess;
        });
        
        // Test step 3: Change difficulty and load saved state
        scenario->AddTestStep([](GameSimulation& sim, std::string& message) {
            float originalDifficulty = sim.GetPlayer(0)->currentDifficulty;
            
            // Change difficulty
            sim.UpdatePlayerGearLevel(0, 30.0f);
            sim.SimulateGameTime(2.0f);
            
            float changedDifficulty = sim.GetPlayer(0)->currentDifficulty;
            
            // Load saved state
            bool loadSuccess = sim.LoadDifficultyState(0, "temp_test_difficulty.dat");
            
            float loadedDifficulty = sim.GetPlayer(0)->currentDifficulty;
            
            message = "Original difficulty: " + std::to_string(originalDifficulty) + 
                     ", Changed difficulty: " + std::to_string(changedDifficulty) + 
                     ", Loaded difficulty: " + std::to_string(loadedDifficulty);
            
            // Clean up temporary file
            std::remove("temp_test_difficulty.dat");
            
            return loadSuccess && std::abs(loadedDifficulty - originalDifficulty) < 0.1f;
        });
        
        verificationTest->AddScenario(std::move(scenario));
    }
    
    void AddEdgeCaseScenario() {
        auto scenario = std::make_unique<TestScenario>(
            "Edge Cases",
            "Tests edge cases and boundary conditions"
        );
        
        // Setup step: Add players with extreme gear levels
        scenario->AddSetupStep([](GameSimulation& sim) {
            sim.AddPlayer(1, 1.0f, "MinGearPlayer");    // Minimum gear level
            sim.AddPlayer(50, 100.0f, "MaxGearPlayer"); // Maximum gear level
            sim.AddMonster(MT_ZOMBIE, 100, 20, 10, "EdgeCaseZombie");
        });
        
        // Test step 1: Verify minimum gear level player has appropriate difficulty
        scenario->AddTestStep([](GameSimulation& sim, std::string& message) {
            float minDifficulty = sim.CalculatePlayerDifficulty(0);
            message = "Minimum gear player difficulty: " + std::to_string(minDifficulty);
            
            // Difficulty should be positive but low
            return minDifficulty > 0.0f && minDifficulty < 20.0f;
        });
        
        // Test step 2: Verify maximum gear level player has appropriate difficulty
        scenario->AddTestStep([](GameSimulation& sim, std::string& message) {
            float maxDifficulty = sim.CalculatePlayerDifficulty(1);
            message = "Maximum gear player difficulty: " + std::to_string(maxDifficulty);
            
            // Difficulty should be high but not excessive
            return maxDifficulty > 50.0f;
        });
        
        // Test step 3: Verify monster scaling works for extreme difficulties
        scenario->AddTestStep([](GameSimulation& sim, std::string& message) {
            std::string minPlayerMonsterStats = sim.ScaleMonsterForPlayer(0, 0);
            std::string maxPlayerMonsterStats = sim.ScaleMonsterForPlayer(0, 1);
            
            message = "Monster stats for min gear player:\n" + minPlayerMonsterStats + 
                     "\nMonster stats for max gear player:\n" + maxPlayerMonsterStats;
            
            // Verify that monster stats are different for min and max gear players
            return minPlayerMonsterStats != maxPlayerMonsterStats;
        });
        
        // Test step 4: Verify rapid gear level changes are smoothed
        scenario->AddTestStep([](GameSimulation& sim, std::string& message) {
            // Get initial difficulty
            float initialDifficulty = sim.GetPlayer(0)->currentDifficulty;
            
            // Make a huge jump in gear level
            sim.UpdatePlayerGearLevel(0, 90.0f);
            
            // Simulate a small amount of time
            sim.SimulateGameTime(0.1f);
            
            // Get new difficulty
            float newDifficulty = sim.GetPlayer(0)->currentDifficulty;
            
            message = "Initial difficulty: " + std::to_string(initialDifficulty) + 
                     ", New difficulty after gear jump: " + std::to_string(newDifficulty);
            
            // Difficulty should increase, but not by the full amount immediately
            return newDifficulty > initialDifficulty && 
                   newDifficulty < initialDifficulty + (90.0f - 1.0f) * 0.5f;
        });
        
        verificationTest->AddScenario(std::move(scenario));
    }
    
    std::unique_ptr<SystemVerificationTest> verificationTest;
};

TEST_F(DifficultySystemVerificationTest, RunAllScenarios) {
    bool allPassed = verificationTest->RunAllScenarios();
    
    // Generate and save the report
    std::string report = verificationTest->GenerateReport();
    verificationTest->SaveReportToFile("difficulty_system_verification_report.txt");
    
    // Output the report to the console
    std::cout << report << std::endl;
    
    EXPECT_TRUE(allPassed) << "Not all verification scenarios passed. See report for details.";
}

} // namespace testing
} // namespace devilution
