/**
 * @file gear_manager.cpp
 *
 * Implementation of the gear level manager that integrates the gear scoring system with the game.
 */
#include "gear/gear_manager.h"

#include <algorithm>
#include <cmath>
#include <sstream>

#include "utils/log.hpp"
#include "gear/gear_config.h"
#include "gear/gear_tools.h"

namespace devilution {

GearLevelManager& GearLevelManager::GetInstance()
{
    static GearLevelManager instance;
    return instance;
}

void GearLevelManager::Initialize()
{
    if (initialized) {
        return;
    }
    
    // Load configuration
    GearConfigLoader::LoadFromFile("gear_config.json");
    
    // Clear any existing cache
    gearLevelCache.clear();
    
    initialized = true;
    LogVerbose("Gear Level Manager initialized");
}

float GearLevelManager::GetCurrentGearLevel(const Player &player)
{
    // Check cache first
    uint8_t playerId = player.getId();
    auto it = gearLevelCache.find(playerId);
    if (it != gearLevelCache.end()) {
        return it->second;
    }
    
    // Calculate gear level
    float gearLevel = CalculateGearLevel(player);
    
    // Cache the result
    gearLevelCache[playerId] = gearLevel;
    
    return gearLevel;
}

float GearLevelManager::GetPotentialGearLevel(const Player &player, const Item &newItem, inv_body_loc slot)
{
    // Create a copy of the player to simulate the equipment change
    Player simulatedPlayer = player;
    
    // Store the old item
    Item oldItem = simulatedPlayer.InvBody[slot];
    
    // Equip the new item
    simulatedPlayer.InvBody[slot] = newItem;
    
    // Calculate the new gear level
    float potentialGearLevel = CalculateGearLevel(simulatedPlayer);
    
    return potentialGearLevel;
}

float GearLevelManager::CompareItems(const Player &player, const Item &item1, const Item &item2, inv_body_loc slot)
{
    // Get gear level with item1
    float gearLevel1 = GetPotentialGearLevel(player, item1, slot);
    
    // Get gear level with item2
    float gearLevel2 = GetPotentialGearLevel(player, item2, slot);
    
    // Return the difference (positive if item2 is better)
    return gearLevel2 - gearLevel1;
}

std::string GearLevelManager::GetGearLevelExplanation(const Player &player)
{
    std::stringstream explanation;
    explanation << "Gear Level Explanation for " << player._pName << ":\n\n";
    
    // Calculate current gear level
    float gearLevel = GetCurrentGearLevel(player);
    
    // Get individual item scores
    explanation << "Item Scores:\n";
    float totalWeightedScore = 0.0f;
    float totalWeight = 0.0f;
    
    for (int i = 0; i < NUM_INVLOC; i++) {
        const Item &item = player.InvBody[i];
        float slotWeight = SlotImportance[i];
        
        // Get slot name
        std::string slotName;
        switch (static_cast<inv_body_loc>(i)) {
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
        
        if (!item.isEmpty()) {
            float itemScore = GearScorer::CalculateItemScore(item);
            float weightedScore = itemScore * slotWeight;
            
            totalWeightedScore += weightedScore;
            totalWeight += slotWeight;
            
            explanation << "- " << slotName << ": " << item._iIName << " (Score: " << itemScore 
                       << ", Weight: " << slotWeight << ", Weighted: " << weightedScore << ")\n";
        } else {
            explanation << "- " << slotName << ": Empty\n";
            totalWeight += slotWeight;
        }
    }
    
    // Calculate weighted average
    float gearScore = (totalWeight > 0) ? (totalWeightedScore / totalWeight) : 0.0f;
    
    // Get configuration
    const GearScoringConfig &config = GearConfigLoader::GetConfig();
    
    // Factor in character level
    float levelFactor = std::sqrt(static_cast<float>(player._pLevel)) * config.characterLevelWeight;
    
    explanation << "\nGear Score: " << gearScore << " (Weighted average of item scores)\n";
    explanation << "Character Level Factor: " << levelFactor << " (Based on level " << player._pLevel << ")\n";
    explanation << "Gear Level: " << gearLevel << " (Gear Score + Level Factor, normalized)\n";
    
    // Add difficulty interpretation
    explanation << "\nDifficulty Interpretation:\n";
    if (gearLevel < 20.0f) {
        explanation << "- Beginner level gear (< 20)\n";
        explanation << "- Monsters will be relatively easy\n";
    } else if (gearLevel < 40.0f) {
        explanation << "- Novice level gear (20-40)\n";
        explanation << "- Monsters will provide a moderate challenge\n";
    } else if (gearLevel < 60.0f) {
        explanation << "- Intermediate level gear (40-60)\n";
        explanation << "- Monsters will be challenging\n";
    } else if (gearLevel < 80.0f) {
        explanation << "- Advanced level gear (60-80)\n";
        explanation << "- Monsters will be very challenging\n";
    } else {
        explanation << "- Expert level gear (80+)\n";
        explanation << "- Monsters will be extremely challenging\n";
    }
    
    return explanation.str();
}

void GearLevelManager::InvalidateCache(const Player &player)
{
    uint8_t playerId = player.getId();
    gearLevelCache.erase(playerId);
    LogVerbose("Invalidated gear level cache for player {}", playerId);
}

uint32_t GearLevelManager::RegisterChangeCallback(GearLevelChangeCallback callback)
{
    uint32_t callbackId = nextCallbackId++;
    changeCallbacks[callbackId] = callback;
    return callbackId;
}

bool GearLevelManager::UnregisterChangeCallback(uint32_t callbackId)
{
    auto it = changeCallbacks.find(callbackId);
    if (it != changeCallbacks.end()) {
        changeCallbacks.erase(it);
        return true;
    }
    return false;
}

void GearLevelManager::OnItemEquipped(Player &player, const Item &item, inv_body_loc slot)
{
    // Get old gear level
    float oldGearLevel = GetCurrentGearLevel(player);
    
    // Invalidate cache
    InvalidateCache(player);
    
    // Get new gear level
    float newGearLevel = GetCurrentGearLevel(player);
    
    // Fire change event
    GearLevelChangeEvent event;
    event.player = &player;
    event.oldGearLevel = oldGearLevel;
    event.newGearLevel = newGearLevel;
    event.eventType = GearLevelEventType::ItemEquipped;
    event.slot = slot;
    event.item = &item;
    
    FireChangeEvent(event);
}

void GearLevelManager::OnItemUnequipped(Player &player, inv_body_loc slot)
{
    // Get old gear level
    float oldGearLevel = GetCurrentGearLevel(player);
    
    // Invalidate cache
    InvalidateCache(player);
    
    // Get new gear level
    float newGearLevel = GetCurrentGearLevel(player);
    
    // Fire change event
    GearLevelChangeEvent event;
    event.player = &player;
    event.oldGearLevel = oldGearLevel;
    event.newGearLevel = newGearLevel;
    event.eventType = GearLevelEventType::ItemUnequipped;
    event.slot = slot;
    event.item = nullptr;
    
    FireChangeEvent(event);
}

void GearLevelManager::OnPlayerLevelUp(Player &player)
{
    // Get old gear level
    float oldGearLevel = GetCurrentGearLevel(player);
    
    // Invalidate cache
    InvalidateCache(player);
    
    // Get new gear level
    float newGearLevel = GetCurrentGearLevel(player);
    
    // Fire change event
    GearLevelChangeEvent event;
    event.player = &player;
    event.oldGearLevel = oldGearLevel;
    event.newGearLevel = newGearLevel;
    event.eventType = GearLevelEventType::LevelUp;
    event.slot = static_cast<inv_body_loc>(0);
    event.item = nullptr;
    
    FireChangeEvent(event);
}

float GearLevelManager::CalculateGearLevel(const Player &player)
{
    // Use the composite scoring system
    return GearScorer::CalculateGearLevel(player);
}

void GearLevelManager::FireChangeEvent(const GearLevelChangeEvent &event)
{
    for (const auto &pair : changeCallbacks) {
        pair.second(event);
    }
    
    // Log the change
    LogVerbose("Gear level changed for player {}: {:.2f} -> {:.2f} ({})",
        event.player->getId(), event.oldGearLevel, event.newGearLevel,
        event.eventType == GearLevelEventType::ItemEquipped ? "Item Equipped" :
        event.eventType == GearLevelEventType::ItemUnequipped ? "Item Unequipped" :
        event.eventType == GearLevelEventType::LevelUp ? "Level Up" : "Recalculated");
}

} // namespace devilution
