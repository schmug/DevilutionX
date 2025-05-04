/**
 * @file gear_tools.cpp
 *
 * Implementation of gear scoring tools and utilities.
 */
#include "gear/gear_tools.h"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>

#include "utils/log.hpp"

namespace devilution {

ItemComparisonResult GearTools::CompareItems(const Item &item1, const Item &item2)
{
    ItemComparisonResult result;
    result.item1 = &item1;
    result.item2 = &item2;
    
    // Calculate detailed scores for both items
    result.score1 = GearScorer::CalculateDetailedItemScore(item1);
    result.score2 = GearScorer::CalculateDetailedItemScore(item2);
    
    // Calculate difference and percentage change
    result.scoreDifference = result.score2.totalScore - result.score1.totalScore;
    if (result.score1.totalScore > 0) {
        result.scorePercentChange = (result.scoreDifference / result.score1.totalScore) * 100.0f;
    } else {
        result.scorePercentChange = result.score2.totalScore > 0 ? 100.0f : 0.0f;
    }
    
    // Generate comparison text
    std::stringstream comparison;
    comparison << "Item Comparison: " << item1._iIName << " vs " << item2._iIName << "\n\n";
    
    // Format scores with 2 decimal places
    comparison << std::fixed << std::setprecision(2);
    
    // Compare base scores
    comparison << "Base Score:\n";
    comparison << "  " << item1._iIName << ": " << result.score1.baseScore << "\n";
    comparison << "  " << item2._iIName << ": " << result.score2.baseScore << "\n";
    float baseDiff = result.score2.baseScore - result.score1.baseScore;
    comparison << "  Difference: " << baseDiff << " (" 
              << (baseDiff > 0 ? "+" : "") << (result.score1.baseScore > 0 ? (baseDiff / result.score1.baseScore) * 100.0f : 0.0f) 
              << "%)\n\n";
    
    // Compare affix scores
    comparison << "Affix Score:\n";
    comparison << "  " << item1._iIName << ": " << result.score1.affixScore << "\n";
    comparison << "  " << item2._iIName << ": " << result.score2.affixScore << "\n";
    float affixDiff = result.score2.affixScore - result.score1.affixScore;
    comparison << "  Difference: " << affixDiff << " (" 
              << (affixDiff > 0 ? "+" : "") << (result.score1.affixScore > 0 ? (affixDiff / result.score1.affixScore) * 100.0f : 0.0f) 
              << "%)\n\n";
    
    // Compare level factors
    comparison << "Level Factor:\n";
    comparison << "  " << item1._iIName << ": " << result.score1.levelFactor << " (Item level: " << item1._iILvl << ")\n";
    comparison << "  " << item2._iIName << ": " << result.score2.levelFactor << " (Item level: " << item2._iILvl << ")\n\n";
    
    // Compare total scores
    comparison << "Total Score:\n";
    comparison << "  " << item1._iIName << ": " << result.score1.totalScore << "\n";
    comparison << "  " << item2._iIName << ": " << result.score2.totalScore << "\n";
    comparison << "  Difference: " << result.scoreDifference << " (" 
              << (result.scoreDifference > 0 ? "+" : "") << result.scorePercentChange 
              << "%)\n\n";
    
    // Overall recommendation
    comparison << "Recommendation: ";
    if (std::abs(result.scorePercentChange) < 5.0f) {
        comparison << "Items are roughly equivalent (within 5% score difference).";
    } else if (result.scoreDifference > 0) {
        comparison << item2._iIName << " is better by " << result.scorePercentChange << "%.";
    } else {
        comparison << item1._iIName << " is better by " << -result.scorePercentChange << "%.";
    }
    
    result.comparisonText = comparison.str();
    return result;
}

const Item* GearTools::FindBestItemForSlot(const Player &player, inv_body_loc slot)
{
    const Item *bestItem = nullptr;
    float bestScore = 0.0f;
    
    // Check currently equipped item
    const Item &equippedItem = player.InvBody[slot];
    if (!equippedItem.isEmpty()) {
        bestItem = &equippedItem;
        bestScore = GearScorer::CalculateItemScore(equippedItem);
    }
    
    // Check inventory for better items that can be equipped in this slot
    for (int i = 0; i < InventoryGridCells; i++) {
        const Item &invItem = player.InvList[i];
        if (invItem.isEmpty()) {
            continue;
        }
        
        // Check if this item can be equipped in the target slot
        bool canEquip = false;
        switch (slot) {
        case INVLOC_HEAD:
            canEquip = invItem.isHelm();
            break;
        case INVLOC_RING_LEFT:
        case INVLOC_RING_RIGHT:
            canEquip = invItem._itype == ItemType::Ring;
            break;
        case INVLOC_AMULET:
            canEquip = invItem._itype == ItemType::Amulet;
            break;
        case INVLOC_HAND_LEFT:
        case INVLOC_HAND_RIGHT:
            canEquip = invItem.isWeapon() || invItem.isShield();
            break;
        case INVLOC_CHEST:
            canEquip = invItem.isArmor() && !invItem.isHelm() && !invItem.isShield();
            break;
        default:
            canEquip = false;
            break;
        }
        
        if (canEquip) {
            float score = GearScorer::CalculateItemScore(invItem);
            if (score > bestScore) {
                bestItem = &invItem;
                bestScore = score;
            }
        }
    }
    
    return bestItem;
}

std::string GearTools::SuggestGearUpgrades(const Player &player)
{
    std::stringstream suggestions;
    suggestions << "Gear Upgrade Suggestions for " << player._pName << ":\n\n";
    suggestions << std::fixed << std::setprecision(2);
    
    // Check each equipment slot
    for (int i = 0; i < NUM_INVLOC; i++) {
        inv_body_loc slot = static_cast<inv_body_loc>(i);
        const Item &equippedItem = player.InvBody[slot];
        
        // Get slot name
        std::string slotName;
        switch (slot) {
        case INVLOC_HEAD:
            slotName = "Head";
            break;
        case INVLOC_RING_LEFT:
            slotName = "Left Ring";
            break;
        case INVLOC_RING_RIGHT:
            slotName = "Right Ring";
            break;
        case INVLOC_AMULET:
            slotName = "Amulet";
            break;
        case INVLOC_HAND_LEFT:
            slotName = "Left Hand";
            break;
        case INVLOC_HAND_RIGHT:
            slotName = "Right Hand";
            break;
        case INVLOC_CHEST:
            slotName = "Chest";
            break;
        default:
            slotName = "Unknown";
            break;
        }
        
        suggestions << slotName << ":\n";
        
        if (equippedItem.isEmpty()) {
            suggestions << "  Currently empty\n";
            
            // Find best item for this slot
            const Item *bestItem = FindBestItemForSlot(player, slot);
            if (bestItem && !bestItem->isEmpty()) {
                suggestions << "  Suggestion: Equip " << bestItem->_iIName << " (Score: " 
                          << GearScorer::CalculateItemScore(*bestItem) << ")\n";
            } else {
                suggestions << "  Suggestion: Find an item for this slot\n";
            }
        } else {
            float equippedScore = GearScorer::CalculateItemScore(equippedItem);
            suggestions << "  Current: " << equippedItem._iIName << " (Score: " << equippedScore << ")\n";
            
            // Find best item for this slot
            const Item *bestItem = FindBestItemForSlot(player, slot);
            if (bestItem && bestItem != &equippedItem) {
                float bestScore = GearScorer::CalculateItemScore(*bestItem);
                float improvement = ((bestScore - equippedScore) / equippedScore) * 100.0f;
                
                suggestions << "  Suggestion: Replace with " << bestItem->_iIName 
                          << " (Score: " << bestScore << ", +" << improvement << "%)\n";
            } else {
                suggestions << "  Suggestion: Keep current item\n";
            }
        }
        
        suggestions << "\n";
    }
    
    // Overall gear level
    float gearLevel = GearScorer::CalculateGearLevel(player);
    suggestions << "Current Gear Level: " << gearLevel << "\n";
    
    // Suggest priority upgrades
    suggestions << "\nPriority Upgrades:\n";
    
    // Find the slot with the lowest score relative to its importance
    inv_body_loc worstSlot = INVLOC_HEAD;
    float worstRelativeScore = std::numeric_limits<float>::max();
    
    for (int i = 0; i < NUM_INVLOC; i++) {
        inv_body_loc slot = static_cast<inv_body_loc>(i);
        const Item &item = player.InvBody[slot];
        
        float score = item.isEmpty() ? 0.0f : GearScorer::CalculateItemScore(item);
        float importance = SlotImportance[i];
        float relativeScore = score / importance;
        
        if (relativeScore < worstRelativeScore) {
            worstRelativeScore = relativeScore;
            worstSlot = slot;
        }
    }
    
    // Suggest upgrading the worst slot
    std::string slotName;
    switch (worstSlot) {
    case INVLOC_HEAD:
        slotName = "Head";
        break;
    case INVLOC_RING_LEFT:
        slotName = "Left Ring";
        break;
    case INVLOC_RING_RIGHT:
        slotName = "Right Ring";
        break;
    case INVLOC_AMULET:
        slotName = "Amulet";
        break;
    case INVLOC_HAND_LEFT:
        slotName = "Left Hand";
        break;
    case INVLOC_HAND_RIGHT:
        slotName = "Right Hand";
        break;
    case INVLOC_CHEST:
        slotName = "Chest";
        break;
    default:
        slotName = "Unknown";
        break;
    }
    
    suggestions << "1. Focus on upgrading your " << slotName << " slot first\n";
    
    return suggestions.str();
}

std::string GearTools::GenerateScoreDistribution(const Player &player)
{
    std::stringstream distribution;
    distribution << "Gear Score Distribution for " << player._pName << ":\n\n";
    distribution << std::fixed << std::setprecision(2);
    
    // Collect all items from inventory
    std::vector<std::pair<const Item*, float>> allItems;
    
    // Add equipped items
    for (int i = 0; i < NUM_INVLOC; i++) {
        const Item &item = player.InvBody[i];
        if (!item.isEmpty()) {
            float score = GearScorer::CalculateItemScore(item);
            allItems.push_back(std::make_pair(&item, score));
        }
    }
    
    // Add inventory items
    for (int i = 0; i < InventoryGridCells; i++) {
        const Item &item = player.InvList[i];
        if (!item.isEmpty() && !item.isGold()) {
            float score = GearScorer::CalculateItemScore(item);
            allItems.push_back(std::make_pair(&item, score));
        }
    }
    
    // Sort items by score
    std::sort(allItems.begin(), allItems.end(), [](const auto &a, const auto &b) {
        return a.second > b.second;
    });
    
    // Calculate statistics
    float totalScore = 0.0f;
    float minScore = std::numeric_limits<float>::max();
    float maxScore = 0.0f;
    
    for (const auto &pair : allItems) {
        totalScore += pair.second;
        minScore = std::min(minScore, pair.second);
        maxScore = std::max(maxScore, pair.second);
    }
    
    float avgScore = allItems.empty() ? 0.0f : totalScore / allItems.size();
    
    // Generate distribution
    distribution << "Total Items: " << allItems.size() << "\n";
    distribution << "Average Score: " << avgScore << "\n";
    distribution << "Min Score: " << (allItems.empty() ? 0.0f : minScore) << "\n";
    distribution << "Max Score: " << maxScore << "\n\n";
    
    // Generate histogram
    const int numBins = 10;
    std::vector<int> histogram(numBins, 0);
    
    if (!allItems.empty()) {
        float range = maxScore - minScore;
        float binSize = range > 0 ? range / numBins : 1.0f;
        
        for (const auto &pair : allItems) {
            int bin = range > 0 ? std::min(numBins - 1, static_cast<int>((pair.second - minScore) / binSize)) : 0;
            histogram[bin]++;
        }
        
        // Print histogram
        distribution << "Score Distribution:\n";
        
        for (int i = 0; i < numBins; i++) {
            float binStart = minScore + i * binSize;
            float binEnd = minScore + (i + 1) * binSize;
            
            distribution << binStart << " - " << binEnd << ": ";
            
            // Print bar
            int barLength = histogram[i] * 50 / allItems.size();
            for (int j = 0; j < barLength; j++) {
                distribution << "#";
            }
            
            distribution << " (" << histogram[i] << " items)\n";
        }
    }
    
    // List top 5 items
    distribution << "\nTop 5 Items by Score:\n";
    
    for (size_t i = 0; i < std::min(allItems.size(), size_t(5)); i++) {
        const Item *item = allItems[i].first;
        float score = allItems[i].second;
        
        distribution << (i + 1) << ". " << item->_iIName << " (Score: " << score << ")\n";
    }
    
    return distribution.str();
}

std::string GearTools::SimulateItemUpgrade(const Player &player, const Item &newItem, inv_body_loc slot)
{
    std::stringstream simulation;
    simulation << "Item Upgrade Simulation for " << player._pName << ":\n\n";
    simulation << std::fixed << std::setprecision(2);
    
    // Calculate current gear level
    float currentGearLevel = GearScorer::CalculateGearLevel(player);
    
    // Create a copy of the player to simulate the upgrade
    Player simulatedPlayer = player;
    
    // Store the old item for comparison
    Item oldItem = simulatedPlayer.InvBody[slot];
    
    // Equip the new item
    simulatedPlayer.InvBody[slot] = newItem;
    
    // Calculate new gear level
    float newGearLevel = GearScorer::CalculateGearLevel(simulatedPlayer);
    
    // Calculate the difference
    float levelDifference = newGearLevel - currentGearLevel;
    float percentChange = (levelDifference / currentGearLevel) * 100.0f;
    
    // Generate simulation report
    simulation << "Current Gear Level: " << currentGearLevel << "\n";
    
    if (!oldItem.isEmpty()) {
        simulation << "Replacing: " << oldItem._iIName << " (Score: " << GearScorer::CalculateItemScore(oldItem) << ")\n";
    } else {
        simulation << "Equipping in empty slot\n";
    }
    
    simulation << "New Item: " << newItem._iIName << " (Score: " << GearScorer::CalculateItemScore(newItem) << ")\n\n";
    
    simulation << "New Gear Level: " << newGearLevel << "\n";
    simulation << "Difference: " << levelDifference << " (" << (levelDifference > 0 ? "+" : "") << percentChange << "%)\n\n";
    
    // Provide interpretation
    simulation << "Impact: ";
    
    if (std::abs(percentChange) < 2.0f) {
        simulation << "Minimal impact on overall gear level (less than 2% change).";
    } else if (percentChange > 10.0f) {
        simulation << "Significant upgrade! This will increase your gear level by over 10%.";
    } else if (percentChange > 5.0f) {
        simulation << "Substantial upgrade that will increase your gear level by over 5%.";
    } else if (percentChange > 0) {
        simulation << "Moderate upgrade that will slightly increase your gear level.";
    } else {
        simulation << "This item would actually decrease your gear level.";
    }
    
    return simulation.str();
}

std::string GearTools::GenerateBenchmarkReport()
{
    std::stringstream report;
    report << "Gear Scoring Benchmark Report\n";
    report << "============================\n\n";
    report << std::fixed << std::setprecision(2);
    
    // Define benchmark items
    struct BenchmarkItem {
        std::string name;
        item_quality quality;
        GearCategory category;
        int level;
        float expectedScore;
        float actualScore;
    };
    
    std::vector<BenchmarkItem> benchmarks = {
        { "Basic Sword", ITEM_QUALITY_NORMAL, GearCategory::Weapon, 1, 5.0f, 0.0f },
        { "Magic Sword", ITEM_QUALITY_MAGIC, GearCategory::Weapon, 10, 15.0f, 0.0f },
        { "Unique Sword", ITEM_QUALITY_UNIQUE, GearCategory::Weapon, 20, 30.0f, 0.0f },
        { "Basic Armor", ITEM_QUALITY_NORMAL, GearCategory::Armor, 1, 4.0f, 0.0f },
        { "Magic Armor", ITEM_QUALITY_MAGIC, GearCategory::Armor, 10, 12.0f, 0.0f },
        { "Unique Armor", ITEM_QUALITY_UNIQUE, GearCategory::Armor, 20, 25.0f, 0.0f },
        { "Basic Ring", ITEM_QUALITY_NORMAL, GearCategory::Jewelry, 1, 3.0f, 0.0f },
        { "Magic Ring", ITEM_QUALITY_MAGIC, GearCategory::Jewelry, 10, 10.0f, 0.0f },
        { "Unique Ring", ITEM_QUALITY_UNIQUE, GearCategory::Jewelry, 20, 20.0f, 0.0f }
    };
    
    // Create mock items for benchmarking
    for (auto &benchmark : benchmarks) {
        Item mockItem;
        mockItem._iIName = benchmark.name;
        mockItem._iMagical = benchmark.quality;
        mockItem._iILvl = benchmark.level;
        
        // Set type based on category
        switch (benchmark.category) {
        case GearCategory::Weapon:
            mockItem._itype = ItemType::Sword;
            mockItem._iClass = ICLASS_WEAPON;
            mockItem._iMinDam = 1 + benchmark.level / 2;
            mockItem._iMaxDam = 3 + benchmark.level;
            break;
        case GearCategory::Armor:
            mockItem._itype = ItemType::LightArmor;
            mockItem._iClass = ICLASS_ARMOR;
            mockItem._iAC = 5 + benchmark.level;
            break;
        case GearCategory::Jewelry:
            mockItem._itype = ItemType::Ring;
            mockItem._iClass = ICLASS_MISC;
            break;
        default:
            break;
        }
        
        // Add some affixes based on quality
        if (benchmark.quality == ITEM_QUALITY_MAGIC || benchmark.quality == ITEM_QUALITY_UNIQUE) {
            mockItem._iPLStr = 1 + benchmark.level / 5;
            mockItem._iPLMag = 1 + benchmark.level / 5;
            mockItem._iPLDex = 1 + benchmark.level / 5;
            mockItem._iPLVit = 1 + benchmark.level / 5;
        }
        
        if (benchmark.quality == ITEM_QUALITY_UNIQUE) {
            mockItem._iPLFR = 5 + benchmark.level / 2;
            mockItem._iPLLR = 5 + benchmark.level / 2;
            mockItem._iPLMR = 5 + benchmark.level / 2;
        }
        
        // Calculate actual score
        benchmark.actualScore = GearScorer::CalculateItemScore(mockItem);
    }
    
    // Generate report
    report << "Item Scores:\n";
    report << "------------\n";
    report << std::setw(15) << "Item" << std::setw(10) << "Quality" << std::setw(10) << "Category" 
           << std::setw(8) << "Level" << std::setw(12) << "Expected" << std::setw(12) << "Actual" 
           << std::setw(12) << "Difference" << std::setw(10) << "% Diff" << "\n";
    report << std::string(79, '-') << "\n";
    
    float totalDiffPercent = 0.0f;
    int numBenchmarks = benchmarks.size();
    
    for (const auto &benchmark : benchmarks) {
        std::string qualityStr;
        switch (benchmark.quality) {
        case ITEM_QUALITY_NORMAL:
            qualityStr = "Normal";
            break;
        case ITEM_QUALITY_MAGIC:
            qualityStr = "Magic";
            break;
        case ITEM_QUALITY_UNIQUE:
            qualityStr = "Unique";
            break;
        default:
            qualityStr = "Unknown";
            break;
        }
        
        std::string categoryStr = GetGearCategoryName(benchmark.category);
        
        float diff = benchmark.actualScore - benchmark.expectedScore;
        float diffPercent = benchmark.expectedScore != 0 ? (diff / benchmark.expectedScore) * 100.0f : 0.0f;
        
        totalDiffPercent += std::abs(diffPercent);
        
        report << std::setw(15) << benchmark.name << std::setw(10) << qualityStr << std::setw(10) << categoryStr 
               << std::setw(8) << benchmark.level << std::setw(12) << benchmark.expectedScore 
               << std::setw(12) << benchmark.actualScore << std::setw(12) << diff 
               << std::setw(9) << (diff > 0 ? "+" : "") << diffPercent << "%" << "\n";
    }
    
    float avgDiffPercent = numBenchmarks > 0 ? totalDiffPercent / numBenchmarks : 0.0f;
    
    report << "\nAverage Deviation: " << avgDiffPercent << "%\n\n";
    
    // Overall assessment
    report << "Assessment:\n";
    report << "-----------\n";
    
    if (avgDiffPercent < 10.0f) {
        report << "The scoring system is well-calibrated (average deviation < 10%).\n";
    } else if (avgDiffPercent < 20.0f) {
        report << "The scoring system shows moderate deviation from expected values (10-20%).\n";
        report << "Consider adjusting the scoring parameters for better calibration.\n";
    } else {
        report << "The scoring system shows significant deviation from expected values (>20%).\n";
        report << "Recommend a thorough review and recalibration of the scoring parameters.\n";
    }
    
    return report.str();
}

} // namespace devilution
