/**
 * @file gear_manager.h
 *
 * Interface for the gear level manager that integrates the gear scoring system with the game.
 */
#pragma once

#include <cstdint>
#include <unordered_map>
#include <functional>
#include <optional>

#include "player.h"
#include "items.h"
#include "gear/composite_score.h"

namespace devilution {

/**
 * @brief Event type for gear level changes
 */
enum class GearLevelEventType {
    ItemEquipped,    // An item was equipped
    ItemUnequipped,  // An item was unequipped
    ItemChanged,     // An item was changed (e.g., upgraded)
    LevelUp,         // Player leveled up
    Recalculated     // Gear level was recalculated for any other reason
};

/**
 * @brief Structure for gear level change events
 */
struct GearLevelChangeEvent {
    Player *player;              // The player whose gear level changed
    float oldGearLevel;          // Previous gear level
    float newGearLevel;          // New gear level
    GearLevelEventType eventType; // Type of event that caused the change
    inv_body_loc slot;           // Slot that changed (for item events)
    const Item *item;            // Item involved (for item events)
};

/**
 * @brief Callback type for gear level change events
 */
using GearLevelChangeCallback = std::function<void(const GearLevelChangeEvent&)>;

/**
 * @brief Manager class for gear level calculations and caching
 */
class GearLevelManager {
public:
    /**
     * @brief Gets the singleton instance
     * @return Reference to the singleton instance
     */
    static GearLevelManager& GetInstance();
    
    /**
     * @brief Initializes the gear level manager
     */
    void Initialize();
    
    /**
     * @brief Gets the current gear level for a player
     * @param player The player to get the gear level for
     * @return The calculated gear level
     */
    float GetCurrentGearLevel(const Player &player);
    
    /**
     * @brief Gets the potential gear level if an item is equipped
     * @param player The player to calculate for
     * @param newItem The item to simulate equipping
     * @param slot The slot to equip the item in
     * @return The potential gear level
     */
    float GetPotentialGearLevel(const Player &player, const Item &newItem, inv_body_loc slot);
    
    /**
     * @brief Compares two items and determines which is better for a player
     * @param player The player to compare for
     * @param item1 First item to compare
     * @param item2 Second item to compare
     * @param slot The slot to compare for
     * @return Positive if item2 is better, negative if item1 is better, zero if equal
     */
    float CompareItems(const Player &player, const Item &item1, const Item &item2, inv_body_loc slot);
    
    /**
     * @brief Gets a text explanation of a player's gear level
     * @param player The player to explain
     * @return A text explanation of the gear level calculation
     */
    std::string GetGearLevelExplanation(const Player &player);
    
    /**
     * @brief Invalidates the cached gear level for a player
     * @param player The player to invalidate
     */
    void InvalidateCache(const Player &player);
    
    /**
     * @brief Registers a callback for gear level changes
     * @param callback The callback function to register
     * @return An ID that can be used to unregister the callback
     */
    uint32_t RegisterChangeCallback(GearLevelChangeCallback callback);
    
    /**
     * @brief Unregisters a callback for gear level changes
     * @param callbackId The ID of the callback to unregister
     * @return True if the callback was unregistered, false if not found
     */
    bool UnregisterChangeCallback(uint32_t callbackId);
    
    /**
     * @brief Notifies the manager that an item was equipped
     * @param player The player who equipped the item
     * @param item The item that was equipped
     * @param slot The slot the item was equipped in
     */
    void OnItemEquipped(Player &player, const Item &item, inv_body_loc slot);
    
    /**
     * @brief Notifies the manager that an item was unequipped
     * @param player The player who unequipped the item
     * @param slot The slot the item was unequipped from
     */
    void OnItemUnequipped(Player &player, inv_body_loc slot);
    
    /**
     * @brief Notifies the manager that a player leveled up
     * @param player The player who leveled up
     */
    void OnPlayerLevelUp(Player &player);
    
private:
    GearLevelManager() = default;
    ~GearLevelManager() = default;
    
    GearLevelManager(const GearLevelManager&) = delete;
    GearLevelManager& operator=(const GearLevelManager&) = delete;
    
    /**
     * @brief Calculates the gear level for a player
     * @param player The player to calculate for
     * @return The calculated gear level
     */
    float CalculateGearLevel(const Player &player);
    
    /**
     * @brief Fires a gear level change event
     * @param event The event to fire
     */
    void FireChangeEvent(const GearLevelChangeEvent &event);
    
    // Cache of gear levels by player ID
    std::unordered_map<uint8_t, float> gearLevelCache;
    
    // Callbacks for gear level changes
    std::unordered_map<uint32_t, GearLevelChangeCallback> changeCallbacks;
    uint32_t nextCallbackId = 1;
    
    // Flag to track initialization
    bool initialized = false;
};

} // namespace devilution
