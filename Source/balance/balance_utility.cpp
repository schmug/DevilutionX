/**
 * @file balance_utility.cpp
 *
 * Implementation of utilities to help balance and fine-tune the dynamic difficulty system.
 */
#include "balance/balance_utility.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <random>

#include "utils/log.hpp"

namespace devilution {

BalanceUtility& BalanceUtility::GetInstance()
{
    static BalanceUtility instance;
    return instance;
}

void BalanceUtility::Initialize()
{
    if (initialized) {
        return;
    }
    
    // Initialize required subsystems
    GearLevelManager::GetInstance().Initialize();
    
    initialized = true;
    LogVerbose("Balance Utility initialized");
}

BalanceTestResults BalanceUtility::RunTestScenario(const BalanceTestScenario &scenario)
{
    BalanceTestResults results;
    results.scenarioName = scenario.name;
    results.playerLevel = scenario.playerLevel;
    results.gearLevel = scenario.gearLevel;
    
    // Create a mock player
    Player player = CreateMockPlayer(scenario.playerLevel, scenario.gearLevel);
    
    // Calculate difficulty level
    DifficultyManager &diffManager = DifficultyManager::GetInstance();
    results.difficultyLevel = diffManager.CalculateDifficulty(player);
    
    // Track combat statistics
    int totalRounds = 0;
    int playerWins = 0;
    float totalDamageTaken = 0.0f;
    
    // Run combat simulations for each monster type
    for (const auto &monsterType : scenario.monsterTypes) {
        // Create a mock monster scaled to the current difficulty
        MonsterData monster = CreateMockMonster(monsterType, results.difficultyLevel);
        
        // Simulate combat
        auto [playerWon, rounds] = SimulateCombat(player, monster, scenario.numRounds);
        
        // Record results
        results.monsterResults.push_back(std::make_pair(monsterType, playerWon));
        
        if (playerWon) {
            playerWins++;
        }
        
        totalRounds += rounds;
        totalDamageTaken += (playerWon ? 50.0f : 100.0f); // Mock damage taken
    }
    
    // Calculate statistics
    results.playerSurvivalRate = static_cast<float>(playerWins) / scenario.monsterTypes.size();
    results.averageTimeToKill = static_cast<float>(totalRounds) / scenario.monsterTypes.size();
    results.averageDamageTaken = totalDamageTaken / scenario.monsterTypes.size();
    
    // Add notes based on results
    std::stringstream notes;
    if (results.playerSurvivalRate < 0.3f) {
        notes << "DIFFICULTY TOO HIGH: Player survival rate is very low. ";
    } else if (results.playerSurvivalRate > 0.9f) {
        notes << "DIFFICULTY TOO LOW: Player survival rate is very high. ";
    } else {
        notes << "BALANCED: Player survival rate is reasonable. ";
    }
    
    if (results.averageTimeToKill > 20.0f) {
        notes << "Combat takes too long to resolve. ";
    } else if (results.averageTimeToKill < 3.0f) {
        notes << "Combat resolves too quickly. ";
    } else {
        notes << "Combat duration is appropriate. ";
    }
    
    results.notes = notes.str();
    
    return results;
}

std::vector<BalanceTestResults> BalanceUtility::RunTestBatch(const std::vector<BalanceTestScenario> &scenarios)
{
    std::vector<BalanceTestResults> allResults;
    
    for (const auto &scenario : scenarios) {
        BalanceTestResults results = RunTestScenario(scenario);
        allResults.push_back(results);
    }
    
    return allResults;
}

std::string BalanceUtility::GenerateTestReport(const std::vector<BalanceTestResults> &results)
{
    std::stringstream report;
    
    // Report header
    report << "===== DYNAMIC DIFFICULTY BALANCE TEST REPORT =====\n\n";
    report << "Test Date: " << "2023-08-01" << "\n"; // TODO: Use actual date
    report << "Number of Scenarios: " << results.size() << "\n\n";
    
    // Summary statistics
    float avgSurvivalRate = 0.0f;
    float avgTimeToKill = 0.0f;
    float avgDamageTaken = 0.0f;
    
    for (const auto &result : results) {
        avgSurvivalRate += result.playerSurvivalRate;
        avgTimeToKill += result.averageTimeToKill;
        avgDamageTaken += result.averageDamageTaken;
    }
    
    avgSurvivalRate /= results.size();
    avgTimeToKill /= results.size();
    avgDamageTaken /= results.size();
    
    report << "Summary Statistics:\n";
    report << "  Average Player Survival Rate: " << std::fixed << std::setprecision(2) << (avgSurvivalRate * 100.0f) << "%\n";
    report << "  Average Time to Kill: " << std::fixed << std::setprecision(2) << avgTimeToKill << " rounds\n";
    report << "  Average Damage Taken: " << std::fixed << std::setprecision(2) << avgDamageTaken << " points\n\n";
    
    // Detailed results
    report << "Detailed Results by Scenario:\n";
    report << "------------------------------------\n";
    
    for (const auto &result : results) {
        report << "Scenario: " << result.scenarioName << "\n";
        report << "  Player Level: " << result.playerLevel << "\n";
        report << "  Gear Level: " << result.gearLevel << "\n";
        report << "  Calculated Difficulty: " << result.difficultyLevel << "\n";
        report << "  Survival Rate: " << std::fixed << std::setprecision(2) << (result.playerSurvivalRate * 100.0f) << "%\n";
        report << "  Avg Time to Kill: " << std::fixed << std::setprecision(2) << result.averageTimeToKill << " rounds\n";
        report << "  Avg Damage Taken: " << std::fixed << std::setprecision(2) << result.averageDamageTaken << " points\n";
        report << "  Notes: " << result.notes << "\n";
        report << "------------------------------------\n";
    }
    
    // Balance analysis
    report << "\nBalance Analysis:\n";
    report << AnalyzeBalanceIssues(results);
    
    // Recommendations
    report << "\nRecommended Adjustments:\n";
    report << GenerateRecommendedAdjustments(results);
    
    return report.str();
}

bool BalanceUtility::AdjustGearScoringParams(const std::vector<BalanceTestResults> &results, GearScoringConfig &config)
{
    bool adjustmentsMade = false;
    
    // Calculate average survival rate
    float avgSurvivalRate = 0.0f;
    for (const auto &result : results) {
        avgSurvivalRate += result.playerSurvivalRate;
    }
    avgSurvivalRate /= results.size();
    
    // Adjust base quality multipliers if needed
    if (avgSurvivalRate < 0.4f) {
        // Increase base quality multipliers to make gear more powerful
        for (auto &[quality, multiplier] : config.qualityMultipliers) {
            multiplier *= 1.1f; // Increase by 10%
        }
        adjustmentsMade = true;
    } else if (avgSurvivalRate > 0.8f) {
        // Decrease base quality multipliers to make gear less powerful
        for (auto &[quality, multiplier] : config.qualityMultipliers) {
            multiplier *= 0.9f; // Decrease by 10%
        }
        adjustmentsMade = true;
    }
    
    // Adjust affix weights if needed
    // This is a simplified example - real implementation would be more nuanced
    if (avgSurvivalRate < 0.4f) {
        // Increase offensive affix weights
        config.affixWeights["damage"] *= 1.15f;
        config.affixWeights["attack"] *= 1.15f;
        adjustmentsMade = true;
    } else if (avgSurvivalRate > 0.8f) {
        // Decrease offensive affix weights
        config.affixWeights["damage"] *= 0.85f;
        config.affixWeights["attack"] *= 0.85f;
        adjustmentsMade = true;
    }
    
    return adjustmentsMade;
}

bool BalanceUtility::AdjustMonsterScalingParams(const std::vector<BalanceTestResults> &results, std::unordered_map<monster_id, MonsterStatDefinition> &monsterDefs)
{
    bool adjustmentsMade = false;
    
    // Calculate average survival rate and time to kill
    float avgSurvivalRate = 0.0f;
    float avgTimeToKill = 0.0f;
    
    for (const auto &result : results) {
        avgSurvivalRate += result.playerSurvivalRate;
        avgTimeToKill += result.averageTimeToKill;
    }
    
    avgSurvivalRate /= results.size();
    avgTimeToKill /= results.size();
    
    // Adjust monster health scaling if needed
    if (avgTimeToKill < 5.0f) {
        // Increase monster health scaling to make fights last longer
        for (auto &[id, def] : monsterDefs) {
            def.healthScalingFactor *= 1.2f; // Increase by 20%
        }
        adjustmentsMade = true;
    } else if (avgTimeToKill > 15.0f) {
        // Decrease monster health scaling to make fights shorter
        for (auto &[id, def] : monsterDefs) {
            def.healthScalingFactor *= 0.8f; // Decrease by 20%
        }
        adjustmentsMade = true;
    }
    
    // Adjust monster damage scaling if needed
    if (avgSurvivalRate > 0.8f) {
        // Increase monster damage scaling to make fights more dangerous
        for (auto &[id, def] : monsterDefs) {
            def.damageScalingFactor *= 1.15f; // Increase by 15%
        }
        adjustmentsMade = true;
    } else if (avgSurvivalRate < 0.3f) {
        // Decrease monster damage scaling to make fights less dangerous
        for (auto &[id, def] : monsterDefs) {
            def.damageScalingFactor *= 0.85f; // Decrease by 15%
        }
        adjustmentsMade = true;
    }
    
    return adjustmentsMade;
}

bool BalanceUtility::AdjustDifficultyTransitionParams(const std::vector<BalanceTestResults> &results, DifficultyTransitionParams &params)
{
    bool adjustmentsMade = false;
    
    // Calculate average difficulty level
    float avgDifficultyLevel = 0.0f;
    for (const auto &result : results) {
        avgDifficultyLevel += result.difficultyLevel;
    }
    avgDifficultyLevel /= results.size();
    
    // Adjust smoothing factor if needed
    if (avgDifficultyLevel > 2.0f) {
        // Increase smoothing factor to make transitions more gradual
        params.smoothingFactor *= 1.2f; // Increase by 20%
        adjustmentsMade = true;
    } else if (avgDifficultyLevel < 0.5f) {
        // Decrease smoothing factor to make transitions more responsive
        params.smoothingFactor *= 0.8f; // Decrease by 20%
        adjustmentsMade = true;
    }
    
    // Adjust transition speed if needed
    if (avgDifficultyLevel > 2.0f) {
        // Decrease transition speed to make difficulty increases more gradual
        params.transitionSpeed *= 0.8f; // Decrease by 20%
        adjustmentsMade = true;
    } else if (avgDifficultyLevel < 0.5f) {
        // Increase transition speed to make difficulty increases more responsive
        params.transitionSpeed *= 1.2f; // Increase by 20%
        adjustmentsMade = true;
    }
    
    return adjustmentsMade;
}

std::vector<BalanceTestScenario> BalanceUtility::CreatePredefinedScenarios()
{
    std::vector<BalanceTestScenario> scenarios;
    
    // Early game scenario
    BalanceTestScenario earlyGame;
    earlyGame.name = "Early Game";
    earlyGame.description = "Player just starting out with basic gear";
    earlyGame.playerLevel = 1.0f;
    earlyGame.gearLevel = 5.0f;
    earlyGame.monsterTypes = {MT_ZOMBIE, MT_FALLEN, MT_SKELETON};
    earlyGame.numRounds = 10;
    scenarios.push_back(earlyGame);
    
    // Mid game scenario
    BalanceTestScenario midGame;
    midGame.name = "Mid Game";
    midGame.description = "Player with moderate gear progression";
    midGame.playerLevel = 15.0f;
    midGame.gearLevel = 30.0f;
    midGame.monsterTypes = {MT_GOLEM, MT_SUCCUBUS, MT_BALROG};
    midGame.numRounds = 15;
    scenarios.push_back(midGame);
    
    // Late game scenario
    BalanceTestScenario lateGame;
    lateGame.name = "Late Game";
    lateGame.description = "Player with high-end gear";
    lateGame.playerLevel = 30.0f;
    lateGame.gearLevel = 75.0f;
    lateGame.monsterTypes = {MT_DIABLO, MT_BLACKKNIGHT, MT_COUNSELOR};
    lateGame.numRounds = 20;
    scenarios.push_back(lateGame);
    
    // Gear spike scenario
    BalanceTestScenario gearSpike;
    gearSpike.name = "Gear Spike";
    gearSpike.description = "Player with gear much better than expected for their level";
    gearSpike.playerLevel = 10.0f;
    gearSpike.gearLevel = 50.0f;
    gearSpike.monsterTypes = {MT_GOLEM, MT_SUCCUBUS, MT_BALROG};
    gearSpike.numRounds = 15;
    scenarios.push_back(gearSpike);
    
    // Gear deficit scenario
    BalanceTestScenario gearDeficit;
    gearDeficit.name = "Gear Deficit";
    gearDeficit.description = "Player with gear much worse than expected for their level";
    gearDeficit.playerLevel = 25.0f;
    gearDeficit.gearLevel = 15.0f;
    gearDeficit.monsterTypes = {MT_GOLEM, MT_SUCCUBUS, MT_BALROG};
    gearDeficit.numRounds = 15;
    scenarios.push_back(gearDeficit);
    
    return scenarios;
}

Player BalanceUtility::CreateMockPlayer(float level, float gearLevel)
{
    // Create a mock player with the specified level and gear level
    // This is a simplified implementation - real implementation would be more complex
    Player player;
    
    // Set player level
    player._pLevel = static_cast<int>(level);
    
    // Set player stats based on level
    player._pStrength = 10 + static_cast<int>(level * 1.5f);
    player._pDexterity = 10 + static_cast<int>(level * 1.2f);
    player._pVitality = 10 + static_cast<int>(level * 1.3f);
    player._pMagic = 10 + static_cast<int>(level * 1.1f);
    
    // Set player health and mana
    player._pMaxHP = 100 + static_cast<int>(level * 10.0f);
    player._pHitPoints = player._pMaxHP;
    player._pMaxMana = 50 + static_cast<int>(level * 5.0f);
    player._pMana = player._pMaxMana;
    
    // Set player damage
    player._pDamageMod = static_cast<int>(level * 2.0f);
    
    // TODO: Set up mock equipment to match the desired gear level
    
    return player;
}

MonsterData BalanceUtility::CreateMockMonster(monster_id type, float difficultyLevel)
{
    // Create a mock monster scaled to the specified difficulty level
    // This is a simplified implementation - real implementation would be more complex
    MonsterData monster;
    
    // Set monster type
    monster._mtype = type;
    
    // Set base stats based on monster type
    switch (type) {
    case MT_ZOMBIE:
        monster._mMaxHP = 50;
        monster._mHitPoints = 50;
        monster._mMinDamage = 5;
        monster._mMaxDamage = 10;
        monster._mAC = 10;
        break;
    case MT_FALLEN:
        monster._mMaxHP = 30;
        monster._mHitPoints = 30;
        monster._mMinDamage = 3;
        monster._mMaxDamage = 7;
        monster._mAC = 5;
        break;
    case MT_SKELETON:
        monster._mMaxHP = 40;
        monster._mHitPoints = 40;
        monster._mMinDamage = 4;
        monster._mMaxDamage = 8;
        monster._mAC = 15;
        break;
    case MT_GOLEM:
        monster._mMaxHP = 100;
        monster._mHitPoints = 100;
        monster._mMinDamage = 10;
        monster._mMaxDamage = 15;
        monster._mAC = 30;
        break;
    case MT_SUCCUBUS:
        monster._mMaxHP = 80;
        monster._mHitPoints = 80;
        monster._mMinDamage = 8;
        monster._mMaxDamage = 16;
        monster._mAC = 20;
        break;
    case MT_BALROG:
        monster._mMaxHP = 150;
        monster._mHitPoints = 150;
        monster._mMinDamage = 15;
        monster._mMaxDamage = 25;
        monster._mAC = 40;
        break;
    case MT_DIABLO:
        monster._mMaxHP = 500;
        monster._mHitPoints = 500;
        monster._mMinDamage = 30;
        monster._mMaxDamage = 50;
        monster._mAC = 80;
        break;
    case MT_BLACKKNIGHT:
        monster._mMaxHP = 300;
        monster._mHitPoints = 300;
        monster._mMinDamage = 20;
        monster._mMaxDamage = 35;
        monster._mAC = 60;
        break;
    case MT_COUNSELOR:
        monster._mMaxHP = 200;
        monster._mHitPoints = 200;
        monster._mMinDamage = 15;
        monster._mMaxDamage = 30;
        monster._mAC = 50;
        break;
    default:
        monster._mMaxHP = 100;
        monster._mHitPoints = 100;
        monster._mMinDamage = 10;
        monster._mMaxDamage = 20;
        monster._mAC = 30;
        break;
    }
    
    // Scale monster stats based on difficulty level
    float healthScale = 1.0f + (difficultyLevel * 0.5f);
    float damageScale = 1.0f + (difficultyLevel * 0.3f);
    float acScale = 1.0f + (difficultyLevel * 0.2f);
    
    monster._mMaxHP = static_cast<int>(monster._mMaxHP * healthScale);
    monster._mHitPoints = monster._mMaxHP;
    monster._mMinDamage = static_cast<int>(monster._mMinDamage * damageScale);
    monster._mMaxDamage = static_cast<int>(monster._mMaxDamage * damageScale);
    monster._mAC = static_cast<int>(monster._mAC * acScale);
    
    return monster;
}

std::pair<bool, int> BalanceUtility::SimulateCombat(const Player &player, const MonsterData &monster, int numRounds)
{
    // Simple combat simulation
    // This is a very simplified implementation - real implementation would be more complex
    std::random_device rd;
    std::mt19937 gen(rd());
    
    int playerHP = player._pHitPoints;
    int monsterHP = monster._mHitPoints;
    
    int round = 0;
    bool playerWon = false;
    
    while (round < numRounds && playerHP > 0 && monsterHP > 0) {
        // Player attacks monster
        std::uniform_int_distribution<> playerDamageDist(1, 10 + player._pDamageMod);
        int playerDamage = playerDamageDist(gen);
        
        // Apply monster AC
        playerDamage = std::max(1, playerDamage - monster._mAC / 10);
        
        monsterHP -= playerDamage;
        
        // Check if monster is defeated
        if (monsterHP <= 0) {
            playerWon = true;
            break;
        }
        
        // Monster attacks player
        std::uniform_int_distribution<> monsterDamageDist(monster._mMinDamage, monster._mMaxDamage);
        int monsterDamage = monsterDamageDist(gen);
        
        playerHP -= monsterDamage;
        
        // Increment round counter
        round++;
    }
    
    return std::make_pair(playerWon, round);
}

std::string BalanceUtility::AnalyzeBalanceIssues(const std::vector<BalanceTestResults> &results)
{
    std::stringstream analysis;
    
    // Count scenarios with different balance issues
    int tooEasy = 0;
    int tooHard = 0;
    int tooSlow = 0;
    int tooFast = 0;
    
    for (const auto &result : results) {
        if (result.playerSurvivalRate > 0.8f) {
            tooEasy++;
        } else if (result.playerSurvivalRate < 0.3f) {
            tooHard++;
        }
        
        if (result.averageTimeToKill > 15.0f) {
            tooSlow++;
        } else if (result.averageTimeToKill < 5.0f) {
            tooFast++;
        }
    }
    
    // Generate analysis based on counts
    if (tooEasy > results.size() / 3) {
        analysis << "- ISSUE: Game is too easy in " << tooEasy << " out of " << results.size() << " scenarios.\n";
        analysis << "  Player survival rates are too high, suggesting monsters are not challenging enough.\n";
    }
    
    if (tooHard > results.size() / 3) {
        analysis << "- ISSUE: Game is too difficult in " << tooHard << " out of " << results.size() << " scenarios.\n";
        analysis << "  Player survival rates are too low, suggesting monsters are too challenging.\n";
    }
    
    if (tooSlow > results.size() / 3) {
        analysis << "- ISSUE: Combat takes too long in " << tooSlow << " out of " << results.size() << " scenarios.\n";
        analysis << "  Average time to kill is too high, suggesting monster health may be too high or player damage too low.\n";
    }
    
    if (tooFast > results.size() / 3) {
        analysis << "- ISSUE: Combat resolves too quickly in " << tooFast << " out of " << results.size() << " scenarios.\n";
        analysis << "  Average time to kill is too low, suggesting monster health may be too low or player damage too high.\n";
    }
    
    // Check for gear level scaling issues
    bool gearScalingIssue = false;
    
    for (size_t i = 0; i < results.size(); i++) {
        for (size_t j = i + 1; j < results.size(); j++) {
            float gearRatio = results[i].gearLevel / results[j].gearLevel;
            float survivalRatio = results[i].playerSurvivalRate / results[j].playerSurvivalRate;
            
            // If gear level difference doesn't correspond to survival rate difference
            if (gearRatio > 2.0f && survivalRatio < 1.2f) {
                gearScalingIssue = true;
                analysis << "- ISSUE: Gear scaling may be insufficient. Large gear level differences don't translate to proportional survival rate differences.\n";
                break;
            }
        }
        
        if (gearScalingIssue) {
            break;
        }
    }
    
    // If no issues found
    if (analysis.str().empty()) {
        analysis << "- No significant balance issues detected. The current parameters appear to be well-balanced.\n";
    }
    
    return analysis.str();
}

std::string BalanceUtility::GenerateRecommendedAdjustments(const std::vector<BalanceTestResults> &results)
{
    std::stringstream recommendations;
    
    // Calculate average statistics
    float avgSurvivalRate = 0.0f;
    float avgTimeToKill = 0.0f;
    float avgDifficultyLevel = 0.0f;
    
    for (const auto &result : results) {
        avgSurvivalRate += result.playerSurvivalRate;
        avgTimeToKill += result.averageTimeToKill;
        avgDifficultyLevel += result.difficultyLevel;
    }
    
    avgSurvivalRate /= results.size();
    avgTimeToKill /= results.size();
    avgDifficultyLevel /= results.size();
    
    // Generate recommendations based on statistics
    if (avgSurvivalRate > 0.8f) {
        recommendations << "1. Increase monster damage scaling by 10-15%\n";
        recommendations << "2. Decrease gear quality multipliers by 5-10%\n";
        recommendations << "3. Increase difficulty transition speed by 20%\n";
    } else if (avgSurvivalRate < 0.3f) {
        recommendations << "1. Decrease monster damage scaling by 10-15%\n";
        recommendations << "2. Increase gear quality multipliers by 5-10%\n";
        recommendations << "3. Decrease difficulty transition speed by 20%\n";
    }
    
    if (avgTimeToKill > 15.0f) {
        recommendations << "4. Decrease monster health scaling by 15-20%\n";
        recommendations << "5. Increase offensive affix weights by 10%\n";
    } else if (avgTimeToKill < 5.0f) {
        recommendations << "4. Increase monster health scaling by 15-20%\n";
        recommendations << "5. Decrease offensive affix weights by 10%\n";
    }
    
    if (avgDifficultyLevel > 2.0f) {
        recommendations << "6. Increase smoothing factor by 20% to make difficulty transitions more gradual\n";
        recommendations << "7. Adjust difficulty calculation formula to reduce the impact of gear level\n";
    } else if (avgDifficultyLevel < 0.5f) {
        recommendations << "6. Decrease smoothing factor by 20% to make difficulty transitions more responsive\n";
        recommendations << "7. Adjust difficulty calculation formula to increase the impact of gear level\n";
    }
    
    // Check for specific scenario issues
    for (const auto &result : results) {
        if (result.name == "Gear Spike" && result.playerSurvivalRate > 0.9f) {
            recommendations << "8. Improve difficulty scaling for players with unexpectedly high gear levels\n";
            recommendations << "9. Consider implementing a faster response to sudden gear level increases\n";
        }
        
        if (result.name == "Gear Deficit" && result.playerSurvivalRate < 0.1f) {
            recommendations << "8. Improve difficulty scaling for players with unexpectedly low gear levels\n";
            recommendations << "9. Consider implementing a floor on difficulty to prevent extremely challenging scenarios\n";
        }
    }
    
    // If no recommendations
    if (recommendations.str().empty()) {
        recommendations << "Current parameters appear to be well-balanced. No specific adjustments recommended at this time.\n";
    }
    
    return recommendations.str();
}

} // namespace devilution
