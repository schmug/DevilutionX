#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <thread>
#include <fstream>

#include "player.h"
#include "monsters.h"
#include "items.h"
#include "gear/gear_score.h"
#include "difficulty/difficulty_manager.h"
#include "difficulty/difficulty_wave.h"
#include "monsters/monster_scaling.h"
#include "multiplayer/mp_difficulty.h"
#include "utils/log.hpp"

namespace devilution {
namespace testing {

// Forward declarations
class GameSimulation;
class TestScenario;
class TestResult;

/**
 * @brief Structure to hold player data for simulation
 */
struct SimulatedPlayer {
    int id;
    int level;
    float gearLevel;
    std::string name;
    std::vector<Item> equipment;
    float currentDifficulty;
    
    SimulatedPlayer(int id, int level, float gearLevel, const std::string& name)
        : id(id), level(level), gearLevel(gearLevel), name(name), currentDifficulty(0.0f)
    {
    }
};

/**
 * @brief Structure to hold monster data for simulation
 */
struct SimulatedMonster {
    monster_id type;
    int baseHP;
    int baseDamage;
    int baseAC;
    std::string name;
    
    SimulatedMonster(monster_id type, int baseHP, int baseDamage, int baseAC, const std::string& name)
        : type(type), baseHP(baseHP), baseDamage(baseDamage), baseAC(baseAC), name(name)
    {
    }
};

/**
 * @brief Structure to hold game state for simulation
 */
struct GameState {
    float gameTime;
    std::vector<SimulatedPlayer> players;
    std::vector<SimulatedMonster> monsters;
    bool isMultiplayer;
    float waveDifficulty;
    
    GameState()
        : gameTime(0.0f), isMultiplayer(false), waveDifficulty(1.0f)
    {
    }
};

/**
 * @brief Class for simulating game sessions
 */
class GameSimulation {
public:
    GameSimulation();
    ~GameSimulation();
    
    /**
     * @brief Initializes the simulation
     */
    void Initialize();
    
    /**
     * @brief Adds a player to the simulation
     * @param level Player level
     * @param gearLevel Player gear level
     * @param name Player name
     * @return Player ID
     */
    int AddPlayer(int level, float gearLevel, const std::string& name);
    
    /**
     * @brief Adds a monster to the simulation
     * @param type Monster type
     * @param baseHP Base hit points
     * @param baseDamage Base damage
     * @param baseAC Base armor class
     * @param name Monster name
     * @return Monster ID
     */
    int AddMonster(monster_id type, int baseHP, int baseDamage, int baseAC, const std::string& name);
    
    /**
     * @brief Sets whether the simulation is multiplayer
     * @param isMultiplayer Whether the simulation is multiplayer
     */
    void SetMultiplayer(bool isMultiplayer);
    
    /**
     * @brief Sets wave parameters for the simulation
     * @param patternType Wave pattern type
     * @param amplitude Wave amplitude
     * @param frequency Wave frequency
     * @param phase Wave phase
     * @param baseline Wave baseline
     */
    void SetWaveParameters(WavePatternType patternType, float amplitude, float frequency, float phase, float baseline);
    
    /**
     * @brief Updates player gear level
     * @param playerId Player ID
     * @param newGearLevel New gear level
     */
    void UpdatePlayerGearLevel(int playerId, float newGearLevel);
    
    /**
     * @brief Simulates game time passing
     * @param seconds Number of seconds to simulate
     */
    void SimulateGameTime(float seconds);
    
    /**
     * @brief Gets the current game state
     * @return Current game state
     */
    const GameState& GetGameState() const;
    
    /**
     * @brief Gets a player by ID
     * @param playerId Player ID
     * @return Pointer to the player, or nullptr if not found
     */
    const SimulatedPlayer* GetPlayer(int playerId) const;
    
    /**
     * @brief Gets a monster by ID
     * @param monsterId Monster ID
     * @return Pointer to the monster, or nullptr if not found
     */
    const SimulatedMonster* GetMonster(int monsterId) const;
    
    /**
     * @brief Calculates difficulty for a player
     * @param playerId Player ID
     * @return Calculated difficulty
     */
    float CalculatePlayerDifficulty(int playerId);
    
    /**
     * @brief Calculates multiplayer difficulty
     * @return Calculated multiplayer difficulty
     */
    float CalculateMultiplayerDifficulty();
    
    /**
     * @brief Scales a monster for a player
     * @param monsterId Monster ID
     * @param playerId Player ID
     * @return Scaled monster stats as a string
     */
    std::string ScaleMonsterForPlayer(int monsterId, int playerId);
    
    /**
     * @brief Scales a monster for multiplayer
     * @param monsterId Monster ID
     * @return Scaled monster stats as a string
     */
    std::string ScaleMonsterForMultiplayer(int monsterId);
    
    /**
     * @brief Saves difficulty state to a file
     * @param playerId Player ID
     * @param filePath File path to save to
     * @return Whether the save was successful
     */
    bool SaveDifficultyState(int playerId, const std::string& filePath);
    
    /**
     * @brief Loads difficulty state from a file
     * @param playerId Player ID
     * @param filePath File path to load from
     * @return Whether the load was successful
     */
    bool LoadDifficultyState(int playerId, const std::string& filePath);
    
    /**
     * @brief Generates a report of the current simulation state
     * @return Report as a string
     */
    std::string GenerateReport();
    
private:
    GameState gameState;
    GearLevelManager* gearManager;
    DifficultyManager* difficultyManager;
    DifficultyWave* difficultyWave;
    MonsterScaling* monsterScaling;
    MpDifficultyManager* mpDifficultyManager;
    bool initialized;
    
    /**
     * @brief Updates difficulties for all players
     */
    void UpdateDifficulties();
};

/**
 * @brief Class for defining a test scenario
 */
class TestScenario {
public:
    TestScenario(const std::string& name, const std::string& description);
    ~TestScenario();
    
    /**
     * @brief Adds a setup step to the scenario
     * @param step Function to call for setup
     */
    void AddSetupStep(std::function<void(GameSimulation&)> step);
    
    /**
     * @brief Adds a test step to the scenario
     * @param step Function to call for the test
     */
    void AddTestStep(std::function<bool(GameSimulation&, std::string&)> step);
    
    /**
     * @brief Runs the scenario
     * @param simulation Game simulation to use
     * @return Test result
     */
    TestResult Run(GameSimulation& simulation);
    
    /**
     * @brief Gets the scenario name
     * @return Scenario name
     */
    const std::string& GetName() const;
    
    /**
     * @brief Gets the scenario description
     * @return Scenario description
     */
    const std::string& GetDescription() const;
    
private:
    std::string name;
    std::string description;
    std::vector<std::function<void(GameSimulation&)>> setupSteps;
    std::vector<std::function<bool(GameSimulation&, std::string&)>> testSteps;
};

/**
 * @brief Class for holding test results
 */
class TestResult {
public:
    TestResult(const std::string& scenarioName);
    ~TestResult();
    
    /**
     * @brief Records a test step result
     * @param stepIndex Step index
     * @param success Whether the step succeeded
     * @param message Result message
     */
    void RecordStepResult(int stepIndex, bool success, const std::string& message);
    
    /**
     * @brief Checks if all steps succeeded
     * @return Whether all steps succeeded
     */
    bool AllStepsSucceeded() const;
    
    /**
     * @brief Gets the scenario name
     * @return Scenario name
     */
    const std::string& GetScenarioName() const;
    
    /**
     * @brief Gets the result summary
     * @return Result summary
     */
    std::string GetSummary() const;
    
private:
    std::string scenarioName;
    std::vector<bool> stepResults;
    std::vector<std::string> stepMessages;
};

/**
 * @brief Class for running system verification tests
 */
class SystemVerificationTest {
public:
    SystemVerificationTest();
    ~SystemVerificationTest();
    
    /**
     * @brief Adds a test scenario
     * @param scenario Test scenario to add
     */
    void AddScenario(std::unique_ptr<TestScenario> scenario);
    
    /**
     * @brief Runs all test scenarios
     * @return Whether all scenarios succeeded
     */
    bool RunAllScenarios();
    
    /**
     * @brief Generates a report of all test results
     * @return Report as a string
     */
    std::string GenerateReport();
    
    /**
     * @brief Saves the report to a file
     * @param filePath File path to save to
     * @return Whether the save was successful
     */
    bool SaveReportToFile(const std::string& filePath);
    
private:
    std::vector<std::unique_ptr<TestScenario>> scenarios;
    std::vector<TestResult> results;
    GameSimulation simulation;
};

} // namespace testing
} // namespace devilution
