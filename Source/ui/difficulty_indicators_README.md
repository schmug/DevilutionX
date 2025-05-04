# Difficulty Indicators System

## Overview

The Difficulty Indicators System provides visual feedback about area difficulty levels on the minimap and main map. It helps players identify challenging areas and make informed decisions about where to explore based on their current gear level and abilities.

## Key Components

### 1. Difficulty Indicators Manager

The core component that handles rendering difficulty indicators:

- Displays difficulty levels on the minimap
- Shows difficulty levels on the main map
- Highlights area transitions with significant difficulty changes
- Provides a legend for interpreting the indicators

### 2. Display Styles

The system supports several display styles for difficulty indicators:

- **Color Gradient**: Color-coded indicators from green (easy) to red (hard)
- **Number Scale**: Numeric scale (1-5) based on difficulty level
- **Icon Set**: Different icons for each difficulty level
- **Pulse Intensity**: Pulsing indicators with varying intensity based on difficulty

## Configuration

The system is configured through the `difficulty_indicators_config.json` file, which defines:

- Where to show difficulty indicators
- Which display style to use
- Color thresholds and difficulty labels
- UI element positions

Example configuration:

```json
{
  "difficultyIndicators": {
    "showOnMinimap": true,
    "showOnMainMap": true,
    "showAreaTransitions": true,
    "showLegend": true,
    "style": "colorGradient"
  }
}
```

### Configuration Parameters

- **showOnMinimap**: Whether to show indicators on the minimap
- **showOnMainMap**: Whether to show indicators on the main map
- **showAreaTransitions**: Whether to highlight area transitions with different difficulty
- **showLegend**: Whether to show a legend for the indicators
- **style**: Display style to use (colorGradient, numberScale, iconSet, pulseIntensity)

## Usage

### Rendering Difficulty Indicators on Minimap

```cpp
// Get the difficulty indicators manager
DifficultyIndicators &indicators = DifficultyIndicators::GetInstance();

// Initialize the indicators
indicators.Initialize();

// Update the indicators (call this every frame)
indicators.Update(gameTime);

// Render the indicators on the minimap (call this when rendering the minimap)
indicators.RenderMinimapIndicators();
```

### Rendering Difficulty Indicators on Main Map

```cpp
// Get the difficulty indicators manager
DifficultyIndicators &indicators = DifficultyIndicators::GetInstance();

// Render the indicators on the main map (call this when rendering the main map)
indicators.RenderMainMapIndicators();
```

## Design Principles

1. **Informative**: Provide clear visual feedback about area difficulty
2. **Non-Intrusive**: Position indicators to avoid interfering with map readability
3. **Configurable**: Allow players to customize the display
4. **Responsive**: Update indicators based on player's current gear level and abilities

## Visual Styles

### Color Gradient

Difficulty levels are color-coded:
- **Green**: Easy areas (< 20)
- **Blue**: Normal areas (20-40)
- **Yellow**: Hard areas (40-60)
- **Orange**: Very hard areas (60-80)
- **Red**: Extreme areas (80+)

### Number Scale

Difficulty levels are represented as a numeric scale:
- **1**: Easy areas (< 20)
- **2**: Normal areas (20-40)
- **3**: Hard areas (40-60)
- **4**: Very hard areas (60-80)
- **5**: Extreme areas (80+)

### Icon Set

Difficulty levels are represented by different icons:
- **Icon 0**: Easy areas (< 20)
- **Icon 1**: Normal areas (20-40)
- **Icon 2**: Hard areas (40-60)
- **Icon 3**: Very hard areas (60-80)
- **Icon 4**: Extreme areas (80+)

### Pulse Intensity

Difficulty levels are represented by pulsing indicators:
- **Low frequency, low intensity**: Easy areas (< 20)
- **Medium frequency, medium intensity**: Normal to hard areas (20-60)
- **High frequency, high intensity**: Very hard to extreme areas (60+)

## Area Transitions

The system can highlight transitions between areas with significant difficulty differences:
- Highlights transitions with a difference greater than a configurable threshold
- Can show direction arrows indicating whether the adjacent area is harder or easier
- Helps players avoid accidentally entering areas that are too challenging

## Integration with Core Systems

This system integrates with the Difficulty Manager to retrieve area difficulty levels based on the player's current gear level and abilities. It provides visual feedback that helps players understand the challenge level of different areas in the game.
