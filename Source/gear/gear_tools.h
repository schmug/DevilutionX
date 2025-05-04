/**
 * @file gear_tools.h
 *
 * Interface for gear scoring tools and utilities.
 */
#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "items.h"
#include "player.h"
#include "gear/composite_score.h"

namespace devilution {

/**
 * @brief Structure to store item comparison results
 */
struct ItemComparisonResult {
    const Item *item1;
    const Item *item2;
    ItemScoreBreakdown score1;
    ItemScoreBreakdown score2;
    float scoreDifference;
    float scorePercentChange;
    std::string comparisonText;
};

/**
 * @brief Class for comparing items and visualizing gear scores
 */
class GearTools {
public:
    /**
     * @brief Compares two items and generates a detailed comparison
     * @param item1 First item to compare
     * @param item2 Second item to compare
     * @return Detailed comparison result
     */
    static ItemComparisonResult CompareItems(const Item &item1, const Item &item2);
    
    /**
     * @brief Finds the best item for a specific slot from a player's inventory
     * @param player The player to check
     * @param slot The inventory slot to find the best item for
     * @return Pointer to the best item, or nullptr if no valid items found
     */
    static const Item* FindBestItemForSlot(const Player &player, inv_body_loc slot);
    
    /**
     * @brief Suggests gear upgrades for a player
     * @param player The player to suggest upgrades for
     * @return Text with upgrade suggestions
     */
    static std::string SuggestGearUpgrades(const Player &player);
    
    /**
     * @brief Generates a histogram of item scores for balancing analysis
     * @param player The player to analyze
     * @return Text representation of the score distribution
     */
    static std::string GenerateScoreDistribution(const Player &player);
    
    /**
     * @brief Simulates the impact of an item upgrade on the player's gear level
     * @param player The player to simulate for
     * @param newItem The potential new item
     * @param slot The slot to equip the new item in
     * @return Text describing the impact on gear level
     */
    static std::string SimulateItemUpgrade(const Player &player, const Item &newItem, inv_body_loc slot);
    
    /**
     * @brief Generates a benchmark report for a set of standard items
     * @return Text report comparing current scores to expected benchmarks
     */
    static std::string GenerateBenchmarkReport();
};

} // namespace devilution
