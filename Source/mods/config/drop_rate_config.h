#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include <memory>

#include "itemdat.h"
#include "utils/log.hpp"

namespace devilution {

/**
 * @brief Defines the scope of a drop rate modifier
 */
enum class DropRateScope {
	Global,     // Applies to all items
	ItemType,   // Applies to a specific item type (e.g., Sword, Bow)
	ItemClass,  // Applies to a specific item class (e.g., Weapon, Armor)
	ItemId,     // Applies to a specific item ID
	UniqueItem, // Applies to unique items
};

/**
 * @brief Defines when a drop rate modifier is applied
 */
enum class DropRateContext {
	Always,           // Always apply this modifier
	MonsterDrop,      // Apply only when monsters drop items
	ChestDrop,        // Apply only when items come from chests
	GroundDrop,       // Apply only when items are generated on the ground
	QuestReward,      // Apply only for quest rewards
	SpecialObjectDrop // Apply only for special objects (bookcases, weapon racks, etc.)
};

/**
 * @brief Defines the type of drop rate modification
 */
enum class DropRateModType {
	Multiplier,   // Multiply the base drop rate by a factor
	AddFlat,      // Add a flat value to the base drop rate
	SetAbsolute,  // Set the drop rate to a specific value
};

/**
 * @brief Defines a condition for when a drop rate modifier applies
 */
struct DropRateCondition {
	std::optional<int> minMonsterLevel;   // Minimum monster level
	std::optional<int> maxMonsterLevel;   // Maximum monster level
	std::optional<int> minDungeonLevel;   // Minimum dungeon level
	std::optional<int> maxDungeonLevel;   // Maximum dungeon level
	std::optional<int> difficulty;        // Game difficulty (0=Normal, 1=Nightmare, 2=Hell)
	std::optional<bool> multiplayerOnly;  // Only apply in multiplayer games

	bool Evaluate(int monsterLevel, int dungeonLevel, int gameDifficulty, bool isMultiplayer) const;
};

/**
 * @brief Defines a single drop rate modifier
 */
struct DropRateModifier {
	DropRateScope scope;                 // What items this applies to
	std::string targetId;                // ID of the target (item type, class, or specific ID)
	DropRateContext context;             // When this modifier is applied
	DropRateModType modType;             // How the drop rate is modified
	float value;                         // The value to use for modification
	int priority;                        // Higher priority modifiers are applied first
	DropRateCondition condition;         // Conditions for this modifier to apply
	std::string description;             // Description of the modifier

	bool AppliesTo(ItemType itemType, item_class itemClass, _item_indexes itemId, bool isUnique) const;
	bool AppliesToContext(DropRateContext currentContext) const;
	float ApplyModifier(float baseDropRate) const;
};

/**
 * @brief Configuration for the drop rate modification system
 */
class DropRateConfig {
public:
	static DropRateConfig& getInstance();

	/**
	 * @brief Load drop rate configuration from a file
	 * @param filePath Path to the configuration file
	 * @param validate Whether to validate the configuration
	 * @return True if the configuration was loaded successfully
	 */
	bool LoadFromFile(const std::string& filePath, bool validate = true);

	/**
	 * @brief Save drop rate configuration to a file
	 * @param filePath Path to save the configuration to
	 * @return True if the configuration was saved successfully
	 */
	bool SaveToFile(const std::string& filePath) const;

	/**
	 * @brief Create a default configuration file
	 * @param filePath Path to create the configuration file at
	 * @return True if the file was created successfully
	 */
	bool CreateDefaultConfig(const std::string& filePath) const;

	/**
	 * @brief Get the modified drop rate for an item
	 * @param item The item to get the drop rate for
	 * @param context The context in which the item is being dropped
	 * @param monsterLevel The level of the monster dropping the item
	 * @param dungeonLevel The level of the dungeon
	 * @param isUniqueItem Whether the item is unique
	 * @return The modified drop rate
	 */
	float GetModifiedDropRate(const ItemData& item, DropRateContext context, int monsterLevel, int dungeonLevel, bool isUniqueItem) const;

	/**
	 * @brief Validate the current configuration
	 * @return Validation result
	 */
	struct DropRateValidationResult {
		bool isValid;
		std::string errorMessage;
	};
	DropRateValidationResult Validate() const;

	// Getters
	float GetMaxDropRateMultiplier() const { return _maxDropRateMultiplier; }
	float GetMinDropRateMultiplier() const { return _minDropRateMultiplier; }
	const std::vector<DropRateModifier>& GetModifiers() const { return _modifiers; }

	// Setters for testing
	void SetMaxDropRateMultiplier(float value) { _maxDropRateMultiplier = value; }
	void SetMinDropRateMultiplier(float value) { _minDropRateMultiplier = value; }
	void SetModifiers(const std::vector<DropRateModifier>& value) { _modifiers = value; }
	void AddModifier(const DropRateModifier& modifier) { _modifiers.push_back(modifier); }
	void ClearModifiers() { _modifiers.clear(); }
	
	/**
	 * @brief Reset configuration to default values
	 */
	void ResetToDefault();

private:
	DropRateConfig();

	std::vector<DropRateModifier> _modifiers;
	float _maxDropRateMultiplier = 10.0f;  // Maximum allowed multiplier (safeguard)
	float _minDropRateMultiplier = 0.1f;   // Minimum allowed multiplier (safeguard)

	// Helper methods for loading config
	bool ParseModifierFromJson(const std::string& jsonString, DropRateModifier& modifier);
	
	/**
	 * @brief Apply a modifier to a drop rate
	 * @param modifier The modifier to apply
	 * @param currentRate The current drop rate
	 * @return The modified drop rate
	 */
	float ApplyModifier(const DropRateModifier& modifier, float currentRate) const;
};

} // namespace devilution
