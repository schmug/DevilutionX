/**
 * @file gear_score.h
 *
 * Interface for the gear scoring system that evaluates equipment quality and calculates player gear level.
 */
#pragma once

#include <cstdint>
#include <array>
#include <string_view>

#include "items.h"
#include "player.h"

namespace devilution {

/**
 * @brief Represents the quality tiers for items in the gear scoring system
 */
enum class GearQuality : uint8_t {
    Normal,     // White/common items
    Magic,      // Blue/magic items
    Rare,       // Yellow/rare items
    Unique,     // Gold/unique items
    Set,        // Green/set items
    Crafted,    // Orange/crafted items
    
    FIRST = Normal,
    LAST = Crafted
};

/**
 * @brief Represents the major item categories for specialized scoring
 */
enum class GearCategory : uint8_t {
    Weapon,     // All weapons
    Armor,      // Body armor
    Shield,     // Shields
    Helm,       // Helmets
    Jewelry,    // Rings and amulets
    Other,      // Miscellaneous items
    
    FIRST = Weapon,
    LAST = Other
};

/**
 * @brief Base scores for each quality tier
 */
constexpr std::array<float, 6> QualityBaseScores = {
    1.0f,   // Normal
    2.0f,   // Magic
    3.5f,   // Rare
    5.0f,   // Unique
    4.5f,   // Set
    4.0f    // Crafted
};

/**
 * @brief Multipliers for each gear category
 */
constexpr std::array<float, 6> CategoryMultipliers = {
    1.5f,   // Weapon (highest impact)
    1.2f,   // Armor
    1.0f,   // Shield
    0.8f,   // Helm
    1.0f,   // Jewelry
    0.5f    // Other
};

/**
 * @brief Slot importance weights for calculating overall gear score
 */
constexpr std::array<float, NUM_INVLOC> SlotImportance = {
    0.8f,   // INVLOC_HEAD
    0.6f,   // INVLOC_RING_LEFT
    0.6f,   // INVLOC_RING_RIGHT
    0.7f,   // INVLOC_AMULET
    1.5f,   // INVLOC_HAND_LEFT (weapon)
    1.0f,   // INVLOC_HAND_RIGHT (shield/weapon)
    1.3f    // INVLOC_CHEST (armor)
};

/**
 * @brief Converts an item quality to a GearQuality enum
 * @param quality The item_quality to convert
 * @return The corresponding GearQuality
 */
GearQuality GetGearQuality(item_quality quality);

/**
 * @brief Determines the gear category for an item
 * @param item The item to categorize
 * @return The corresponding GearCategory
 */
GearCategory GetGearCategory(const Item &item);

/**
 * @brief Calculates the base score for an item based on its quality and category
 * @param item The item to score
 * @return The base score value
 */
float CalculateItemBaseScore(const Item &item);

/**
 * @brief Calculates the affix score contribution for an item
 * @param item The item to evaluate
 * @return The score from the item's affixes
 */
float CalculateItemAffixScore(const Item &item);

/**
 * @brief Calculates the total score for a single item
 * @param item The item to score
 * @return The total item score
 */
float CalculateItemScore(const Item &item);

/**
 * @brief Calculates the overall gear level for a player
 * @param player The player to evaluate
 * @return The calculated gear level
 */
float CalculateGearLevel(const Player &player);

/**
 * @brief Gets a text description of the gear quality
 * @param quality The gear quality to describe
 * @return A text description of the quality
 */
std::string_view GetGearQualityName(GearQuality quality);

/**
 * @brief Gets a text description of the gear category
 * @param category The gear category to describe
 * @return A text description of the category
 */
std::string_view GetGearCategoryName(GearCategory category);

} // namespace devilution
