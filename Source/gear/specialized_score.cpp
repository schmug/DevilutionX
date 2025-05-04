/**
 * @file specialized_score.cpp
 *
 * Implementation of specialized item category scoring systems.
 */
#include "gear/specialized_score.h"

#include <algorithm>
#include <cmath>
#include <sstream>

#include "utils/log.hpp"
#include "gear/gear_config.h"

namespace devilution {

// ===== WeaponScorer Implementation =====

float WeaponScorer::CalculateScore(const Item &item) const
{
    if (item.isEmpty() || !item.isWeapon()) {
        return 0.0f;
    }
    
    float score = 0.0f;
    
    // Calculate DPS contribution
    float dpsScore = CalculateDPS(item);
    score += dpsScore;
    
    // Calculate elemental damage contribution
    float elementalScore = CalculateElementalDamageValue(item);
    score += elementalScore;
    
    // Calculate critical hit contribution
    float critScore = CalculateCriticalHitValue(item);
    score += critScore;
    
    // Apply weapon-specific multipliers based on type
    if (item._itype == ItemType::Sword) {
        score *= 1.05f; // Swords are slightly favored
    } else if (item._itype == ItemType::Axe) {
        score *= 1.1f; // Axes get a bonus
    } else if (item._itype == ItemType::Mace) {
        score *= 1.08f; // Maces get a bonus
    } else if (item._itype == ItemType::Bow) {
        score *= 1.12f; // Bows get a significant bonus
    } else if (item._itype == ItemType::Staff) {
        score *= 1.15f; // Staves get the highest bonus
    }
    
    LogVerbose("Weapon '{}' specialized score: {:.2f} (DPS: {:.2f}, Elemental: {:.2f}, Crit: {:.2f})",
        item._iIName, score, dpsScore, elementalScore, critScore);
    
    return score;
}

float WeaponScorer::CalculateDPS(const Item &item) const
{
    // Calculate average damage
    float avgDamage = (item._iMinDam + item._iMaxDam) / 2.0f;
    
    // Factor in attack speed (approximated)
    float attackSpeed = 1.0f; // Default attack speed
    if (item._iClass == ICLASS_WEAPON) {
        // Adjust based on weapon class
        if (item._itype == ItemType::Sword) {
            attackSpeed = 1.2f; // Swords are faster
        } else if (item._itype == ItemType::Axe) {
            attackSpeed = 0.9f; // Axes are slower
        } else if (item._itype == ItemType::Mace) {
            attackSpeed = 0.8f; // Maces are slower
        } else if (item._itype == ItemType::Bow) {
            attackSpeed = 1.1f; // Bows are faster
        } else if (item._itype == ItemType::Staff) {
            attackSpeed = 0.7f; // Staves are slower
        }
    }
    
    // Calculate DPS
    float dps = avgDamage * attackSpeed;
    
    // Apply a scaling factor to convert DPS to score points
    return dps * 0.5f;
}

float WeaponScorer::CalculateElementalDamageValue(const Item &item) const
{
    float elementalScore = 0.0f;
    
    // Check for fire damage
    if (item._iFlags & ISPL_FIREDAM) {
        elementalScore += 5.0f;
    }
    
    // Check for lightning damage
    if (item._iFlags & ISPL_LIGHTDAM) {
        elementalScore += 5.0f;
    }
    
    // Check for magic damage (approximated)
    if (item._iFlags & ISPL_ALLRUNES) {
        elementalScore += 5.0f;
    }
    
    return elementalScore;
}

float WeaponScorer::CalculateCriticalHitValue(const Item &item) const
{
    float critScore = 0.0f;
    
    // Check for to-hit bonus which can increase crit chance
    if (item._iPLToHit > 0) {
        critScore += item._iPLToHit * 0.1f;
    }
    
    // Check for dexterity bonus which can increase crit chance
    if (item._iPLDex > 0) {
        critScore += item._iPLDex * 0.2f;
    }
    
    return critScore;
}

std::string WeaponScorer::GetScoreExplanation(const Item &item) const
{
    if (item.isEmpty() || !item.isWeapon()) {
        return "Not a weapon";
    }
    
    float dpsScore = CalculateDPS(item);
    float elementalScore = CalculateElementalDamageValue(item);
    float critScore = CalculateCriticalHitValue(item);
    
    std::stringstream explanation;
    explanation << "Weapon Scoring for " << item._iIName << ":\n";
    explanation << "- DPS Score: " << dpsScore << " (Damage: " << item._iMinDam << "-" << item._iMaxDam << ")\n";
    explanation << "- Elemental Damage: " << elementalScore << "\n";
    explanation << "- Critical Hit Value: " << critScore << "\n";
    
    // Add weapon type bonus explanation
    if (item._itype == ItemType::Sword) {
        explanation << "- Sword Bonus: 5%\n";
    } else if (item._itype == ItemType::Axe) {
        explanation << "- Axe Bonus: 10%\n";
    } else if (item._itype == ItemType::Mace) {
        explanation << "- Mace Bonus: 8%\n";
    } else if (item._itype == ItemType::Bow) {
        explanation << "- Bow Bonus: 12%\n";
    } else if (item._itype == ItemType::Staff) {
        explanation << "- Staff Bonus: 15%\n";
    }
    
    explanation << "- Total Weapon Score: " << CalculateScore(item);
    
    return explanation.str();
}

// ===== ArmorScorer Implementation =====

float ArmorScorer::CalculateScore(const Item &item) const
{
    if (item.isEmpty() || !item.isArmor()) {
        return 0.0f;
    }
    
    float score = 0.0f;
    
    // Calculate damage reduction contribution
    float damageReductionScore = CalculateDamageReductionValue(item);
    score += damageReductionScore;
    
    // Calculate resistances contribution
    float resistancesScore = CalculateResistancesValue(item);
    score += resistancesScore;
    
    // Calculate defensive stats contribution
    float defensiveStatsScore = CalculateDefensiveStatsValue(item);
    score += defensiveStatsScore;
    
    // Apply armor-specific multipliers based on type
    if (item.isHelm()) {
        score *= 0.9f; // Helms are slightly less valuable
    } else if (item.isShield()) {
        score *= 1.1f; // Shields get a bonus
    } else {
        // Body armor
        score *= 1.2f; // Body armor gets a significant bonus
    }
    
    LogVerbose("Armor '{}' specialized score: {:.2f} (Reduction: {:.2f}, Resistances: {:.2f}, Defensive: {:.2f})",
        item._iIName, score, damageReductionScore, resistancesScore, defensiveStatsScore);
    
    return score;
}

float ArmorScorer::CalculateDamageReductionValue(const Item &item) const
{
    // Armor class directly contributes to damage reduction
    float acScore = item._iAC * 0.5f;
    
    // Durability affects how long the armor will last
    float durabilityFactor = std::min(1.0f + (item._iDurability / 100.0f), 2.0f);
    acScore *= durabilityFactor;
    
    return acScore;
}

float ArmorScorer::CalculateResistancesValue(const Item &item) const
{
    float resistanceScore = 0.0f;
    
    // Fire resistance
    if (item._iPLFR > 0) {
        resistanceScore += item._iPLFR * 0.2f;
    }
    
    // Lightning resistance
    if (item._iPLLR > 0) {
        resistanceScore += item._iPLLR * 0.2f;
    }
    
    // Magic resistance
    if (item._iPLMR > 0) {
        resistanceScore += item._iPLMR * 0.2f;
    }
    
    // All resistances are more valuable together (synergy bonus)
    if (item._iPLFR > 0 && item._iPLLR > 0 && item._iPLMR > 0) {
        int minResist = std::min({item._iPLFR, item._iPLLR, item._iPLMR});
        resistanceScore += minResist * 0.3f; // Bonus for having all resistances
    }
    
    return resistanceScore;
}

float ArmorScorer::CalculateDefensiveStatsValue(const Item &item) const
{
    float defensiveScore = 0.0f;
    
    // Vitality bonus
    if (item._iPLVit > 0) {
        defensiveScore += item._iPLVit * 0.5f;
    }
    
    // Life bonus
    if (item._iPLHP > 0) {
        defensiveScore += item._iPLHP * 0.1f;
    }
    
    // Damage reduction (if applicable)
    if (item._iPLDamMod < 0) { // Negative damage mod means damage reduction
        defensiveScore += std::abs(item._iPLDamMod) * 0.5f;
    }
    
    return defensiveScore;
}

std::string ArmorScorer::GetScoreExplanation(const Item &item) const
{
    if (item.isEmpty() || !item.isArmor()) {
        return "Not armor";
    }
    
    float damageReductionScore = CalculateDamageReductionValue(item);
    float resistancesScore = CalculateResistancesValue(item);
    float defensiveStatsScore = CalculateDefensiveStatsValue(item);
    
    std::stringstream explanation;
    explanation << "Armor Scoring for " << item._iIName << ":\n";
    explanation << "- Damage Reduction: " << damageReductionScore << " (AC: " << item._iAC << ")\n";
    explanation << "- Resistances: " << resistancesScore << " (Fire: " << item._iPLFR << ", Lightning: " << item._iPLLR << ", Magic: " << item._iPLMR << ")\n";
    explanation << "- Defensive Stats: " << defensiveStatsScore << "\n";
    
    // Add armor type bonus explanation
    if (item.isHelm()) {
        explanation << "- Helm Modifier: -10%\n";
    } else if (item.isShield()) {
        explanation << "- Shield Bonus: +10%\n";
    } else {
        explanation << "- Body Armor Bonus: +20%\n";
    }
    
    explanation << "- Total Armor Score: " << CalculateScore(item);
    
    return explanation.str();
}

// ===== JewelryScorer Implementation =====

float JewelryScorer::CalculateScore(const Item &item) const
{
    if (item.isEmpty() || !item.isJewelry()) {
        return 0.0f;
    }
    
    float score = 0.0f;
    
    // Calculate utility effects contribution
    float utilityScore = CalculateUtilityEffectsValue(item);
    score += utilityScore;
    
    // Calculate proc chances contribution
    float procScore = CalculateProcChancesValue(item);
    score += procScore;
    
    // Calculate unique bonuses contribution
    float uniqueBonusScore = CalculateUniqueBonusesValue(item);
    score += uniqueBonusScore;
    
    // Apply jewelry-specific multipliers based on type
    if (item._itype == ItemType::Ring) {
        score *= 1.0f; // Rings are baseline
    } else if (item._itype == ItemType::Amulet) {
        score *= 1.15f; // Amulets get a bonus
    }
    
    LogVerbose("Jewelry '{}' specialized score: {:.2f} (Utility: {:.2f}, Proc: {:.2f}, Unique: {:.2f})",
        item._iIName, score, utilityScore, procScore, uniqueBonusScore);
    
    return score;
}

float JewelryScorer::CalculateUtilityEffectsValue(const Item &item) const
{
    float utilityScore = 0.0f;
    
    // Attribute bonuses are valuable on jewelry
    if (item._iPLStr > 0) {
        utilityScore += item._iPLStr * 0.4f;
    }
    if (item._iPLMag > 0) {
        utilityScore += item._iPLMag * 0.5f; // Magic is especially valuable on jewelry
    }
    if (item._iPLDex > 0) {
        utilityScore += item._iPLDex * 0.4f;
    }
    if (item._iPLVit > 0) {
        utilityScore += item._iPLVit * 0.4f;
    }
    
    // Life and mana bonuses
    if (item._iPLHP > 0) {
        utilityScore += item._iPLHP * 0.1f;
    }
    if (item._iPLMana > 0) {
        utilityScore += item._iPLMana * 0.15f; // Mana is more valuable on jewelry
    }
    
    // All attributes bonus (if applicable)
    if (item._iPLStr > 0 && item._iPLMag > 0 && item._iPLDex > 0 && item._iPLVit > 0) {
        int minAttr = std::min({item._iPLStr, item._iPLMag, item._iPLDex, item._iPLVit});
        utilityScore += minAttr * 0.5f; // Bonus for having all attributes
    }
    
    return utilityScore;
}

float JewelryScorer::CalculateProcChancesValue(const Item &item) const
{
    float procScore = 0.0f;
    
    // Check for spell charges (approximated)
    if (item._iCharges > 0) {
        procScore += item._iCharges * 0.5f;
    }
    
    // Check for special flags that might indicate procs
    if (item._iFlags & ISPL_FIREDAM) {
        procScore += 3.0f; // Fire damage proc
    }
    if (item._iFlags & ISPL_LIGHTDAM) {
        procScore += 3.0f; // Lightning damage proc
    }
    if (item._iFlags & ISPL_THORNS) {
        procScore += 4.0f; // Thorns effect
    }
    
    return procScore;
}

float JewelryScorer::CalculateUniqueBonusesValue(const Item &item) const
{
    float uniqueScore = 0.0f;
    
    // Unique items get a bonus
    if (item._iMagical == ITEM_QUALITY_UNIQUE) {
        uniqueScore += 5.0f;
        
        // Check for specific unique jewelry bonuses
        if (item._iIName == "Ring of Truth") {
            uniqueScore += 3.0f; // Example unique bonus
        } else if (item._iIName == "Amulet of Warding") {
            uniqueScore += 4.0f; // Example unique bonus
        }
    }
    
    // Resistances are valuable on jewelry
    if (item._iPLFR > 0 || item._iPLLR > 0 || item._iPLMR > 0) {
        float resistSum = item._iPLFR + item._iPLLR + item._iPLMR;
        uniqueScore += resistSum * 0.15f;
    }
    
    return uniqueScore;
}

std::string JewelryScorer::GetScoreExplanation(const Item &item) const
{
    if (item.isEmpty() || !item.isJewelry()) {
        return "Not jewelry";
    }
    
    float utilityScore = CalculateUtilityEffectsValue(item);
    float procScore = CalculateProcChancesValue(item);
    float uniqueBonusScore = CalculateUniqueBonusesValue(item);
    
    std::stringstream explanation;
    explanation << "Jewelry Scoring for " << item._iIName << ":\n";
    explanation << "- Utility Effects: " << utilityScore << "\n";
    explanation << "- Proc Chances: " << procScore << "\n";
    explanation << "- Unique Bonuses: " << uniqueBonusScore << "\n";
    
    // Add jewelry type bonus explanation
    if (item._itype == ItemType::Ring) {
        explanation << "- Ring Modifier: None\n";
    } else if (item._itype == ItemType::Amulet) {
        explanation << "- Amulet Bonus: +15%\n";
    }
    
    explanation << "- Total Jewelry Score: " << CalculateScore(item);
    
    return explanation.str();
}

// ===== ItemScorerFactory Implementation =====

std::unique_ptr<ItemScorer> ItemScorerFactory::CreateScorer(const Item &item)
{
    GearCategory category = GetGearCategory(item);
    return CreateScorer(category);
}

std::unique_ptr<ItemScorer> ItemScorerFactory::CreateScorer(GearCategory category)
{
    switch (category) {
    case GearCategory::Weapon:
        return std::make_unique<WeaponScorer>();
    case GearCategory::Armor:
    case GearCategory::Shield:
    case GearCategory::Helm:
        return std::make_unique<ArmorScorer>();
    case GearCategory::Jewelry:
        return std::make_unique<JewelryScorer>();
    default:
        // Default to weapon scorer for unknown categories
        return std::make_unique<WeaponScorer>();
    }
}

} // namespace devilution
