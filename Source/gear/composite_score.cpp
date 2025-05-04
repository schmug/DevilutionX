/**
 * @file composite_score.cpp
 *
 * Implementation of the composite gear scoring system that combines all scoring components.
 */
#include "gear/composite_score.h"

#include <algorithm>
#include <cmath>
#include <sstream>

#include "utils/log.hpp"
#include "gear/gear_config.h"
#include "gear/specialized_score.h"

namespace devilution {

ItemScoreBreakdown GearScorer::CalculateDetailedItemScore(const Item &item)
{
    if (item.isEmpty()) {
        return ItemScoreBreakdown();
    }
    
    ItemScoreBreakdown breakdown;
    
    // Calculate base score from quality and category
    breakdown.baseScore = CalculateItemBaseScore(item);
    
    // Calculate score from affixes
    breakdown.affixScore = CalculateItemAffixScore(item);
    
    // Get configuration
    const GearScoringConfig &config = GearConfigLoader::GetConfig();
    
    // Calculate level factor
    breakdown.levelFactor = std::min(config.levelFactorBase + (item._iILvl * config.levelFactorMultiplier), config.levelFactorMax);
    
    // Calculate total score using base formula
    // Formula: (base_score + affix_score) * level_factor
    breakdown.totalScore = (breakdown.baseScore + breakdown.affixScore) * breakdown.levelFactor;
    
    // Apply any special bonuses for unique or set items
    if (item._iMagical == ITEM_QUALITY_UNIQUE) {
        // Unique items get a bonus
        breakdown.totalScore *= config.uniqueItemBonus;
    }
    
    // Apply specialized scoring based on item category
    std::unique_ptr<ItemScorer> specializedScorer = ItemScorerFactory::CreateScorer(item);
    if (specializedScorer) {
        float specializedScore = specializedScorer->CalculateScore(item);
        
        // Blend the specialized score with the base score (60% specialized, 40% base)
        breakdown.totalScore = (breakdown.totalScore * 0.4f) + (specializedScore * 0.6f);
        
        LogVerbose("Item '{}' specialized score: {:.2f}", item._iIName, specializedScore);
    }
    
    LogVerbose("Item '{}' score breakdown: base={:.2f}, affix={:.2f}, level_factor={:.2f}, total={:.2f}",
        item._iIName, breakdown.baseScore, breakdown.affixScore, breakdown.levelFactor, breakdown.totalScore);
    
    return breakdown;
}

float GearScorer::CalculateItemScore(const Item &item)
{
    return CalculateDetailedItemScore(item).totalScore;
}

float GearScorer::CalculateGearLevel(const Player &player)
{
    float totalWeightedScore = 0.0f;
    float totalWeight = 0.0f;
    
    // Calculate score for each equipped item
    for (int i = 0; i < NUM_INVLOC; i++) {
        const Item &item = player.InvBody[i];
        if (!item.isEmpty()) {
            float itemScore = CalculateItemScore(item);
            float slotWeight = SlotImportance[i];
            
            totalWeightedScore += itemScore * slotWeight;
            totalWeight += slotWeight;
        }
    }
    
    // Calculate weighted average
    float gearScore = (totalWeight > 0) ? (totalWeightedScore / totalWeight) : 0.0f;
    
    // Get configuration
    const GearScoringConfig &config = GearConfigLoader::GetConfig();
    
    // Factor in character level with diminishing returns
    float levelFactor = std::sqrt(static_cast<float>(player._pLevel)) * config.characterLevelWeight;
    
    // Calculate final gear level
    float gearLevel = (gearScore * config.gearScoreWeight) + levelFactor;
    
    // Normalize to the configured range
    gearLevel = NormalizeScore(gearLevel, config.minGearLevel, config.maxGearLevel);
    
    LogVerbose("Player '{}' gear level: {:.2f} (gear score: {:.2f}, level factor: {:.2f})",
        player._pName, gearLevel, gearScore, levelFactor);
    
    return gearLevel;
}

std::string GearScorer::GetScoreExplanation(const Item &item)
{
    if (item.isEmpty()) {
        return "Empty item slot";
    }
    
    ItemScoreBreakdown breakdown = CalculateDetailedItemScore(item);
    
    std::stringstream explanation;
    explanation << "Score breakdown for " << item._iIName << ":\n";
    explanation << "- Base score: " << breakdown.baseScore << " (Quality: " << GetGearQualityName(GetGearQuality(item._iMagical)) 
                << ", Category: " << GetGearCategoryName(GetGearCategory(item)) << ")\n";
    explanation << "- Affix score: " << breakdown.affixScore << "\n";
    explanation << "- Level factor: " << breakdown.levelFactor << " (Item level: " << item._iILvl << ")\n";
    
    // Get configuration
    const GearScoringConfig &config = GearConfigLoader::GetConfig();
    
    if (item._iMagical == ITEM_QUALITY_UNIQUE) {
        explanation << "- Unique bonus: " << (config.uniqueItemBonus - 1.0f) * 100.0f << "%\n";
    }
    
    // Add specialized scoring explanation
    std::unique_ptr<ItemScorer> specializedScorer = ItemScorerFactory::CreateScorer(item);
    if (specializedScorer) {
        explanation << "\nSpecialized Scoring:\n";
        explanation << specializedScorer->GetScoreExplanation(item) << "\n";
        explanation << "\nFinal Score (40% base, 60% specialized)\n";
    }
    
    explanation << "- Total score: " << breakdown.totalScore;
    
    return explanation.str();
}

float GearScorer::NormalizeScore(float score, float minValue, float maxValue)
{
    // Get configuration
    const GearScoringConfig &config = GearConfigLoader::GetConfig();
    
    // Define the expected range of raw scores from config
    const float rawMin = config.rawScoreMin;
    const float rawMax = config.rawScoreMax;
    
    // Clamp the score to the expected range
    float clampedScore = std::max(rawMin, std::min(score, rawMax));
    
    // Normalize to the desired range
    float normalizedScore = minValue + ((clampedScore - rawMin) / (rawMax - rawMin)) * (maxValue - minValue);
    
    return normalizedScore;
}

} // namespace devilution
