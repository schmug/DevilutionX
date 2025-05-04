/**
 * @file composite_score.h
 *
 * Interface for the composite gear scoring system that combines all scoring components.
 */
#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "items.h"
#include "player.h"
#include "gear/gear_score.h"
#include "gear/affix_score.h"

namespace devilution {

/**
 * @brief Structure to store a detailed breakdown of an item's score
 */
struct ItemScoreBreakdown {
    float baseScore;        // Score from item quality and type
    float affixScore;       // Score from magical properties
    float levelFactor;      // Multiplier based on item level
    float totalScore;       // Final calculated score
    
    // Constructor with default values
    ItemScoreBreakdown()
        : baseScore(0.0f), affixScore(0.0f), levelFactor(0.0f), totalScore(0.0f)
    {
    }
};

/**
 * @brief Class that handles composite gear scoring
 */
class GearScorer {
public:
    /**
     * @brief Calculates a detailed score breakdown for an item
     * @param item The item to score
     * @return A detailed breakdown of the item's score
     */
    static ItemScoreBreakdown CalculateDetailedItemScore(const Item &item);
    
    /**
     * @brief Calculates the total score for an item
     * @param item The item to score
     * @return The total item score
     */
    static float CalculateItemScore(const Item &item);
    
    /**
     * @brief Calculates the overall gear level for a player
     * @param player The player to evaluate
     * @return The calculated gear level
     */
    static float CalculateGearLevel(const Player &player);
    
    /**
     * @brief Gets a text explanation of an item's score
     * @param item The item to explain
     * @return A text explanation of the score calculation
     */
    static std::string GetScoreExplanation(const Item &item);
    
    /**
     * @brief Normalizes a score to a specific range
     * @param score The raw score to normalize
     * @param minValue The minimum value in the normalized range
     * @param maxValue The maximum value in the normalized range
     * @return The normalized score
     */
    static float NormalizeScore(float score, float minValue = 1.0f, float maxValue = 100.0f);
};

} // namespace devilution
