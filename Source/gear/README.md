# Dynamic Difficulty System - Gear Scoring Documentation

## Overview

The Dynamic Difficulty System adjusts monster difficulty based on the player's gear level. This document explains the gear scoring system that evaluates equipment quality and calculates the player's overall gear level.

## Architecture

The gear scoring system consists of several components:

1. **Base Scoring System** - Evaluates item quality and type
2. **Affix Scoring System** - Evaluates magical properties on items
3. **Specialized Scoring System** - Applies category-specific rules for weapons, armor, and jewelry
4. **Composite Scoring System** - Combines all scoring components into a final gear score
5. **Configuration System** - Allows tuning of scoring parameters without code changes

### Directory Structure

```
Source/gear/
├── gear_score.h/cpp     - Core gear scoring interfaces and base implementation
├── affix_score.h/cpp    - Affix evaluation system
├── specialized_score.h/cpp - Category-specific scoring rules
├── composite_score.h/cpp - Combined scoring system
├── gear_config.h/cpp    - Configuration system
├── README.md            - This documentation
```

## Scoring Components

### Base Item Scoring

Base scoring evaluates items based on:
- **Quality** (Normal, Magic, Rare, Unique, etc.)
- **Category** (Weapon, Armor, Shield, Helm, Jewelry, etc.)
- **Item Level** - Higher level items receive a scaling bonus

### Affix Scoring

Affix scoring evaluates magical properties based on:
- **Affix Type** - Different weights for different types of affixes
- **Affix Value** - Higher values receive proportionally higher scores
- **Item Category** - The same affix may be worth more on certain item types
  - Example: +Strength is more valuable on weapons, +Magic is more valuable on jewelry

### Specialized Scoring

Category-specific scoring rules:
- **Weapons** - Evaluates DPS, elemental damage, critical hit bonuses
- **Armor** - Evaluates damage reduction, resistances, defensive stats
- **Jewelry** - Evaluates utility effects, proc chances, unique bonuses

### Composite Scoring

The final score is a weighted combination of:
- Base item score
- Affix score
- Specialized category score
- Level factor (based on item level)
- Special bonuses (for unique or set items)

### Gear Level Calculation

The player's overall gear level is calculated as:
1. Calculate score for each equipped item
2. Apply slot importance weights (weapons matter more than rings)
3. Calculate weighted average of all equipped items
4. Factor in character level with diminishing returns
5. Normalize to a meaningful range (1-100)

## Configuration System

The scoring system is highly configurable through the `GearScoringConfig` structure:

- **Quality Scores** - Base scores for each quality tier
- **Category Multipliers** - Importance multipliers for each item category
- **Slot Importance** - Weights for each equipment slot
- **Affix Scores** - Scoring parameters for each affix type
- **Formula Parameters** - Controls for level scaling, unique bonuses, etc.
- **Normalization Parameters** - Controls for the final gear level range

## Extending the System

### Adding New Item Types

To add a new item type:
1. Add the new type to the `GearCategory` enum in `gear_score.h`
2. Add appropriate multipliers in the configuration system
3. Update the `GetGearCategory` function to recognize the new type
4. If needed, create a specialized scorer for the new category

### Adding New Affix Types

To add a new affix type:
1. Add the new affix to the `AffixDefinitions` array in `affix_score.cpp`
2. Define appropriate weights and multipliers
3. Update the `CalculateItemAffixScore` function to evaluate the new affix

## Balancing Guidelines

When balancing the scoring system:

1. **Quality Balance** - Unique items should score higher than magic items, but not overwhelmingly so
2. **Slot Balance** - Weapons and armor should have more impact than jewelry
3. **Affix Balance** - Offensive stats should be balanced against defensive stats
4. **Level Scaling** - Higher level items should score better, but with diminishing returns
5. **Character Level Impact** - Character level should matter, but gear should matter more

## Debugging and Testing

The system includes several debugging tools:

1. **Score Explanation** - Shows a detailed breakdown of how an item's score was calculated
2. **Verbose Logging** - Logs detailed scoring information when enabled
3. **Configuration System** - Allows easy adjustment of scoring parameters

## Example Usage

```cpp
// Calculate gear level for a player
float gearLevel = CalculateGearLevel(player);

// Get detailed score breakdown for an item
ItemScoreBreakdown breakdown = GearScorer::CalculateDetailedItemScore(item);

// Get text explanation of an item's score
std::string explanation = GearScorer::GetScoreExplanation(item);
```
