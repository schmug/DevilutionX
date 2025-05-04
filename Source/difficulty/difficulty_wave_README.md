# Difficulty Wave Effects System

## Overview

The Difficulty Wave Effects System creates wave-like variations in difficulty to provide more dynamic and varied gameplay. It modulates the base difficulty level over time using various wave patterns, creating periods of increased and decreased challenge that prevent the game from feeling static.

## Key Components

### 1. Wave Generator

The core component that generates wave patterns:

- Produces various wave patterns (sine, square, triangle, sawtooth, random)
- Supports compound waves combining multiple patterns
- Applies wave effects to base difficulty values

### 2. Wave Parameters

Configurable parameters that control wave behavior:

- **Pattern Type**: The type of wave pattern to use
- **Amplitude**: How much the difficulty varies (higher = more variation)
- **Frequency**: How often the difficulty changes (higher = more frequent changes)
- **Phase**: Offset in the wave cycle (shifts the wave pattern)
- **Baseline**: Center point of the wave (default difficulty level)

## Wave Patterns

The system supports several wave patterns, each creating a different gameplay experience:

- **Sine**: Smooth, gradual transitions between easier and harder periods
- **Square**: Abrupt transitions between distinct easy and hard phases
- **Triangle**: Linear increases and decreases in difficulty
- **Sawtooth**: Gradual increase in difficulty followed by a sudden drop
- **Random**: Unpredictable fluctuations in difficulty
- **Compound**: Combination of multiple patterns for complex variations

## Configuration

The system is configured through the `difficulty_wave_config.json` file, which defines:

- Default wave parameters
- Compound wave components
- Preset configurations
- Gameplay settings

Example configuration:

```json
{
  "waveParameters": {
    "patternType": "compound",
    "amplitude": 0.2,
    "frequency": 0.1,
    "phase": 0.0,
    "baseline": 1.0
  }
}
```

## Usage

### Initializing the Wave System

```cpp
// Get the difficulty wave system
DifficultyWave &wave = DifficultyWave::GetInstance();

// Initialize the system
wave.Initialize();
```

### Updating the Wave

```cpp
// Update the wave (call this every frame with the current game time)
wave.Update(gameTime);
```

### Applying Wave Effects to Difficulty

```cpp
// Get the difficulty manager
DifficultyManager &diffManager = DifficultyManager::GetInstance();

// Get the base difficulty for a player
float baseDifficulty = diffManager.CalculateDifficulty(player);

// Get the difficulty wave system
DifficultyWave &wave = DifficultyWave::GetInstance();

// Apply wave effects to the base difficulty
float modifiedDifficulty = wave.ApplyWaveEffect(baseDifficulty, gameTime);

// Use the modified difficulty for monster scaling
```

## Gameplay Impact

The wave system creates a more dynamic difficulty curve that:

1. **Prevents Monotony**: Difficulty variations keep gameplay fresh and engaging
2. **Creates Rhythm**: Alternating periods of higher and lower challenge create a natural gameplay rhythm
3. **Adds Unpredictability**: Players can't always predict when difficulty will increase or decrease
4. **Provides Breathing Room**: Periods of lower difficulty allow players to recover and prepare
5. **Creates Memorable Moments**: Intensity spikes create memorable gameplay experiences

## Presets

The system includes several presets for different gameplay experiences:

- **Subtle**: Gentle variations for a more consistent experience
- **Moderate**: Balanced variations suitable for most gameplay
- **Intense**: Stronger variations with more dramatic difficulty spikes
- **Chaotic**: Unpredictable difficulty changes for a more random experience
- **Pulsing**: Distinct alternating periods of easy and hard gameplay

## Integration with Core Systems

This system integrates with the Difficulty Manager to modify the calculated difficulty level based on wave patterns. The modified difficulty is then used for monster scaling, creating a more dynamic and varied gameplay experience.

## Customization

Players can customize the wave effects through in-game options:

- Enable/disable wave effects
- Adjust wave effect strength
- Select different wave patterns
- Choose from presets

## Technical Implementation

The wave system uses mathematical functions to generate wave patterns:

- **Sine Wave**: `amplitude * sin(2 * pi * frequency * time + phase)`
- **Square Wave**: `amplitude * sign(sin(2 * pi * frequency * time + phase))`
- **Triangle Wave**: `amplitude * (2 * abs(frac(frequency * time + phase / (2 * pi)) - 0.5) - 0.5)`
- **Sawtooth Wave**: `amplitude * (2 * frac(frequency * time + phase / (2 * pi)) - 1)`
- **Random Wave**: `amplitude * random(-1, 1)`
- **Compound Wave**: Weighted sum of component waves
