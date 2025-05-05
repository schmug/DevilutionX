/**
 * @file drop_rate_modifier.h
 *
 * Interface for the item drop rate modification system.
 */

#pragma once

#include "itemdat.h"
#include "mods/config/drop_rate_config.h"
#include "utils/log.hpp"

namespace devilution {

/**
 * @brief Interface for modifying item drop rates
 * 
 * This class provides methods to integrate the drop rate configuration
 * with the existing item generation system.
 */
class DropRateManager {
public:
	// Item type enum for drop rate settings
	enum class ItemType {
		Normal = 0,
		Magic = 1,
		Rare = 2,
		Unique = 3,
		Max = 4
	};

	// Ensure this is the only implementation of getInstance() in the codebase
	// This is declared inline to prevent multiple definitions
	static inline DropRateManager& getInstance() {
		static DropRateManager instance;
		return instance;
	}
	
	/**
	 * @brief Get the current gold drop rate percentage
	 * @return The gold drop rate percentage (0-100)
	 */
	int GetGoldDropRatePercent() const { return goldDropRatePercent; }
	
	/**
	 * @brief Set the gold drop rate percentage
	 * @param percent The gold drop rate percentage (0-100)
	 */
	void SetGoldDropRatePercent(int percent)
	{
		// Clamp the value to 0-100 range
		if (percent < 0) percent = 0;
		if (percent > 100) percent = 100;
		
		// Set the value
		goldDropRatePercent = percent;
		
		// Log the change
		LogVerbose("Gold drop rate set to {}%", goldDropRatePercent);
		
		// Note: We've removed the SaveSettings call for now to focus on core functionality
	}
	
	/**
	 * @brief Get the current gold amount percentage
	 * @return The gold amount percentage (0-100)
	 */
	int GetGoldAmountPercent() const { return goldAmountPercent; }
	
	/**
	 * @brief Set the gold amount percentage
	 * @param percent The gold amount percentage (0-100)
	 */
	void SetGoldAmountPercent(int percent)
	{
		// Clamp the value to 0-100 range
		if (percent < 0) percent = 0;
		if (percent > 100) percent = 100;
		
		// Set the value
		goldAmountPercent = percent;
		
		// Log the change
		LogVerbose("Gold amount set to {}%", goldAmountPercent);
		
		// Note: We've removed the SaveSettings call for now to focus on core functionality
	}
	
	/**
	 * @brief Get the current item drop rate percentage
	 * @return The item drop rate percentage (0-100)
	 */
	int GetItemDropRatePercent() const { return itemDropRatePercent; }
	
	/**
	 * @brief Set the item drop rate percentage
	 * @param percent The item drop rate percentage (0-100)
	 */
	void SetItemDropRatePercent(int percent)
	{
		// Clamp the value to 0-100 range
		if (percent < 0) percent = 0;
		if (percent > 100) percent = 100;
		
		// Set the value
		itemDropRatePercent = percent;
		
		// Log the change
		LogVerbose("Item drop rate set to {}%", itemDropRatePercent);
	}
	
	/**
	 * @brief Get the current item type preference
	 * @return The item type preference (0-3)
	 */
	ItemType GetItemTypePreference() const { return itemTypePreference; }
	
	/**
	 * @brief Set the item type preference
	 * @param type The item type preference (0-3)
	 */
	void SetItemTypePreference(ItemType type)
	{
		itemTypePreference = type;
		LogVerbose("Item type preference set to {}", static_cast<int>(itemTypePreference));
	}
	
	/**
	 * @brief Get the current item quality percentage
	 * @return The item quality percentage (0-100)
	 */
	int GetItemQualityPercent() const { return itemQualityPercent; }
	
	/**
	 * @brief Set the item quality percentage
	 * @param percent The item quality percentage (0-100)
	 */
	void SetItemQualityPercent(int percent)
	{
		// Clamp the value to 0-100 range
		if (percent < 0) percent = 0;
		if (percent > 100) percent = 100;
		
		// Set the value
		itemQualityPercent = percent;
		
		// Log the change
		LogVerbose("Item quality set to {}%", itemQualityPercent);
	}
	
	/**
	 * @brief Get the current special object quality scaling factor percentage
	 * @return The special object quality scaling factor percentage (0-100)
	 */
	int GetSpecialObjectQualityScalingFactor() const { return specialObjectQualityScalingFactor; }
	
	/**
	 * @brief Set the special object quality scaling factor percentage
	 * @param factor The special object quality scaling factor percentage (0-100)
	 */
	void SetSpecialObjectQualityScalingFactor(int factor)
	{
		// Clamp the value to 0-100 range
		if (factor < 0) factor = 0;
		if (factor > 100) factor = 100;
		
		// Set the value
		specialObjectQualityScalingFactor = factor;
		
		// Log the change
		LogVerbose("Special object quality scaling factor set to {}%", specialObjectQualityScalingFactor);
	}
	
	/**
	 * @brief Reset all drop rates to default values
	 */
	void ResetDropRatesToDefaults()
	{
		// Reset gold drop rate to default
		goldDropRatePercent = 20;
		
		// Reset gold amount to default
		goldAmountPercent = 100;
		
		// Reset item drop rate to default
		itemDropRatePercent = 60;
		
		// Reset item type preference to default
		itemTypePreference = ItemType::Normal;
		
		// Reset item quality to default
		itemQualityPercent = 50;
		
		// Reset special object quality scaling factor to default
		specialObjectQualityScalingFactor = 25;
		
		LogVerbose("Drop rates reset to defaults");
		LogVerbose("Gold drop rate: {}%", goldDropRatePercent);
		LogVerbose("Gold amount: {}%", goldAmountPercent);
		LogVerbose("Item drop rate: {}%", itemDropRatePercent);
		LogVerbose("Item type preference: {}", static_cast<int>(itemTypePreference));
		LogVerbose("Item quality: {}%", itemQualityPercent);
		LogVerbose("Special object quality scaling factor: {}%", specialObjectQualityScalingFactor);
		
		// Note: We've removed the SaveSettings call for now to focus on core functionality
	}
	
	// We're not implementing SaveSettings and LoadSettings for now to focus on the core functionality
	
	/**
	 * @brief Load the drop rate configuration from a file
	 * @param configPath Path to the drop rate configuration file
	 * @return True if the configuration was loaded successfully
	 */
	bool LoadConfig(const std::string& configPath);

	/**
	 * @brief Reload the drop rate configuration from a file
	 * @param configPath Path to the drop rate configuration file
	 * @return True if the configuration was reloaded successfully
	 */
	bool ReloadConfig(const std::string& configPath = "");

	/**
	 * @brief Get the modified drop rate for an item in the context of monster drops
	 * @param item The item data
	 * @param monsterLevel The level of the monster dropping the item
	 * @param isUnique Whether the item is unique
	 * @return The modified drop rate
	 */
	float GetMonsterDropRate(const ItemData& item, int monsterLevel, bool isUnique) const;
	
	/**
	 * @brief Get the modified drop rate for an item in the context of chest drops
	 * @param item The item data
	 * @param dungeonLevel The current dungeon level
	 * @param isUnique Whether the item is unique
	 * @return The modified drop rate
	 */
	float GetChestDropRate(const ItemData& item, int dungeonLevel, bool isUnique) const;
	
	/**
	 * @brief Get the modified drop rate for an item in the context of ground spawns
	 * @param item The item data
	 * @param dungeonLevel The current dungeon level
	 * @param isUnique Whether the item is unique
	 * @return The modified drop rate
	 */
	float GetGroundDropRate(const ItemData& item, int dungeonLevel, bool isUnique) const;
	
	/**
	 * @brief Get the modified drop rate for an item in the context of quest rewards
	 * @param item The item data
	 * @param dungeonLevel The current dungeon level
	 * @param isUnique Whether the item is unique
	 * @return The modified drop rate
	 */
	float GetQuestRewardDropRate(const ItemData& item, int dungeonLevel, bool isUnique) const;


private:
	DropRateManager()
	{
		LogVerbose("Drop rate manager initializing with default settings");
		LogVerbose("Default gold drop rate: {}%", goldDropRatePercent);
	}
	
	DropRateConfig& config = DropRateConfig::getInstance();
	
	// Dynamic drop rate settings
	int goldDropRatePercent = 20; // Default 20% gold drop rate
	int goldAmountPercent = 100; // Default 100% gold amount
	int itemDropRatePercent = 60; // Default 60% item drop rate
	ItemType itemTypePreference = ItemType::Normal; // Default normal item type preference
	int itemQualityPercent = 50; // Default 50% item quality
	int specialObjectQualityScalingFactor = 25; // Default 25% special object quality scaling factor
	
	// Configuration loading and saving
	bool LoadConfig(const std::string& configPath, DropRateConfig& config);
	
	// We're not implementing persistence methods for now to focus on the core functionality
};

} // namespace devilution