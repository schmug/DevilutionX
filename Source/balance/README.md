# Dynamic Difficulty Balance Utility

## Overview

The Balance Utility provides tools for fine-tuning and balancing the dynamic difficulty system. It helps ensure appropriate difficulty scaling and gameplay experience by simulating various scenarios, analyzing results, and recommending parameter adjustments.

## Key Components

### 1. Balance Test Utility

The core component that simulates and analyzes gameplay scenarios:

- Runs predefined test scenarios with different player levels and gear levels
- Simulates combat between players and monsters at various difficulty levels
- Analyzes results to identify balance issues
- Recommends parameter adjustments

### 2. Parameter Adjustment Tools

Tools for adjusting various system parameters:

- Gear scoring weights and multipliers
- Monster stat scaling factors
- Difficulty transition parameters
- Smoothing algorithms and wave effects

## Test Scenarios

The system includes predefined test scenarios that cover different gameplay situations:

- **Early Game**: Players just starting out with basic gear
- **Mid Game**: Players with moderate gear progression
- **Late Game**: Players with high-end gear
- **Gear Spike**: Players with gear much better than expected for their level
- **Gear Deficit**: Players with gear much worse than expected for their level

## Configuration

The system is configured through the `balance_config.json` file, which defines:

- Test scenarios to run
- Target balance metrics
- Default parameter values
- Adjustment factors

## Usage

### Running Balance Tests

```cpp
// Get the balance utility
BalanceUtility &utility = BalanceUtility::GetInstance();

// Initialize the utility
utility.Initialize();

// Create test scenarios
std::vector<BalanceTestScenario> scenarios = utility.CreatePredefinedScenarios();

// Run the tests
std::vector<BalanceTestResults> results = utility.RunTestBatch(scenarios);

// Generate a report
std::string report = utility.GenerateTestReport(results);
```

### Adjusting Parameters

```cpp
// Get the balance utility
BalanceUtility &utility = BalanceUtility::GetInstance();

// Run tests
std::vector<BalanceTestResults> results = utility.RunTestBatch(scenarios);

// Get the current gear scoring configuration
GearScoringConfig &gearConfig = GearLevelManager::GetInstance().GetConfig();

// Adjust gear scoring parameters
bool gearAdjusted = utility.AdjustGearScoringParams(results, gearConfig);

// Get the current monster scaling parameters
auto &monsterDefs = MonsterScalingManager::GetInstance().GetMonsterDefinitions();

// Adjust monster scaling parameters
bool monsterAdjusted = utility.AdjustMonsterScalingParams(results, monsterDefs);

// Get the current difficulty transition parameters
DifficultyTransitionParams &diffParams = DifficultyManager::GetInstance().GetTransitionParams();

// Adjust difficulty transition parameters
bool diffAdjusted = utility.AdjustDifficultyTransitionParams(results, diffParams);
```

## Balance Metrics

The system tracks several key metrics to evaluate balance:

- **Player Survival Rate**: The percentage of encounters where the player survives
- **Time to Kill**: The average number of combat rounds needed to defeat monsters
- **Damage Taken**: The average amount of damage taken by the player
- **Difficulty Level**: The calculated difficulty level based on player gear

## Target Balance Ranges

For optimal gameplay experience, the system aims for these target ranges:

- **Player Survival Rate**: 30-80% (optimal: 50%)
- **Time to Kill**: 5-15 rounds (optimal: 10 rounds)
- **Difficulty Level**: 0.5-2.0 (optimal: 1.0)

## Simulation Model

The balance utility uses a simplified combat simulation model:

1. Create mock players with specified level and gear level
2. Create mock monsters scaled to the current difficulty level
3. Simulate combat rounds with randomized damage calculations
4. Track results including survival rate, time to kill, and damage taken

## Adjustment Recommendations

Based on test results, the system recommends parameter adjustments such as:

- Increasing/decreasing monster health scaling
- Adjusting gear quality multipliers
- Modifying affix weights
- Tuning difficulty transition speed
- Adjusting smoothing factors

## Integration with Core Systems

This utility integrates with the Gear Level Manager, Monster Scaling System, and Difficulty Manager to provide a comprehensive balancing solution for the dynamic difficulty system.
