/**
 * @file balance_utility.h
 *
 * Interface for utilities to help balance and fine-tune the dynamic difficulty system.
 */
#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "player.h"
#include "monsters.h"
#include "gear/gear_config.h"
#include "monsters/monster_scaling.h"
#include "difficulty/difficulty_manager.h"

namespace devilution {

/**
 * @brief Structure for a balance test scenario
 */
struct BalanceTestScenario {
    std::string name;
    std::string description;
    float playerLevel;
    float gearLevel;
    std::vector<monster_id> monsterTypes;
    int numRounds;
    
    // Default constructor
    BalanceTestScenario()
        : name("Default"), description("Default test scenario"), playerLevel(1.0f),
          gearLevel(1.0f), numRounds(10)
    {
    }
};

/**
 * @brief Structure for balance test results
 */
struct BalanceTestResults {
    std::string scenarioName;
    float playerLevel;
    float gearLevel;
    float difficultyLevel;
    std::vector<std::pair<monster_id, bool>> monsterResults; // monster type and whether player won
    float playerSurvivalRate;
    float averageTimeToKill;
    float averageDamageTaken;
    std::string notes;
};

/**
 * @brief Class for balancing and fine-tuning the dynamic difficulty system
 */
class BalanceUtility {
public:
    /**
     * @brief Gets the singleton instance
     * @return Reference to the singleton instance
     */
    static BalanceUtility& GetInstance();
    
    /**
     * @brief Initializes the balance utility
     */
    void Initialize();
    
    /**
     * @brief Runs a balance test scenario
     * @param scenario The scenario to test
     * @return The test results
     */
    BalanceTestResults RunTestScenario(const BalanceTestScenario &scenario);
    
    /**
     * @brief Runs a batch of balance test scenarios
     * @param scenarios The scenarios to test
     * @return The test results for each scenario
     */
    std::vector<BalanceTestResults> RunTestBatch(const std::vector<BalanceTestScenario> &scenarios);
    
    /**
     * @brief Generates a balance test report
     * @param results The test results
     * @return A text report of the test results
     */
    std::string GenerateTestReport(const std::vector<BalanceTestResults> &results);
    
    /**
     * @brief Adjusts gear scoring parameters based on test results
     * @param results The test results
     * @param config The gear scoring configuration to adjust
     * @return True if adjustments were made, false otherwise
     */
    bool AdjustGearScoringParams(const std::vector<BalanceTestResults> &results, GearScoringConfig &config);
    
    /**
     * @brief Adjusts monster scaling parameters based on test results
     * @param results The test results
     * @param monsterDefs The monster stat definitions to adjust
     * @return True if adjustments were made, false otherwise
     */
    bool AdjustMonsterScalingParams(const std::vector<BalanceTestResults> &results, std::unordered_map<monster_id, MonsterStatDefinition> &monsterDefs);
    
    /**
     * @brief Adjusts difficulty transition parameters based on test results
     * @param results The test results
     * @param params The difficulty transition parameters to adjust
     * @return True if adjustments were made, false otherwise
     */
    bool AdjustDifficultyTransitionParams(const std::vector<BalanceTestResults> &results, DifficultyTransitionParams &params);
    
    /**
     * @brief Creates a set of predefined test scenarios
     * @return A vector of predefined test scenarios
     */
    std::vector<BalanceTestScenario> CreatePredefinedScenarios();
    
    /**
     * @brief Creates a mock player for testing
     * @param level The player level
     * @param gearLevel The player gear level
     * @return A mock player
     */
    Player CreateMockPlayer(float level, float gearLevel);
    
    /**
     * @brief Creates a mock monster for testing
     * @param type The monster type
     * @param difficultyLevel The difficulty level to scale to
     * @return A mock monster
     */
    MonsterData CreateMockMonster(monster_id type, float difficultyLevel);
    
    /**
     * @brief Simulates combat between a player and a monster
     * @param player The player
     * @param monster The monster
     * @param numRounds The number of rounds to simulate
     * @return A pair containing whether the player won and the number of rounds it took
     */
    std::pair<bool, int> SimulateCombat(const Player &player, const MonsterData &monster, int numRounds);
    
private:
    BalanceUtility() = default;
    ~BalanceUtility() = default;
    
    BalanceUtility(const BalanceUtility&) = delete;
    BalanceUtility& operator=(const BalanceUtility&) = delete;
    
    /**
     * @brief Analyzes test results to identify balance issues
     * @param results The test results
     * @return A text analysis of the balance issues
     */
    std::string AnalyzeBalanceIssues(const std::vector<BalanceTestResults> &results);
    
    /**
     * @brief Generates recommended parameter adjustments
     * @param results The test results
     * @return A text description of recommended adjustments
     */
    std::string GenerateRecommendedAdjustments(const std::vector<BalanceTestResults> &results);
    
    // Flag to track initialization
    bool initialized = false;
};

} // namespace devilution
