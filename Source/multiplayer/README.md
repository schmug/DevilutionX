# Multiplayer Difficulty System

## Overview

The Multiplayer Difficulty System extends the dynamic difficulty system to work correctly in multiplayer games with players of varying gear levels. It provides several methods for calculating difficulty in multiplayer scenarios, handles synchronization between clients, and supports zone-based difficulty where monsters near a specific player scale to that player's level.

## Key Components

### 1. Multiplayer Difficulty Manager

The core component that handles difficulty in multiplayer games:

- Calculates combined or averaged gear levels for all players
- Implements multiple difficulty calculation methods
- Scales difficulty based on the number of players
- Handles edge cases like players with vastly different gear levels
- Synchronizes difficulty calculations between clients

### 2. Difficulty Calculation Methods

The system supports several methods for calculating difficulty in multiplayer:

- **Average**: Simple average of all player difficulties
- **Maximum**: Maximum difficulty among all players
- **Weighted**: Weighted average based on proximity to monsters
- **Per-Player**: Zone-based difficulty where monsters scale to the nearest player
- **Host**: Uses the host player's difficulty for all monsters

## Configuration

The system is configured through the `mp_difficulty_config.json` file, which defines:

- Difficulty calculation method
- Player count scaling factor
- Maximum allowed variance between player difficulties
- Proximity weight for weighted calculations
- Whether to synchronize difficulty across clients

Example configuration:

```json
{
  "mpDifficulty": {
    "method": "weighted",
    "playerCountScaling": 0.1,
    "maxPlayerDiffVariance": 20.0,
    "proximityWeight": 0.5,
    "syncDifficulty": true
  }
}
```

### Configuration Parameters

- **method**: Difficulty calculation method (average, maximum, weighted, perPlayer, host)
- **playerCountScaling**: Factor to scale difficulty by player count (0.0 = no scaling)
- **maxPlayerDiffVariance**: Maximum allowed variance between player difficulties
- **proximityWeight**: Weight for proximity-based calculations (higher = more weight to closer players)
- **syncDifficulty**: Whether to synchronize difficulty across clients

## Usage

### Getting Effective Difficulty for a Monster

```cpp
// Get the multiplayer difficulty manager
MpDifficultyManager &manager = MpDifficultyManager::GetInstance();

// Get the effective difficulty for a monster
float difficulty = manager.GetEffectiveDifficulty(monster);

// Scale the monster based on the difficulty
ScaleMonsterWithDifficulty(monster, difficulty);
```

### Using Zone-Based Difficulty

```cpp
// Set the difficulty method to per-player
MpDifficultyParams params = manager.GetParams();
params.method = MpDifficultyMethod::PerPlayer;
manager.SetParams(params);

// Find the nearest player to a monster
int nearestPlayerId = manager.FindNearestPlayer(monster);

// Get the effective difficulty for the monster
float difficulty = manager.GetEffectiveDifficulty(monster, nearestPlayerId);
```

### Handling Player Joins and Leaves

```cpp
// When a player joins
manager.OnPlayerJoin(playerId);

// When a player leaves
manager.OnPlayerLeave(playerId);

// Update the difficulty
float gameTime = GetGameTime();
manager.Update(gameTime);
```

## Design Principles

1. **Fairness**: Provide an appropriate challenge for all players regardless of gear level
2. **Flexibility**: Support multiple difficulty calculation methods for different playstyles
3. **Scalability**: Adjust difficulty based on the number of players
4. **Locality**: Allow monsters to scale based on nearby players
5. **Synchronization**: Keep difficulty consistent across all clients

## Balancing Guidelines

- **Player Count Scaling**: 0.05-0.15 (higher = more difficult with more players)
- **Max Player Diff Variance**: 15-30 (lower = more normalized difficulty)
- **Proximity Weight**: 0.3-0.7 (higher = more weight to closer players)

## Recommended Presets

The system includes several presets for different playstyles:

- **Balanced**: Weighted difficulty with moderate player count scaling
- **Challenging**: Maximum difficulty with higher player count scaling
- **Casual**: Average difficulty with lower player count scaling
- **Zone-Based**: Per-player difficulty with high variance allowance
- **Host-Controlled**: Host's difficulty with no variance

## Integration with Core Systems

This system builds on the Gear Scoring System and Difficulty Transition System, extending them to work in multiplayer scenarios. It ensures that the dynamic difficulty system provides an appropriate challenge for all players in a multiplayer game, regardless of their individual gear levels.
