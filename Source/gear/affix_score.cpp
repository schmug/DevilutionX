/**
 * @file affix_score.cpp
 *
 * Implementation of the affix scoring system that evaluates magical properties on items.
 */
#include "gear/affix_score.h"

#include <algorithm>
#include <cmath>

#include "utils/log.hpp"

namespace devilution {

// Define the scoring parameters for each affix type
const std::array<AffixDefinition, 15> AffixDefinitions = {{
    // Name                 Category              BaseWeight  ValueMult  Weapon  Armor   Shield  Helm    Jewelry
    { "Strength",           AffixCategory::Attribute,  0.5f,    1.0f,     1.2f,   1.1f,   1.0f,   1.0f,   0.9f   },
    { "Magic",              AffixCategory::Attribute,  0.5f,    1.0f,     0.9f,   0.9f,   0.9f,   1.0f,   1.3f   },
    { "Dexterity",          AffixCategory::Attribute,  0.5f,    1.0f,     1.3f,   0.9f,   1.0f,   1.0f,   1.0f   },
    { "Vitality",           AffixCategory::Attribute,  0.5f,    1.0f,     0.8f,   1.2f,   1.2f,   1.1f,   0.9f   },
    { "All Attributes",     AffixCategory::Attribute,  0.7f,    1.0f,     1.1f,   1.1f,   1.1f,   1.1f,   1.2f   },
    { "Damage",             AffixCategory::Offensive,  0.6f,    0.3f,     1.5f,   0.7f,   0.8f,   0.7f,   1.0f   },
    { "To-Hit",             AffixCategory::Offensive,  0.4f,    0.05f,    1.4f,   0.6f,   0.7f,   0.7f,   1.0f   },
    { "Armor Class",        AffixCategory::Defensive,  0.5f,    0.2f,     0.5f,   1.4f,   1.3f,   1.2f,   0.7f   },
    { "Fire Resist",        AffixCategory::Defensive,  0.4f,    0.1f,     0.7f,   1.3f,   1.3f,   1.2f,   1.1f   },
    { "Lightning Resist",   AffixCategory::Defensive,  0.4f,    0.1f,     0.7f,   1.3f,   1.3f,   1.2f,   1.1f   },
    { "Magic Resist",       AffixCategory::Defensive,  0.4f,    0.1f,     0.7f,   1.3f,   1.3f,   1.2f,   1.1f   },
    { "All Resist",         AffixCategory::Defensive,  0.6f,    0.15f,    0.8f,   1.4f,   1.4f,   1.3f,   1.2f   },
    { "Life",               AffixCategory::Utility,    0.4f,    0.1f,     0.9f,   1.1f,   1.1f,   1.0f,   1.0f   },
    { "Mana",               AffixCategory::Utility,    0.4f,    0.1f,     0.9f,   0.9f,   0.9f,   1.0f,   1.2f   },
    { "Durability",         AffixCategory::Utility,    0.2f,    0.01f,    1.0f,   1.0f,   1.0f,   1.0f,   0.0f   }
}};

float CalculateAffixScore(const Item &item, int affixValue, size_t affixIndex)
{
    if (affixValue == 0 || affixIndex >= AffixDefinitions.size()) {
        return 0.0f;
    }
    
    const AffixDefinition &affix = AffixDefinitions[affixIndex];
    GearCategory category = GetGearCategory(item);
    
    // Start with base score calculation
    float score = static_cast<float>(std::abs(affixValue)) * affix.valueMultiplier * affix.baseWeight;
    
    // Apply category-specific multiplier
    switch (category) {
    case GearCategory::Weapon:
        score *= affix.weaponMultiplier;
        break;
    case GearCategory::Armor:
        score *= affix.armorMultiplier;
        break;
    case GearCategory::Shield:
        score *= affix.shieldMultiplier;
        break;
    case GearCategory::Helm:
        score *= affix.helmMultiplier;
        break;
    case GearCategory::Jewelry:
        score *= affix.jewelryMultiplier;
        break;
    default:
        // No additional multiplier for other categories
        break;
    }
    
    return score;
}

float CalculateItemAffixScore(const Item &item)
{
    if (item.isEmpty() || item._iMagical == ITEM_QUALITY_NORMAL) {
        return 0.0f;
    }
    
    float totalScore = 0.0f;
    
    // Strength bonus
    totalScore += CalculateAffixScore(item, item._iPLStr, 0);
    
    // Magic bonus
    totalScore += CalculateAffixScore(item, item._iPLMag, 1);
    
    // Dexterity bonus
    totalScore += CalculateAffixScore(item, item._iPLDex, 2);
    
    // Vitality bonus
    totalScore += CalculateAffixScore(item, item._iPLVit, 3);
    
    // All attributes (if item has bonuses to multiple attributes)
    if (item._iPLStr > 0 && item._iPLMag > 0 && item._iPLDex > 0 && item._iPLVit > 0) {
        int allAttrValue = std::min({item._iPLStr, item._iPLMag, item._iPLDex, item._iPLVit});
        totalScore += CalculateAffixScore(item, allAttrValue, 4);
    }
    
    // Damage bonuses
    if (item._iMaxDam - item._iMinDam > 0) {
        int avgDamage = (item._iMaxDam + item._iMinDam) / 2;
        totalScore += CalculateAffixScore(item, avgDamage, 5);
    }
    
    // To-hit bonus
    if (item._iPLToHit > 0) {
        totalScore += CalculateAffixScore(item, item._iPLToHit, 6);
    }
    
    // Armor Class
    if (item._iAC > 0) {
        totalScore += CalculateAffixScore(item, item._iAC, 7);
    }
    
    // Fire Resistance
    totalScore += CalculateAffixScore(item, item._iPLFR, 8);
    
    // Lightning Resistance
    totalScore += CalculateAffixScore(item, item._iPLLR, 9);
    
    // Magic Resistance
    totalScore += CalculateAffixScore(item, item._iPLMR, 10);
    
    // All Resistances (if item has bonuses to multiple resistances)
    if (item._iPLFR > 0 && item._iPLLR > 0 && item._iPLMR > 0) {
        int allResistValue = std::min({item._iPLFR, item._iPLLR, item._iPLMR});
        totalScore += CalculateAffixScore(item, allResistValue, 11);
    }
    
    // Life bonus
    totalScore += CalculateAffixScore(item, item._iPLHP, 12);
    
    // Mana bonus
    totalScore += CalculateAffixScore(item, item._iPLMana, 13);
    
    // Durability bonus (only if significantly durable)
    if (item._iDurability > item._iMaxDur / 2) {
        totalScore += CalculateAffixScore(item, item._iDurability, 14);
    }
    
    // Special case for unique items - add a bonus for their special properties
    if (item._iMagical == ITEM_QUALITY_UNIQUE) {
        totalScore *= 1.2f; // 20% bonus for unique items
    }
    
    LogVerbose("Item '{}' affix score: {:.2f}", item._iIName, totalScore);
    
    return totalScore;
}

std::string_view GetAffixCategoryName(AffixCategory category)
{
    switch (category) {
    case AffixCategory::Offensive:
        return "Offensive";
    case AffixCategory::Defensive:
        return "Defensive";
    case AffixCategory::Attribute:
        return "Attribute";
    case AffixCategory::Utility:
        return "Utility";
    case AffixCategory::Special:
        return "Special";
    default:
        return "Unknown";
    }
}

} // namespace devilution
