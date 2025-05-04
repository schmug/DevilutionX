#include "tests/system_verification_test.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace devilution {
namespace testing {

// GameSimulation implementation
GameSimulation::GameSimulation()
    : initialized(false)
{
}

GameSimulation::~GameSimulation()
{
}

void GameSimulation::Initialize()
{
    if (initialized) {
        return;
    }
    
    // Initialize logging
    LogInit();
    
    // Initialize subsystems
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
    
    // Initialize game state
    gameState = GameState();
    
    initialized = true;
    LogVerbose("GameSimulation initialized");
}

int GameSimulation::AddPlayer(int level, float gearLevel, const std::string& name)
{
    int id = static_cast<int>(gameState.players.size());
    gameState.players.emplace_back(id, level, gearLevel, name);
    return id;
}

int GameSimulation::AddMonster(monster_id type, int baseHP, int baseDamage, int baseAC, const std::string& name)
{
    int id = static_cast<int>(gameState.monsters.size());
    gameState.monsters.emplace_back(type, baseHP, baseDamage, baseAC, name);
    return id;
}

void GameSimulation::SetMultiplayer(bool isMultiplayer)
{
    gameState.isMultiplayer = isMultiplayer;
}

void GameSimulation::SetWaveParameters(WavePatternType patternType, float amplitude, float frequency, float phase, float baseline)
{
    WaveParameters params;
    params.patternType = patternType;
    params.amplitude = amplitude;
    params.frequency = frequency;
    params.phase = phase;
    params.baseline = baseline;
    difficultyWave->SetWaveParameters(params);
}

void GameSimulation::UpdatePlayerGearLevel(int playerId, float newGearLevel)
{
    if (playerId >= 0 && playerId < static_cast<int>(gameState.players.size())) {
        gameState.players[playerId].gearLevel = newGearLevel;
    }
}

void GameSimulation::SimulateGameTime(float seconds)
{
    // Update game time
    float startTime = gameState.gameTime;
    float endTime = startTime + seconds;
    
    // Simulate in small time steps
    float timeStep = 0.1f; // 100ms time step
    int steps = static_cast<int>(seconds / timeStep);
    
    for (int i = 0; i < steps; i++) {
        float currentTime = startTime + (i * timeStep);
        
        // Update wave difficulty
        difficultyWave->Update(currentTime);
        gameState.waveDifficulty = difficultyWave->GetCurrentWaveValue();
        
        // Update difficulties for all players
        UpdateDifficulties();
        
        // Small sleep to simulate real time passing
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    // Final update at end time
    gameState.gameTime = endTime;
    difficultyWave->Update(endTime);
    gameState.waveDifficulty = difficultyWave->GetCurrentWaveValue();
    UpdateDifficulties();
}

const GameState& GameSimulation::GetGameState() const
{
    return gameState;
}

const SimulatedPlayer* GameSimulation::GetPlayer(int playerId) const
{
    if (playerId >= 0 && playerId < static_cast<int>(gameState.players.size())) {
        return &gameState.players[playerId];
    }
    return nullptr;
}

const SimulatedMonster* GameSimulation::GetMonster(int monsterId) const
{
    if (monsterId >= 0 && monsterId < static_cast<int>(gameState.monsters.size())) {
        return &gameState.monsters[monsterId];
    }
    return nullptr;
}

float GameSimulation::CalculatePlayerDifficulty(int playerId)
{
    if (playerId >= 0 && playerId < static_cast<int>(gameState.players.size())) {
        // Create a mock player for difficulty calculation
        Player mockPlayer;
        mockPlayer._pLevel = gameState.players[playerId].level;
        
        // Calculate base difficulty
        float baseDifficulty = gameState.players[playerId].gearLevel;
        
        // Apply wave effect
        float waveDifficulty = difficultyWave->ApplyWaveEffect(baseDifficulty, gameState.gameTime);
        
        // Store and return the difficulty
        gameState.players[playerId].currentDifficulty = waveDifficulty;
        return waveDifficulty;
    }
    return 0.0f;
}

float GameSimulation::CalculateMultiplayerDifficulty()
{
    if (gameState.players.empty()) {
        return 0.0f;
    }
    
    // Create mock players for difficulty calculation
    std::vector<Player> mockPlayers;
    for (const auto& player : gameState.players) {
        Player mockPlayer;
        mockPlayer._pLevel = player.level;
        mockPlayers.push_back(mockPlayer);
    }
    
    // Calculate multiplayer difficulty
    std::vector<Player*> playerPtrs;
    for (auto& player : mockPlayers) {
        playerPtrs.push_back(&player);
    }
    
    float mpDifficulty = mpDifficultyManager->CalculateWeightedDifficulty(playerPtrs);
    
    // Apply wave effect
    float waveMpDifficulty = difficultyWave->ApplyWaveEffect(mpDifficulty, gameState.gameTime);
    
    return waveMpDifficulty;
}

std::string GameSimulation::ScaleMonsterForPlayer(int monsterId, int playerId)
{
    if (monsterId < 0 || monsterId >= static_cast<int>(gameState.monsters.size()) ||
        playerId < 0 || playerId >= static_cast<int>(gameState.players.size())) {
        return "Invalid monster or player ID";
    }
    
    // Get monster and player
    const SimulatedMonster& monster = gameState.monsters[monsterId];
    const SimulatedPlayer& player = gameState.players[playerId];
    
    // Create a mock monster for scaling
    MonsterData mockMonster;
    mockMonster._mtype = monster.type;
    mockMonster._mMaxHP = monster.baseHP;
    mockMonster._mHitPoints = monster.baseHP;
    mockMonster._mMinDamage = monster.baseDamage / 2;
    mockMonster._mMaxDamage = monster.baseDamage;
    mockMonster._mAC = monster.baseAC;
    
    // Scale the monster based on player difficulty
    MonsterData scaledMonster = monsterScaling->ScaleMonster(mockMonster, player.currentDifficulty);
    
    // Format the result
    std::stringstream ss;
    ss << "Scaled " << monster.name << " for " << player.name << " (Difficulty: " << player.currentDifficulty << "):" << std::endl;
    ss << "  HP: " << monster.baseHP << " -> " << scaledMonster._mMaxHP << std::endl;
    ss << "  Damage: " << monster.baseDamage / 2 << "-" << monster.baseDamage << " -> ";
    ss << scaledMonster._mMinDamage << "-" << scaledMonster._mMaxDamage << std::endl;
    ss << "  AC: " << monster.baseAC << " -> " << scaledMonster._mAC;
    
    return ss.str();
}

std::string GameSimulation::ScaleMonsterForMultiplayer(int monsterId)
{
    if (monsterId < 0 || monsterId >= static_cast<int>(gameState.monsters.size())) {
        return "Invalid monster ID";
    }
    
    // Get monster
    const SimulatedMonster& monster = gameState.monsters[monsterId];
    
    // Calculate multiplayer difficulty
    float mpDifficulty = CalculateMultiplayerDifficulty();
    
    // Create a mock monster for scaling
    MonsterData mockMonster;
    mockMonster._mtype = monster.type;
    mockMonster._mMaxHP = monster.baseHP;
    mockMonster._mHitPoints = monster.baseHP;
    mockMonster._mMinDamage = monster.baseDamage / 2;
    mockMonster._mMaxDamage = monster.baseDamage;
    mockMonster._mAC = monster.baseAC;
    
    // Scale the monster based on multiplayer difficulty
    MonsterData scaledMonster = monsterScaling->ScaleMonster(mockMonster, mpDifficulty);
    
    // Format the result
    std::stringstream ss;
    ss << "Scaled " << monster.name << " for multiplayer (Difficulty: " << mpDifficulty << "):" << std::endl;
    ss << "  HP: " << monster.baseHP << " -> " << scaledMonster._mMaxHP << std::endl;
    ss << "  Damage: " << monster.baseDamage / 2 << "-" << monster.baseDamage << " -> ";
    ss << scaledMonster._mMinDamage << "-" << scaledMonster._mMaxDamage << std::endl;
    ss << "  AC: " << monster.baseAC << " -> " << scaledMonster._mAC;
    
    return ss.str();
}

bool GameSimulation::SaveDifficultyState(int playerId, const std::string& filePath)
{
    if (playerId < 0 || playerId >= static_cast<int>(gameState.players.size())) {
        return false;
    }
    
    // Create a mock player for saving
    Player mockPlayer;
    mockPlayer._pLevel = gameState.players[playerId].level;
    
    // Create a mock difficulty save manager
    // In a real implementation, this would use the actual DifficultySaveManager
    
    // For now, just write the difficulty to a file
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "DYNDIFF" << std::endl;
    file << "Version: 1" << std::endl;
    file << "PlayerID: " << playerId << std::endl;
    file << "PlayerName: " << gameState.players[playerId].name << std::endl;
    file << "PlayerLevel: " << gameState.players[playerId].level << std::endl;
    file << "GearLevel: " << gameState.players[playerId].gearLevel << std::endl;
    file << "CurrentDifficulty: " << gameState.players[playerId].currentDifficulty << std::endl;
    file << "GameTime: " << gameState.gameTime << std::endl;
    
    file.close();
    return true;
}

bool GameSimulation::LoadDifficultyState(int playerId, const std::string& filePath)
{
    if (playerId < 0 || playerId >= static_cast<int>(gameState.players.size())) {
        return false;
    }
    
    // Open the file
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read the header
    std::string header;
    std::getline(file, header);
    if (header != "DYNDIFF") {
        file.close();
        return false;
    }
    
    // Read the version
    std::string versionLine;
    std::getline(file, versionLine);
    
    // Read the player ID
    std::string playerIdLine;
    std::getline(file, playerIdLine);
    
    // Read the player name
    std::string playerNameLine;
    std::getline(file, playerNameLine);
    
    // Read the player level
    std::string playerLevelLine;
    std::getline(file, playerLevelLine);
    
    // Read the gear level
    std::string gearLevelLine;
    std::getline(file, gearLevelLine);
    
    // Read the current difficulty
    std::string difficultyLine;
    std::getline(file, difficultyLine);
    float difficulty = std::stof(difficultyLine.substr(difficultyLine.find(":") + 2));
    
    // Read the game time
    std::string gameTimeLine;
    std::getline(file, gameTimeLine);
    
    file.close();
    
    // Update the player's difficulty
    gameState.players[playerId].currentDifficulty = difficulty;
    
    return true;
}

std::string GameSimulation::GenerateReport()
{
    std::stringstream ss;
    
    ss << "=== GAME SIMULATION REPORT ===" << std::endl;
    ss << "Game Time: " << gameState.gameTime << std::endl;
    ss << "Wave Difficulty: " << gameState.waveDifficulty << std::endl;
    ss << "Multiplayer: " << (gameState.isMultiplayer ? "Yes" : "No") << std::endl;
    ss << std::endl;
    
    ss << "Players:" << std::endl;
    for (const auto& player : gameState.players) {
        ss << "  " << player.name << " (ID: " << player.id << ")" << std::endl;
        ss << "    Level: " << player.level << std::endl;
        ss << "    Gear Level: " << player.gearLevel << std::endl;
        ss << "    Current Difficulty: " << player.currentDifficulty << std::endl;
    }
    ss << std::endl;
    
    ss << "Monsters:" << std::endl;
    for (const auto& monster : gameState.monsters) {
        ss << "  " << monster.name << " (Type: " << monster.type << ")" << std::endl;
        ss << "    Base HP: " << monster.baseHP << std::endl;
        ss << "    Base Damage: " << monster.baseDamage / 2 << "-" << monster.baseDamage << std::endl;
        ss << "    Base AC: " << monster.baseAC << std::endl;
    }
    ss << std::endl;
    
    if (gameState.isMultiplayer) {
        ss << "Multiplayer Difficulty: " << CalculateMultiplayerDifficulty() << std::endl;
    }
    
    return ss.str();
}

void GameSimulation::UpdateDifficulties()
{
    // Update difficulty for each player
    for (auto& player : gameState.players) {
        // Create a mock player for difficulty calculation
        Player mockPlayer;
        mockPlayer._pLevel = player.level;
        
        // Calculate base difficulty
        float baseDifficulty = player.gearLevel;
        
        // Apply wave effect
        float waveDifficulty = difficultyWave->ApplyWaveEffect(baseDifficulty, gameState.gameTime);
        
        // Store the difficulty
        player.currentDifficulty = waveDifficulty;
    }
}

// TestScenario implementation
TestScenario::TestScenario(const std::string& name, const std::string& description)
    : name(name), description(description)
{
}

TestScenario::~TestScenario()
{
}

void TestScenario::AddSetupStep(std::function<void(GameSimulation&)> step)
{
    setupSteps.push_back(step);
}

void TestScenario::AddTestStep(std::function<bool(GameSimulation&, std::string&)> step)
{
    testSteps.push_back(step);
}

TestResult TestScenario::Run(GameSimulation& simulation)
{
    TestResult result(name);
    
    // Run setup steps
    for (const auto& step : setupSteps) {
        step(simulation);
    }
    
    // Run test steps
    for (size_t i = 0; i < testSteps.size(); i++) {
        std::string message;
        bool success = testSteps[i](simulation, message);
        result.RecordStepResult(static_cast<int>(i), success, message);
    }
    
    return result;
}

const std::string& TestScenario::GetName() const
{
    return name;
}

const std::string& TestScenario::GetDescription() const
{
    return description;
}

// TestResult implementation
TestResult::TestResult(const std::string& scenarioName)
    : scenarioName(scenarioName)
{
}

TestResult::~TestResult()
{
}

void TestResult::RecordStepResult(int stepIndex, bool success, const std::string& message)
{
    // Ensure the vectors are large enough
    if (stepIndex >= static_cast<int>(stepResults.size())) {
        stepResults.resize(stepIndex + 1);
        stepMessages.resize(stepIndex + 1);
    }
    
    // Record the result
    stepResults[stepIndex] = success;
    stepMessages[stepIndex] = message;
}

bool TestResult::AllStepsSucceeded() const
{
    return std::all_of(stepResults.begin(), stepResults.end(), [](bool result) { return result; });
}

const std::string& TestResult::GetScenarioName() const
{
    return scenarioName;
}

std::string TestResult::GetSummary() const
{
    std::stringstream ss;
    
    ss << "Scenario: " << scenarioName << std::endl;
    ss << "Result: " << (AllStepsSucceeded() ? "PASSED" : "FAILED") << std::endl;
    ss << "Steps: " << stepResults.size() << std::endl;
    
    for (size_t i = 0; i < stepResults.size(); i++) {
        ss << "  Step " << (i + 1) << ": " << (stepResults[i] ? "PASSED" : "FAILED") << std::endl;
        ss << "    " << stepMessages[i] << std::endl;
    }
    
    return ss.str();
}

// SystemVerificationTest implementation
SystemVerificationTest::SystemVerificationTest()
{
    simulation.Initialize();
}

SystemVerificationTest::~SystemVerificationTest()
{
}

void SystemVerificationTest::AddScenario(std::unique_ptr<TestScenario> scenario)
{
    scenarios.push_back(std::move(scenario));
}

bool SystemVerificationTest::RunAllScenarios()
{
    results.clear();
    
    for (const auto& scenario : scenarios) {
        TestResult result = scenario->Run(simulation);
        results.push_back(result);
    }
    
    return std::all_of(results.begin(), results.end(), [](const TestResult& result) { return result.AllStepsSucceeded(); });
}

std::string SystemVerificationTest::GenerateReport()
{
    std::stringstream ss;
    
    ss << "=== SYSTEM VERIFICATION TEST REPORT ===" << std::endl;
    ss << "Total Scenarios: " << scenarios.size() << std::endl;
    
    int passedScenarios = 0;
    for (const auto& result : results) {
        if (result.AllStepsSucceeded()) {
            passedScenarios++;
        }
    }
    
    ss << "Passed Scenarios: " << passedScenarios << "/" << scenarios.size() << std::endl;
    ss << "Success Rate: " << std::fixed << std::setprecision(2) << (static_cast<float>(passedScenarios) / scenarios.size() * 100.0f) << "%" << std::endl;
    ss << std::endl;
    
    for (const auto& result : results) {
        ss << result.GetSummary() << std::endl;
    }
    
    return ss.str();
}

bool SystemVerificationTest::SaveReportToFile(const std::string& filePath)
{
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    file << GenerateReport();
    file.close();
    
    return true;
}

} // namespace testing
} // namespace devilution
