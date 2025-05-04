# Difficulty Transition System

## Overview

The Difficulty Transition System provides smooth transitions between difficulty levels as player gear level changes. It prevents sudden difficulty spikes when equipping new items and allows for a temporary "overpowered" feeling after major upgrades before gradually normalizing the challenge.

## Key Components

### 1. Difficulty Manager

The core component that handles difficulty transitions:

- Smoothly interpolates between current and target difficulty levels
- Uses different rates for difficulty increases vs. decreases
- Applies moving averages for additional smoothing
- Provides temporary "overpowered" periods after significant gear upgrades
- Supports multiple interpolation methods (linear, sigmoid, exponential, logarithmic)

### 2. Difficulty Debug Utilities

Tools for testing, visualizing, and balancing the transition system:

- Simulates gear level changes and records difficulty transitions
- Generates visualizations of difficulty curves
- Compares different transition parameter sets
- Tests the overpower effect
- Exports transition data to CSV for further analysis

## Configuration

The system is configured through the `difficulty_config.json` file, which defines:

- Transition rates for increases and decreases
- Overpower factor and duration
- History size for moving averages
- Interpolation type

Example configuration:

```json
{
  "difficultyTransition": {
    "increaseRate": 0.08,
    "decreaseRate": 0.15,
    "overpowerFactor": 1.5,
    "overpowerDuration": 300.0,
    "historySize": 10,
    "interpolationType": "sigmoid"
  }
}
```

### Configuration Parameters

- **increaseRate**: Rate at which difficulty increases (0-1, lower is slower)
- **decreaseRate**: Rate at which difficulty decreases (0-1, lower is slower)
- **overpowerFactor**: Factor for temporary overpowered feeling after upgrades (>1)
- **overpowerDuration**: Duration of overpowered feeling in game time units
- **historySize**: Size of the history buffer for moving averages
- **interpolationType**: Type of interpolation to use (linear, sigmoid, exponential, logarithmic)

## Usage

### Updating Difficulty

```cpp
// Get the difficulty manager
DifficultyManager &manager = DifficultyManager::GetInstance();

// Update difficulty for a player
float gameTime = GetGameTime();
manager.UpdateDifficulty(player, gameTime);

// Get the current smoothed difficulty
float difficulty = manager.GetCurrentDifficulty(player);
```

### Triggering Overpower Effect

```cpp
// When a player equips a significant upgrade
DifficultyManager &manager = DifficultyManager::GetInstance();
float gameTime = GetGameTime();
manager.TriggerOverpower(player, gameTime);
```

### Using Difficulty in Monster Scaling

```cpp
// When scaling a monster
float difficulty = DifficultyManager::GetInstance().GetCurrentDifficulty(player);
MonsterData monster = CreateMonster(type, level);
ScaleMonsterWithDifficulty(monster, difficulty);
```

### Debugging and Visualization

```cpp
// Simulate a gear level change
DifficultyDebug &debug = DifficultyDebug::GetInstance();
auto dataPoints = debug.SimulateGearChange(20.0f, 40.0f, 100.0f, 1.0f, params);

// Visualize the transition
std::string visualization = debug.VisualizeTransition(dataPoints);
LogInfo(visualization);

// Compare different parameter sets
std::vector<DifficultyTransitionParams> paramSets = { params1, params2, params3 };
std::string comparison = debug.CompareTransitionParams(20.0f, 40.0f, 100.0f, 1.0f, paramSets);
LogInfo(comparison);
```

## Design Principles

1. **Smoothness**: Prevent jarring difficulty changes that could frustrate players
2. **Asymmetry**: Difficulty increases more slowly than it decreases
3. **Reward**: Provide temporary power boosts after significant upgrades
4. **Configurability**: Allow easy tuning of all transition parameters
5. **Transparency**: Provide tools to visualize and understand difficulty changes

## Balancing Guidelines

- **Increase Rate**: Keep low (0.05-0.1) to prevent sudden difficulty spikes
- **Decrease Rate**: Set higher than increase rate (1.5-2x) for better player experience
- **Overpower Factor**: 1.2-1.5 for normal upgrades, up to 2.0 for major upgrades
- **Overpower Duration**: 200-500 game time units depending on desired effect
- **History Size**: 5-15 depending on desired smoothness (higher = smoother but more lag)
- **Interpolation Type**: Sigmoid works well for most cases, exponential for more dramatic changes

## Integration with Gear Scoring

This system relies on the Gear Scoring System to calculate player gear levels. The integration ensures that as players find better equipment, the game's challenge scales appropriately but with smooth transitions.
