/**
 * @file gear_score.cpp
 *
 * Implementation of the gear scoring system that evaluates equipment quality and calculates player gear level.
 */
#include "gear/gear_score.h"

#include <algorithm>
#include <cmath>

#include "utils/log.hpp"
#include "gear/affix_score.h"
#include "gear/composite_score.h"
#include "gear/gear_config.h"

namespace devilution {

GearQuality GetGearQuality(item_quality quality)
{
    switch (quality) {
    case ITEM_QUALITY_NORMAL:
        return GearQuality::Normal;
    case ITEM_QUALITY_MAGIC:
        return GearQuality::Magic;
    case ITEM_QUALITY_UNIQUE:
        return GearQuality::Unique;
    default:
        return GearQuality::Normal;
    }
}

GearCategory GetGearCategory(const Item &item)
{
    if (item.isEmpty()) {
        return GearCategory::Other;
    }
    
    if (item.isWeapon()) {
        return GearCategory::Weapon;
    }
    
    if (item.isArmor()) {
        if (item.isHelm()) {
            return GearCategory::Helm;
        }
        if (item.isShield()) {
            return GearCategory::Shield;
        }
        return GearCategory::Armor;
    }
    
    if (item.isJewelry()) {
        return GearCategory::Jewelry;
    }
    
    return GearCategory::Other;
}

float CalculateItemBaseScore(const Item &item)
{
    if (item.isEmpty()) {
        return 0.0f;
    }
    
    // Get quality and category
    GearQuality quality = GetGearQuality(item._iMagical);
    GearCategory category = GetGearCategory(item);
    
    // Get configuration
    const GearScoringConfig &config = GearConfigLoader::GetConfig();
    
    // Calculate base score from quality
    float baseScore = 0.0f;
    switch (quality) {
    case GearQuality::Normal:
        baseScore = config.qualityScores.normalScore;
        break;
    case GearQuality::Magic:
        baseScore = config.qualityScores.magicScore;
        break;
    case GearQuality::Rare:
        baseScore = config.qualityScores.rareScore;
        break;
    case GearQuality::Unique:
        baseScore = config.qualityScores.uniqueScore;
        break;
    case GearQuality::Set:
        baseScore = config.qualityScores.setScore;
        break;
    case GearQuality::Crafted:
        baseScore = config.qualityScores.craftedScore;
        break;
    }
    
    // Apply category multiplier
    switch (category) {
    case GearCategory::Weapon:
        baseScore *= config.categoryMultipliers.weaponMultiplier;
        break;
    case GearCategory::Armor:
        baseScore *= config.categoryMultipliers.armorMultiplier;
        break;
    case GearCategory::Shield:
        baseScore *= config.categoryMultipliers.shieldMultiplier;
        break;
    case GearCategory::Helm:
        baseScore *= config.categoryMultipliers.helmMultiplier;
        break;
    case GearCategory::Jewelry:
        baseScore *= config.categoryMultipliers.jewelryMultiplier;
        break;
    case GearCategory::Other:
        baseScore *= config.categoryMultipliers.otherMultiplier;
        break;
    }
    
    // Factor in item level
    float levelFactor = std::min(config.levelFactorBase + (item._iILvl * config.levelFactorMultiplier), config.levelFactorMax);
    baseScore *= levelFactor;
    
    return baseScore;
}

float CalculateItemAffixScore(const Item &item)
{
    // Use the specialized affix scoring system from affix_score.cpp
    return devilution::CalculateItemAffixScore(item);
}

float CalculateItemScore(const Item &item)
{
    // Use the composite scoring system
    return GearScorer::CalculateItemScore(item);
}

float CalculateGearLevel(const Player &player)
{
    // Use the composite scoring system
    return GearScorer::CalculateGearLevel(player);
}

std::string_view GetGearQualityName(GearQuality quality)
{
    switch (quality) {
    case GearQuality::Normal:
        return "Normal";
    case GearQuality::Magic:
        return "Magic";
    case GearQuality::Rare:
        return "Rare";
    case GearQuality::Unique:
        return "Unique";
    case GearQuality::Set:
        return "Set";
    case GearQuality::Crafted:
        return "Crafted";
    default:
        return "Unknown";
    }
}

std::string_view GetGearCategoryName(GearCategory category)
{
    switch (category) {
    case GearCategory::Weapon:
        return "Weapon";
    case GearCategory::Armor:
        return "Armor";
    case GearCategory::Shield:
        return "Shield";
    case GearCategory::Helm:
        return "Helm";
    case GearCategory::Jewelry:
        return "Jewelry";
    case GearCategory::Other:
        return "Other";
    default:
        return "Unknown";
    }
}

} // namespace devilution
