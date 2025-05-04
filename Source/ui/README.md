# Gear Level UI Display System

## Overview

The Gear Level UI Display System provides visual feedback about the player's gear level in the game interface. It shows the calculated gear level in the character screen, displays gear level changes when comparing items, and includes visual representations of gear quality through color coding, star ratings, or tier labels.

## Key Components

### 1. Gear Level UI Manager

The core component that handles rendering gear level information:

- Displays gear level in the character screen
- Shows gear level in the inventory
- Provides item comparison with gear level changes
- Supports multiple display styles (numeric, stars, color-coded, tier-based)

### 2. Display Styles

The system supports several display styles for gear levels:

- **Numeric**: Simple numeric display of the gear level value
- **Stars**: Star rating (1-5 stars) based on gear level
- **Color-Coded**: Color-coded numeric display (white, blue, yellow, orange, red)
- **Tier**: Tier-based display (Basic, Magic, Rare, Epic, Legendary)

## Configuration

The system is configured through the `gear_level_ui_config.json` file, which defines:

- Where to show gear level information
- Which display style to use
- Color thresholds and tier names
- UI element positions

Example configuration:

```json
{
  "gearLevelDisplay": {
    "showInCharScreen": true,
    "showInInventory": true,
    "showInItemCompare": true,
    "showChangeIndicator": true,
    "style": "colorCoded"
  }
}
```

### Configuration Parameters

- **showInCharScreen**: Whether to show gear level in character screen
- **showInInventory**: Whether to show gear level in inventory
- **showInItemCompare**: Whether to show gear level when comparing items
- **showChangeIndicator**: Whether to show up/down indicators for changes
- **style**: Display style to use (numeric, stars, colorCoded, tier)

## Usage

### Rendering Gear Level in Character Screen

```cpp
// Get the gear level UI manager
GearLevelUI &ui = GearLevelUI::GetInstance();

// Render the gear level in the character screen
ui.RenderCharScreenGearLevel(player, x, y);
```

### Rendering Gear Level in Inventory

```cpp
// Get the gear level UI manager
GearLevelUI &ui = GearLevelUI::GetInstance();

// Render the gear level in the inventory
ui.RenderInventoryGearLevel(player, x, y);
```

### Rendering Item Comparison

```cpp
// Get the gear level UI manager
GearLevelUI &ui = GearLevelUI::GetInstance();

// Render the gear level comparison
ui.RenderItemCompareGearLevel(player, currentItem, newItem, slot, x, y);
```

## Design Principles

1. **Non-Intrusive**: Position UI elements to avoid interfering with existing UI
2. **Informative**: Provide clear visual feedback about gear quality
3. **Aesthetic**: Match the game's existing visual style
4. **Configurable**: Allow players to customize the display

## Visual Styles

### Color Coding

Gear levels are color-coded based on quality thresholds:
- **White** (< 20): Low quality gear
- **Blue** (20-40): Moderate quality gear
- **Yellow** (40-60): Good quality gear
- **Orange** (60-80): Very good quality gear
- **Red** (80+): Excellent quality gear

### Star Rating

Gear levels are represented as a star rating:
- **★☆☆☆☆** (< 20): 1 star
- **★★☆☆☆** (20-40): 2 stars
- **★★★☆☆** (40-60): 3 stars
- **★★★★☆** (60-80): 4 stars
- **★★★★★** (80+): 5 stars

### Tier Labels

Gear levels are represented as quality tiers:
- **Basic** (< 20): Entry-level gear
- **Magic** (20-40): Enhanced gear
- **Rare** (40-60): Superior gear
- **Epic** (60-80): Exceptional gear
- **Legendary** (80+): Ultimate gear

## Integration with Core Systems

This system integrates with the Gear Level Manager to retrieve and display the player's current gear level and potential gear level changes when comparing items. It provides visual feedback that helps players understand the impact of their equipment choices on their overall gear level.
