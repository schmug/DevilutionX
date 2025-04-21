/**
 * @file drop_rate_test.cpp
 *
 * Implementation of testing utilities for the item drop rate modification system.
 */

#include "mods/drop_rate_test.h"

#include <fstream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <random>

#include "mods/drop_rate_modifier.h"

#include "utils/log.hpp"
#include "utils/paths.h"
#include "diablo.h"
#include "engine/random.hpp"

namespace devilution {

DropRateTest& DropRateTest::getInstance()
{
    static DropRateTest instance;
    return instance;
}

bool DropRateTest::GenerateReport(const std::string& outputPath)
{
    try {
        std::ofstream report(outputPath);
        if (!report.is_open()) {
            LogError("Failed to open drop rate report file: {}", outputPath);
            return false;
        }
        
        // Write report header
        report << "# Item Drop Rate Modification Report\n\n";
        report << "Generated on: " << SDL_GetTicks() << "\n\n";
        report << "## Configuration Summary\n\n";
        
        // Write configuration summary
        report << "Max Drop Rate Multiplier: " << config.GetMaxDropRateMultiplier() << "\n";
        report << "Min Drop Rate Multiplier: " << config.GetMinDropRateMultiplier() << "\n";
        report << "Number of Modifiers: " << config.GetModifiers().size() << "\n\n";
        
        // Write active modifiers
        report << "## Active Modifiers\n\n";
        report << "| Priority | Scope | Target | Context | Type | Value | Conditions |\n";
        report << "|----------|-------|--------|---------|------|-------|------------|\n";
        
        for (const auto& modifier : config.GetModifiers()) {
            std::string scope;
            switch (modifier.scope) {
                case DropRateScope::Global: scope = "Global"; break;
                case DropRateScope::ItemType: scope = "ItemType"; break;
                case DropRateScope::ItemClass: scope = "ItemClass"; break;
                case DropRateScope::ItemId: scope = "ItemId"; break;
                case DropRateScope::UniqueItem: scope = "UniqueItem"; break;
                default: scope = "Unknown"; break;
            }
            
            std::string context;
            switch (modifier.context) {
                case DropRateContext::Always: context = "Always"; break;
                case DropRateContext::MonsterDrop: context = "MonsterDrop"; break;
                case DropRateContext::ChestDrop: context = "ChestDrop"; break;
                case DropRateContext::GroundDrop: context = "GroundDrop"; break;
                case DropRateContext::QuestReward: context = "QuestReward"; break;
                default: context = "Unknown"; break;
            }
            
            std::string modType;
            switch (modifier.modType) {
                case DropRateModType::Multiplier: modType = "Multiplier"; break;
                case DropRateModType::AddFlat: modType = "AddFlat"; break;
                case DropRateModType::SetAbsolute: modType = "SetAbsolute"; break;
                default: modType = "Unknown"; break;
            }
            
            std::string conditions;
            if (modifier.condition.minMonsterLevel.has_value()) {
                conditions += "minMonsterLevel=" + std::to_string(modifier.condition.minMonsterLevel.value()) + " ";
            }
            if (modifier.condition.maxMonsterLevel.has_value()) {
                conditions += "maxMonsterLevel=" + std::to_string(modifier.condition.maxMonsterLevel.value()) + " ";
            }
            if (modifier.condition.minDungeonLevel.has_value()) {
                conditions += "minDungeonLevel=" + std::to_string(modifier.condition.minDungeonLevel.value()) + " ";
            }
            if (modifier.condition.maxDungeonLevel.has_value()) {
                conditions += "maxDungeonLevel=" + std::to_string(modifier.condition.maxDungeonLevel.value()) + " ";
            }
            if (modifier.condition.difficulty.has_value()) {
                conditions += "difficulty=" + std::to_string(modifier.condition.difficulty.value()) + " ";
            }
            if (modifier.condition.multiplayerOnly.has_value()) {
                conditions += "multiplayerOnly=" + std::string(modifier.condition.multiplayerOnly.value() ? "true" : "false");
            }
            if (conditions.empty()) {
                conditions = "None";
            }
            
            report << "| " << modifier.priority << " | " 
                   << scope << " | " 
                   << (modifier.targetId.empty() ? "N/A" : modifier.targetId) << " | " 
                   << context << " | " 
                   << modType << " | " 
                   << modifier.value << " | " 
                   << conditions << " |\n";
        }
        
        report << "\n## Drop Rate Comparison (Monster Level 30, Dungeon Level 16)\n\n";
        
        // Compare drop rates for monster level 30, dungeon level 16
        auto comparison = CompareDropRates(30, 16, DropRateContext::MonsterDrop);
        
        report << "| Item | Original Drop Rate | Modified Drop Rate | Change Factor |\n";
        report << "|------|-------------------|-------------------|---------------|\n";
        
        for (const auto& [itemName, rates] : comparison) {
            float originalRate = rates.first;
            float modifiedRate = rates.second;
            float changeFactor = originalRate > 0 ? modifiedRate / originalRate : 0;
            
            report << "| " << itemName << " | " 
                   << std::fixed << std::setprecision(2) << originalRate << " | " 
                   << std::fixed << std::setprecision(2) << modifiedRate << " | " 
                   << std::fixed << std::setprecision(2) << changeFactor << "x |\n";
        }
        
        report << "\n## Drop Simulation (1000 drops, Monster Level 30, Dungeon Level 16)\n\n";
        
        // Simulate 1000 drops for monster level 30, dungeon level 16
        auto simulation = SimulateDrops(1000, 30, 16, DropRateContext::MonsterDrop);
        
        report << "| Item Type | Drop Count | Percentage |\n";
        report << "|-----------|------------|------------|\n";
        
        int totalDrops = 0;
        for (const auto& [itemType, count] : simulation) {
            totalDrops += count;
        }
        
        for (const auto& [itemType, count] : simulation) {
            float percentage = static_cast<float>(count) / totalDrops * 100.0f;
            report << "| " << itemType << " | " 
                   << count << " | " 
                   << std::fixed << std::setprecision(2) << percentage << "% |\n";
        }
        
        report.close();
        LogVerbose("Drop rate report generated: {}", outputPath);
        return true;
    } catch (const std::exception& e) {
        LogError("Exception while generating drop rate report: {}", e.what());
        return false;
    }
}

std::map<std::string, int> DropRateTest::SimulateDrops(
    int numDrops, 
    int monsterLevel, 
    int dungeonLevel, 
    DropRateContext context)
{
    std::map<std::string, int> results;
    
    // Initialize random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Get all items and their modified drop rates
    std::vector<std::pair<const ItemData*, float>> itemsWithRates;
    float totalWeight = 0.0f;
    
    for (size_t i = 0; i < AllItemsList.size(); i++) {
        const ItemData& item = AllItemsList[i];
        if (item.dropRate > 0) {
            bool isUnique = (item.iItemId != UITYPE_NONE);
            float modifiedRate = config.GetModifiedDropRate(item, context, monsterLevel, dungeonLevel, isUnique);
            if (modifiedRate > 0) {
                itemsWithRates.push_back({&item, modifiedRate});
                totalWeight += modifiedRate;
            }
        }
    }
    
    // Create distribution based on modified drop rates
    std::uniform_real_distribution<float> dist(0, totalWeight);
    
    // Simulate drops
    for (int i = 0; i < numDrops; i++) {
        float roll = dist(gen);
        float cumulativeWeight = 0.0f;
        
        for (const auto& [item, weight] : itemsWithRates) {
            cumulativeWeight += weight;
            if (roll <= cumulativeWeight) {
                std::string itemType = ItemTypeToString(item->itype);
                results[itemType]++;
                break;
            }
        }
    }
    
    return results;
}

std::map<std::string, float> DropRateTest::GetModifiedDropRates(
    int monsterLevel, 
    int dungeonLevel, 
    DropRateContext context)
{
    std::map<std::string, float> results;
    
    for (size_t i = 0; i < AllItemsList.size(); i++) {
        const ItemData& item = AllItemsList[i];
        if (item.dropRate > 0) {
            bool isUnique = (item.iItemId != UITYPE_NONE);
            float modifiedRate = config.GetModifiedDropRate(item, context, monsterLevel, dungeonLevel, isUnique);
            if (modifiedRate > 0) {
                std::string itemName = GetItemDisplayName(item);
                results[itemName] = modifiedRate;
            }
        }
    }
    
    return results;
}

std::map<std::string, std::pair<float, float>> DropRateTest::CompareDropRates(
    int monsterLevel, 
    int dungeonLevel, 
    DropRateContext context)
{
    std::map<std::string, std::pair<float, float>> results;
    
    for (size_t i = 0; i < AllItemsList.size(); i++) {
        const ItemData& item = AllItemsList[i];
        if (item.dropRate > 0) {
            bool isUnique = (item.iItemId != UITYPE_NONE);
            float originalRate = static_cast<float>(item.dropRate);
            float modifiedRate = config.GetModifiedDropRate(item, context, monsterLevel, dungeonLevel, isUnique);
            
            std::string itemName = GetItemDisplayName(item);
            results[itemName] = std::make_pair(originalRate, modifiedRate);
        }
    }
    
    return results;
}

std::string DropRateTest::GetItemDisplayName(const ItemData& item)
{
    std::string itemType = ItemTypeToString(item.itype);
    std::string itemName = item.iName;
    
    if (itemName.empty()) {
        return itemType;
    }
    
    return itemName + " (" + itemType + ")";
}

std::map<std::string, std::vector<size_t>> DropRateTest::GroupItemsByType()
{
    std::map<std::string, std::vector<size_t>> groups;
    
    for (size_t i = 0; i < AllItemsList.size(); i++) {
        const ItemData& item = AllItemsList[i];
        std::string itemType = ItemTypeToString(item.itype);
        groups[itemType].push_back(i);
    }
    
    return groups;
}

} // namespace devilution
