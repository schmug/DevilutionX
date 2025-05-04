/**
 * @file affix_score.h
 *
 * Interface for the affix scoring system that evaluates magical properties on items.
 */
#pragma once

#include <cstdint>
#include <array>
#include <string_view>

#include "items.h"
#include "gear/gear_score.h"

namespace devilution {

/**
 * @brief Categories of affixes for scoring purposes
 */
enum class AffixCategory : uint8_t {
    Offensive,      // Damage, attack rating, etc.
    Defensive,      // Armor, resistances, etc.
    Attribute,      // Strength, dexterity, etc.
    Utility,        // Life, mana, etc.
    Special,        // Unique or special properties
    
    FIRST = Offensive,
    LAST = Special
};

/**
 * @brief Structure to define an affix and its scoring parameters
 */
struct AffixDefinition {
    std::string_view name;        // Name of the affix
    AffixCategory category;       // Category of the affix
    float baseWeight;             // Base weight/importance of the affix
    float valueMultiplier;        // Multiplier to convert raw value to score
    
    // Category-specific multipliers
    float weaponMultiplier;       // Multiplier when on weapons
    float armorMultiplier;        // Multiplier when on armor
    float shieldMultiplier;       // Multiplier when on shields
    float helmMultiplier;         // Multiplier when on helms
    float jewelryMultiplier;      // Multiplier when on jewelry
};

/**
 * @brief Defines the scoring parameters for each affix type
 */
extern const std::array<AffixDefinition, 15> AffixDefinitions;

/**
 * @brief Calculates the score contribution from a specific affix
 * @param item The item containing the affix
 * @param affixValue The numerical value of the affix
 * @param affixIndex The index of the affix in AffixDefinitions
 * @return The score contribution from this affix
 */
float CalculateAffixScore(const Item &item, int affixValue, size_t affixIndex);

/**
 * @brief Calculates the total score contribution from all affixes on an item
 * @param item The item to evaluate
 * @return The total score from all affixes
 */
float CalculateItemAffixScore(const Item &item);

/**
 * @brief Gets a text description of the affix category
 * @param category The affix category to describe
 * @return A text description of the category
 */
std::string_view GetAffixCategoryName(AffixCategory category);

} // namespace devilution
