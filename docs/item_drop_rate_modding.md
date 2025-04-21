# Item Drop Rate Modification System

This document provides an overview of the Item Drop Rate Modification System for DevilutionX, which allows modders to customize item drop rates without modifying the core game code.

## Overview

The Item Drop Rate Modification System provides a flexible, configuration-driven approach to adjusting item drop rates in the game. It allows for:

- Global drop rate adjustments
- Item type-specific adjustments (e.g., weapons, armor)
- Item class-specific adjustments (e.g., swords, bows)
- Context-specific adjustments (e.g., monster drops vs. chest drops)
- Conditional adjustments based on game state (e.g., difficulty level, dungeon level)

## Configuration File

The system uses a JSON configuration file located at:
```
<user_data_dir>/mods/config/drop_rate_config.json
```

If this file doesn't exist when the game starts, a default configuration will be created automatically.

### Configuration Schema

The configuration file follows this schema:

```json
{
  "maxDropRateMultiplier": 10.0,
  "minDropRateMultiplier": 0.1,
  "modifiers": [
    {
      "scope": "global|itemType|itemClass|itemId|uniqueItem",
      "targetId": "string",
      "context": "always|monsterDrop|chestDrop|groundDrop|questReward",
      "modType": "multiplier|addFlat|setAbsolute",
      "value": number,
      "priority": number,
      "condition": {
        "minMonsterLevel": number,
        "maxMonsterLevel": number,
        "minDungeonLevel": number,
        "maxDungeonLevel": number,
        "difficulty": number,
        "multiplayerOnly": boolean
      }
    }
  ]
}
```

### Configuration Options

#### Global Settings

- `maxDropRateMultiplier`: Maximum allowed multiplier for drop rates (safeguard)
- `minDropRateMultiplier`: Minimum allowed multiplier for drop rates (safeguard)

#### Modifier Properties

- `scope`: Defines what items this modifier applies to
  - `global`: Applies to all items
  - `itemType`: Applies to a specific item type
  - `itemClass`: Applies to a specific item class
  - `itemId`: Applies to a specific item ID
  - `uniqueItem`: Applies to all unique items

- `targetId`: The ID of the target (required for all scopes except global)
  - For `itemType`: Use "Sword", "Bow", "Shield", etc.
  - For `itemClass`: Use "Weapon", "Armor", "Misc", etc.
  - For `itemId`: Use the item's ID (not yet fully implemented)

- `context`: When this modifier should be applied
  - `always`: Always apply this modifier
  - `monsterDrop`: Apply only when monsters drop items
  - `chestDrop`: Apply only when items come from chests
  - `groundDrop`: Apply only when items are generated on the ground
  - `questReward`: Apply only for quest rewards

- `modType`: How the drop rate should be modified
  - `multiplier`: Multiply the base drop rate by the value
  - `addFlat`: Add the value to the base drop rate
  - `setAbsolute`: Set the drop rate to the value

- `value`: The value to use for modification

- `priority`: Priority of this modifier (higher values are applied first)

- `condition`: Conditions for when this modifier applies
  - `minMonsterLevel`: Minimum monster level for this modifier to apply
  - `maxMonsterLevel`: Maximum monster level for this modifier to apply
  - `minDungeonLevel`: Minimum dungeon level for this modifier to apply
  - `maxDungeonLevel`: Maximum dungeon level for this modifier to apply
  - `difficulty`: Game difficulty (0=Normal, 1=Nightmare, 2=Hell)
  - `multiplayerOnly`: Whether this modifier only applies in multiplayer games

## Example Configuration

Here's an example configuration that demonstrates various features of the system:

```json
{
  "maxDropRateMultiplier": 10.0,
  "minDropRateMultiplier": 0.1,
  "modifiers": [
    {
      "scope": "global",
      "modType": "multiplier",
      "value": 1.2,
      "context": "always",
      "priority": 0,
      "description": "Global 20% increase to all item drop rates"
    },
    {
      "scope": "uniqueItem",
      "modType": "multiplier",
      "value": 1.5,
      "context": "monsterDrop",
      "priority": 10,
      "description": "50% increase to unique item drops from monsters"
    },
    {
      "scope": "itemClass",
      "targetId": "Weapon",
      "modType": "multiplier",
      "value": 1.3,
      "context": "monsterDrop",
      "priority": 5,
      "description": "30% increase to weapon drops from monsters"
    },
    {
      "scope": "global",
      "modType": "multiplier",
      "value": 2.0,
      "context": "always",
      "priority": 20,
      "condition": {
        "difficulty": 2
      },
      "description": "Double all drop rates in Hell difficulty"
    }
  ]
}
```

## How It Works

1. When an item is being considered for dropping, the system checks all applicable modifiers.
2. Modifiers are applied in order of priority (highest first).
3. For each modifier, the system checks if it applies to the current item and context.
4. If the modifier applies, it adjusts the drop rate according to the modifier's type and value.
5. After all modifiers are applied, the system ensures the final drop rate is within the bounds set by the safeguards.

## Balance Considerations

The system includes safeguards to prevent extreme modifications that could break game balance:

- `maxDropRateMultiplier`: Prevents drop rates from being increased too much
- `minDropRateMultiplier`: Prevents drop rates from being decreased too much
- Drop rates cannot be negative

## Technical Implementation

The system is implemented in the following files:

- `Source/mods/config/drop_rate_config.h/cpp`: Configuration schema and loading
- `Source/mods/drop_rate_modifier.h/cpp`: Interface for the item system
- `Source/mods/mod_init.h/cpp`: Initialization of the modding framework

The system integrates with the existing item drop system by modifying the `GetItemIndexForDroppableItem` function to apply the drop rate modifiers.

## Future Enhancements

Planned future enhancements for the system include:

1. Support for item ID-specific modifiers
2. In-game UI for adjusting drop rates
3. More granular control over unique item drop rates
4. Support for modifying item quality distributions
5. Integration with other modding systems (e.g., monster mods, dungeon mods)

## Troubleshooting

If you encounter issues with the Item Drop Rate Modification System:

1. Check that your configuration file is valid JSON
2. Ensure that all required fields are present for each modifier
3. Verify that the `targetId` values match the expected values in the game
4. Check the game logs for any error messages related to the modding framework

If you need to reset to default values, simply delete the configuration file and restart the game.
