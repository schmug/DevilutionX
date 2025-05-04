# Monster Stat Scaling System

## Overview

The Monster Stat Scaling System dynamically adjusts monster statistics based on the player's gear level, creating a more balanced and engaging gameplay experience. As players acquire better equipment, monsters become more challenging, ensuring the game remains interesting throughout the player's progression.

## Key Components

### 1. Monster Stat Scaler

The core component that handles the scaling of monster statistics. It supports various scaling curves:

- **Linear**: Simple linear scaling (base + factor * gearLevel)
- **Exponential**: Exponential growth (base * (1 + factor * gearLevel))
- **Logarithmic**: Logarithmic scaling for diminishing returns (base * (1 + factor * log(gearLevel)))
- **Sigmoid**: S-curve scaling with smooth transitions (base * (1 + factor * sigmoid(gearLevel)))
- **Custom**: Support for custom scaling functions

### 2. Monster Scaling Integration

Integrates the scaling system with the game's monster creation and combat systems:

- Applies scaling when monsters are created
- Adjusts monster damage during combat
- Scales special ability effectiveness
- Maintains consistency by tracking the gear level used for each monster

### 3. Monster Scaling Debug

Provides tools for testing, visualizing, and balancing the scaling system:

- Generates scaling reports across gear level ranges
- Compares base stats to scaled stats
- Simulates combat between players and scaled monsters
- Visualizes scaling curves for individual stats

## Configuration

The system is highly configurable through the `monster_scaling.json` file, which defines:

- Base stats for each monster type
- Scaling parameters (factor, min/max values, curve type) for each stat
- Monster-specific scaling behavior

Example configuration for a monster:

```json
{
  "type": 0,
  "level": 1,
  "name": "Zombie",
  "baseStats": {
    "hitPoints": 50,
    "damageMin": 3,
    "damageMax": 6,
    "armorClass": 10,
    "toHitChance": 30,
    "specialEffectiveness": 1.0
  },
  "scaling": {
    "hitPoints": {
      "factor": 0.15,
      "min": 50,
      "max": 500,
      "curve": "exponential"
    },
    "damage": {
      "factor": 0.1,
      "min": 3,
      "max": 50,
      "curve": "linear"
    },
    "armorClass": {
      "factor": 0.05,
      "min": 10,
      "max": 100,
      "curve": "logarithmic"
    },
    "toHitChance": {
      "factor": 0.05,
      "min": 30,
      "max": 80,
      "curve": "sigmoid"
    },
    "specialEffectiveness": {
      "factor": 0.05,
      "min": 1.0,
      "max": 2.0,
      "curve": "linear"
    }
  }
}
```

## Usage

### Integrating with Monster Creation

```cpp
// When creating a monster
MonsterData monster = CreateMonster(type, level);

// Get the player's gear level
Player &player = GetActivePlayer();

// Apply scaling to the monster
MonsterScalingIntegration::GetInstance().ApplyScalingToMonster(monster, player);
```

### Scaling Monster Damage in Combat

```cpp
// When a monster deals damage
int baseDamage = CalculateBaseDamage(monster);
int scaledDamage = MonsterScalingIntegration::GetInstance().ScaleMonsterDamage(monster, player, baseDamage);
ApplyDamageToPlayer(player, scaledDamage);
```

### Debugging and Balancing

```cpp
// Generate a scaling report for a monster
MonsterData monster = GetMonster(MONSTER_ZOMBIE);
std::string report = MonsterScalingDebug::GetInstance().GenerateScalingReport(monster, 0.0f, 100.0f, 10.0f);
LogInfo(report);

// Visualize hit point scaling
std::string visualization = MonsterScalingDebug::GetInstance().VisualizeStatScaling(monster, "Hit Points", 0.0f, 100.0f, 10.0f);
LogInfo(visualization);

// Simulate combat
std::string combatReport = MonsterScalingDebug::GetInstance().SimulateCombat(player, monster, 10);
LogInfo(combatReport);
```

## Design Principles

1. **Balance**: Monsters should provide an appropriate challenge without becoming damage sponges
2. **Smoothness**: Difficulty increases gradually to avoid sudden spikes
3. **Configurability**: All parameters are easily adjustable for balancing
4. **Performance**: Efficient implementation with caching where appropriate
5. **Transparency**: Debug tools provide insight into the scaling behavior

## Balancing Guidelines

- **Hit Points**: Scale exponentially but with reasonable limits
- **Damage**: Scale linearly to maintain predictable threat
- **Armor Class**: Use logarithmic scaling to prevent extreme values
- **To-Hit Chance**: Use sigmoid scaling to ensure reasonable bounds
- **Special Effectiveness**: Scale linearly but with lower factors

## Integration with Gear Scoring

This system relies on the Gear Scoring System to calculate player gear levels. The integration ensures that as players find better equipment, the game's challenge scales appropriately.
