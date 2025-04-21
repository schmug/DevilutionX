/**
 * @file drop_rate_test.h
 *
 * Testing utilities for the item drop rate modification system.
 */

#pragma once

#include <string>
#include <vector>
#include <map>

#include "itemdat.h"
#include "mods/config/drop_rate_config.h"

namespace devilution {

/**
 * @brief Test utility for the item drop rate modification system
 * 
 * This class provides methods to test and visualize the effects of
 * drop rate modifiers on item drop rates.
 */
class DropRateTest {
public:
    static DropRateTest& getInstance();
    
    /**
     * @brief Generate a report of how drop rate modifiers affect item drop rates
     * @param outputPath Path to write the report to
     * @return True if the report was successfully generated
     */
    bool GenerateReport(const std::string& outputPath);
    
    /**
     * @brief Simulate item drops with the current configuration
     * @param numDrops Number of drops to simulate
     * @param monsterLevel Monster level to use for the simulation
     * @param dungeonLevel Dungeon level to use for the simulation
     * @param context Drop context to use for the simulation
     * @return A map of item types to drop counts
     */
    std::map<std::string, int> SimulateDrops(
        int numDrops, 
        int monsterLevel, 
        int dungeonLevel, 
        DropRateContext context = DropRateContext::MonsterDrop
    );
    
    /**
     * @brief Get the modified drop rates for all items
     * @param monsterLevel Monster level to use for the calculation
     * @param dungeonLevel Dungeon level to use for the calculation
     * @param context Drop context to use for the calculation
     * @return A map of item names to their modified drop rates
     */
    std::map<std::string, float> GetModifiedDropRates(
        int monsterLevel, 
        int dungeonLevel, 
        DropRateContext context = DropRateContext::MonsterDrop
    );
    
    /**
     * @brief Compare drop rates before and after applying modifiers
     * @param monsterLevel Monster level to use for the calculation
     * @param dungeonLevel Dungeon level to use for the calculation
     * @param context Drop context to use for the calculation
     * @return A map of item names to pairs of (original, modified) drop rates
     */
    std::map<std::string, std::pair<float, float>> CompareDropRates(
        int monsterLevel, 
        int dungeonLevel, 
        DropRateContext context = DropRateContext::MonsterDrop
    );

private:
    DropRateTest() = default;
    DropRateConfig& config = DropRateConfig::getInstance();
    
    /**
     * @brief Get a human-readable name for an item
     * @param item The item data
     * @return A string containing the item name and type
     */
    std::string GetItemDisplayName(const ItemData& item);
    
    /**
     * @brief Group items by type for the report
     * @return A map of item types to vectors of item indices
     */
    std::map<std::string, std::vector<size_t>> GroupItemsByType();
};

} // namespace devilution
