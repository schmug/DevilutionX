/**
 * @file drop_rate_config.cpp
 *
 * Implementation of the item drop rate configuration system.
 */

#include "mods/config/drop_rate_config.h"

#include <fstream>
#include <algorithm>
#include <string>
#include <sstream>
#include <nlohmann/json.hpp>

#include "utils/file_util.h"
#include "utils/str_cat.hpp"
#include "utils/log.hpp"
#include "utils/paths.h"
#include "diablo.h"

namespace devilution {

DropRateConfig& DropRateConfig::getInstance()
{
	// Use a local class to avoid the 'not a type name' error
	struct Local {
		static DropRateConfig& getInst() {
			static DropRateConfig instance;
			return instance;
		}
	};
	return Local::getInst();
}

DropRateConfig::DropRateConfig()
{
	// Initialize with default values
	ResetToDefault();
}

void DropRateConfig::ResetToDefault()
{
	_modifiers.clear();
	_maxDropRateMultiplier = 10.0f;
	_minDropRateMultiplier = 0.1f;
	
	// Add default modifiers if needed
	// For example, a small global boost to unique item drop rates
	DropRateModifier defaultUniqueBoost;
	defaultUniqueBoost.scope = DropRateScope::UniqueItem;
	defaultUniqueBoost.targetId = "";
	defaultUniqueBoost.context = DropRateContext::Always;
	defaultUniqueBoost.modType = DropRateModType::Multiplier;
	defaultUniqueBoost.value = 1.0f; // No change by default
	defaultUniqueBoost.priority = 0;
	
	_modifiers.push_back(defaultUniqueBoost);
}

bool DropRateConfig::LoadFromFile(const std::string& filePath, bool validate)
{
	try {
		std::ifstream configFile(filePath);
		if (!configFile.is_open()) {
			LogError("Failed to open drop rate config file: {}", filePath);
			return false;
		}
		
		// Parse JSON configuration
		try {
			// Use nlohmann/json library to parse the JSON file
			nlohmann::json configJson;
			configFile >> configJson;
			
			// Clear existing modifiers
			_modifiers.clear();
			
			// Load global settings
			if (configJson.contains("maxDropRateMultiplier")) {
				_maxDropRateMultiplier = configJson["maxDropRateMultiplier"].get<float>();
			}
			
			if (configJson.contains("minDropRateMultiplier")) {
				_minDropRateMultiplier = configJson["minDropRateMultiplier"].get<float>();
			}
			
			// Load modifiers
			if (configJson.contains("modifiers") && configJson["modifiers"].is_array()) {
				for (const auto& modJson : configJson["modifiers"]) {
					DropRateModifier modifier;
					
					// Parse scope
					if (modJson.contains("scope")) {
						std::string scopeStr = modJson["scope"].get<std::string>();
						if (scopeStr == "global") modifier.scope = DropRateScope::Global;
						else if (scopeStr == "itemType") modifier.scope = DropRateScope::ItemType;
						else if (scopeStr == "itemClass") modifier.scope = DropRateScope::ItemClass;
						else if (scopeStr == "itemId") modifier.scope = DropRateScope::ItemId;
						else if (scopeStr == "uniqueItem") modifier.scope = DropRateScope::UniqueItem;
					}
					
					// Parse targetId
					if (modJson.contains("targetId")) {
						modifier.targetId = modJson["targetId"].get<std::string>();
					}
					
					// Parse context
					if (modJson.contains("context")) {
						std::string contextStr = modJson["context"].get<std::string>();
						if (contextStr == "always") modifier.context = DropRateContext::Always;
						else if (contextStr == "monsterDrop") modifier.context = DropRateContext::MonsterDrop;
						else if (contextStr == "chestDrop") modifier.context = DropRateContext::ChestDrop;
						else if (contextStr == "groundDrop") modifier.context = DropRateContext::GroundDrop;
						else if (contextStr == "questReward") modifier.context = DropRateContext::QuestReward;
					}
					
					// Parse modType
					if (modJson.contains("modType")) {
						std::string modTypeStr = modJson["modType"].get<std::string>();
						if (modTypeStr == "multiplier") modifier.modType = DropRateModType::Multiplier;
						else if (modTypeStr == "addFlat") modifier.modType = DropRateModType::AddFlat;
						else if (modTypeStr == "setAbsolute") modifier.modType = DropRateModType::SetAbsolute;
					}
					
					// Parse value
					if (modJson.contains("value")) {
						modifier.value = modJson["value"].get<float>();
					}
					
					// Parse priority
					if (modJson.contains("priority")) {
						modifier.priority = modJson["priority"].get<int>();
					}
					
					// Parse description
					if (modJson.contains("description")) {
						modifier.description = modJson["description"].get<std::string>();
					}
					
					// Parse conditions if present
					if (modJson.contains("conditions")) {
						auto& condJson = modJson["conditions"];
						if (condJson.contains("minMonsterLevel")) {
							modifier.condition.minMonsterLevel = condJson["minMonsterLevel"].get<int>();
						}
						if (condJson.contains("maxMonsterLevel")) {
							modifier.condition.maxMonsterLevel = condJson["maxMonsterLevel"].get<int>();
						}
						if (condJson.contains("minDungeonLevel")) {
							modifier.condition.minDungeonLevel = condJson["minDungeonLevel"].get<int>();
						}
						if (condJson.contains("maxDungeonLevel")) {
							modifier.condition.maxDungeonLevel = condJson["maxDungeonLevel"].get<int>();
						}
						if (condJson.contains("difficulty")) {
							modifier.condition.difficulty = condJson["difficulty"].get<int>();
						}
						if (condJson.contains("multiplayerOnly")) {
							modifier.condition.multiplayerOnly = condJson["multiplayerOnly"].get<bool>();
						}
					}
					
					// Add the modifier to our list
					_modifiers.push_back(modifier);
				}
			}
			
			// Log successful loading
			LogInfo("Loaded {} drop rate modifiers from {}", _modifiers.size(), filePath);
			
		} catch (const nlohmann::json::parse_error& e) {
			LogError("JSON parse error in drop rate config: {}", e.what());
			return false;
		} catch (const std::exception& e) {
			LogError("Error parsing drop rate JSON config: {}", e.what());
			return false;
		}
		
		// If validation is requested, validate the loaded configuration
		if (validate) {
			auto validationResult = Validate();
			if (!validationResult.isValid) {
				LogError("Drop rate configuration validation failed: {}", validationResult.errorMessage);
				return false;
			}
		}
		
		return true;
	} catch (const std::exception& e) {
		LogError("Exception while loading drop rate config: {}", e.what());
		return false;
	}
}

float DropRateConfig::GetModifiedDropRate(const ItemData& item, DropRateContext context, int monsterLevel, int dungeonLevel, bool isUniqueItem) const
{
	float baseRate = static_cast<float>(item.dropRate);
	float modifiedRate = baseRate;
	
	// Apply all applicable modifiers in order of priority
	for (const auto& modifier : _modifiers) {
		if (modifier.AppliesToContext(context) && 
		    modifier.AppliesTo(item._itype, item._iClass, static_cast<_item_indexes>(item.iItemId), isUniqueItem) && 
		    modifier.condition.Evaluate(monsterLevel, dungeonLevel, 0, gbIsMultiplayer)) {
			modifiedRate = ApplyModifier(modifier, modifiedRate);
		}
	}
	
	// Apply safeguards
	if (baseRate > 0) { // Only apply safeguards to items that can actually drop
		float multiplier = modifiedRate / baseRate;
		multiplier = std::max(_minDropRateMultiplier, std::min(_maxDropRateMultiplier, multiplier));
		modifiedRate = baseRate * multiplier;
	}
	
	// Ensure drop rate is never negative
	return std::max(0.0f, modifiedRate);
}

DropRateConfig::DropRateValidationResult DropRateConfig::Validate() const
{
	DropRateValidationResult result;
	result.isValid = true;
	
	// Check for valid multiplier ranges
	if (_maxDropRateMultiplier <= 0.0f) {
		result.isValid = false;
		result.errorMessage = "Maximum drop rate multiplier must be greater than 0";
		return result;
	}
	
	if (_minDropRateMultiplier <= 0.0f) {
		result.isValid = false;
		result.errorMessage = "Minimum drop rate multiplier must be greater than 0";
		return result;
	}
	
	if (_minDropRateMultiplier > _maxDropRateMultiplier) {
		result.isValid = false;
		result.errorMessage = "Minimum drop rate multiplier cannot be greater than maximum";
		return result;
	}
	
	// Check for valid modifiers
	for (const auto& modifier : _modifiers) {
		// Check for valid scope
		if (modifier.scope == DropRateScope::ItemId && modifier.targetId.empty()) {
			result.isValid = false;
			result.errorMessage = "Item ID scope requires a target ID";
			return result;
		}
		
		// Check for valid modifier value
		if (modifier.modType == DropRateModType::Multiplier && modifier.value <= 0.0f) {
			result.isValid = false;
			result.errorMessage = "Multiplier value must be greater than 0";
			return result;
		}
	}
	
	return result;
}

bool DropRateModifier::AppliesToContext(DropRateContext currentContext) const
{
	return context == DropRateContext::Always || context == currentContext;
}

float DropRateModifier::ApplyModifier(float baseDropRate) const
{
	switch (modType) {
		case DropRateModType::Multiplier:
			return baseDropRate * value;
			
		case DropRateModType::AddFlat:
			return baseDropRate + value;
			
		case DropRateModType::SetAbsolute:
			return value;
			
		default:
			return baseDropRate;
	}
}

float DropRateConfig::ApplyModifier(const DropRateModifier& modifier, float currentRate) const
{
	return modifier.ApplyModifier(currentRate);
}

bool DropRateConfig::SaveToFile(const std::string& filePath) const
{
	try {
		std::ofstream configFile(filePath);
		if (!configFile.is_open()) {
			LogError("Failed to open drop rate config file for writing: {}", filePath);
			return false;
		}
		
		// Create JSON object
		nlohmann::json configJson;
		
		// Add global settings
		configJson["maxDropRateMultiplier"] = _maxDropRateMultiplier;
		configJson["minDropRateMultiplier"] = _minDropRateMultiplier;
		
		// Add modifiers array
		nlohmann::json modifiersArray = nlohmann::json::array();
		
		for (const auto& mod : _modifiers) {
			nlohmann::json modJson;
			
			// Convert scope to string
			switch (mod.scope) {
			case DropRateScope::Global:
				modJson["scope"] = "global";
				break;
			case DropRateScope::ItemType:
				modJson["scope"] = "itemType";
				break;
			case DropRateScope::ItemClass:
				modJson["scope"] = "itemClass";
				break;
			case DropRateScope::ItemId:
				modJson["scope"] = "itemId";
				break;
			case DropRateScope::UniqueItem:
				modJson["scope"] = "uniqueItem";
				break;
			}
			
			// Add targetId if not empty
			if (!mod.targetId.empty()) {
				modJson["targetId"] = mod.targetId;
			}
			
			// Convert context to string
			switch (mod.context) {
			case DropRateContext::Always:
				modJson["context"] = "always";
				break;
			case DropRateContext::MonsterDrop:
				modJson["context"] = "monsterDrop";
				break;
			case DropRateContext::ChestDrop:
				modJson["context"] = "chestDrop";
				break;
			case DropRateContext::GroundDrop:
				modJson["context"] = "groundDrop";
				break;
			case DropRateContext::QuestReward:
				modJson["context"] = "questReward";
				break;
			}
			
			// Convert modType to string
			switch (mod.modType) {
			case DropRateModType::Multiplier:
				modJson["modType"] = "multiplier";
				break;
			case DropRateModType::AddFlat:
				modJson["modType"] = "addFlat";
				break;
			case DropRateModType::SetAbsolute:
				modJson["modType"] = "setAbsolute";
				break;
			}
			
			// Add value and priority
			modJson["value"] = mod.value;
			modJson["priority"] = mod.priority;
			
			// Add description if not empty
			if (!mod.description.empty()) {
				modJson["description"] = mod.description;
			}
			
			// Add conditions if any are set
			nlohmann::json condJson;
			bool hasConditions = false;
			
			if (mod.condition.minMonsterLevel.has_value()) {
				condJson["minMonsterLevel"] = mod.condition.minMonsterLevel.value();
				hasConditions = true;
			}
			
			if (mod.condition.maxMonsterLevel.has_value()) {
				condJson["maxMonsterLevel"] = mod.condition.maxMonsterLevel.value();
				hasConditions = true;
			}
			
			if (mod.condition.minDungeonLevel.has_value()) {
				condJson["minDungeonLevel"] = mod.condition.minDungeonLevel.value();
				hasConditions = true;
			}
			
			if (mod.condition.maxDungeonLevel.has_value()) {
				condJson["maxDungeonLevel"] = mod.condition.maxDungeonLevel.value();
				hasConditions = true;
			}
			
			if (mod.condition.difficulty.has_value()) {
				condJson["difficulty"] = mod.condition.difficulty.value();
				hasConditions = true;
			}
			
			if (mod.condition.multiplayerOnly.has_value()) {
				condJson["multiplayerOnly"] = mod.condition.multiplayerOnly.value();
				hasConditions = true;
			}
			
			if (hasConditions) {
				modJson["conditions"] = condJson;
			}
			
			// Add this modifier to the array
			modifiersArray.push_back(modJson);
		}
		
		// Add modifiers array to config
		configJson["modifiers"] = modifiersArray;
		
		// Write formatted JSON to file
		configFile << configJson.dump(4);
		configFile.close();
		
		LogInfo("Saved drop rate configuration to {}", filePath);
		return true;
	} catch (const std::exception& e) {
		LogError("Exception while saving drop rate config: {}", e.what());
		return false;
	}
}

bool DropRateConfig::CreateDefaultConfig(const std::string& filePath) const
{
	// Create a default configuration file
	try {
		std::ofstream configFile(filePath);
		if (!configFile.is_open()) {
			LogError("Failed to create default drop rate config file: {}", filePath);
			return false;
		}
		
		// Create a default JSON configuration
		nlohmann::json configJson;
		
		// Add global settings
		configJson["maxDropRateMultiplier"] = 10.0f;
		configJson["minDropRateMultiplier"] = 0.1f;
		
		// Create default modifiers
		nlohmann::json modifiersArray = nlohmann::json::array();
		
		// Example 1: Global unique item boost
		nlohmann::json uniqueBoost;
		uniqueBoost["scope"] = "uniqueItem";
		uniqueBoost["context"] = "always";
		uniqueBoost["modType"] = "multiplier";
		uniqueBoost["value"] = 1.5f;
		uniqueBoost["priority"] = 10;
		uniqueBoost["description"] = "Global boost to unique item drop rates";
		modifiersArray.push_back(uniqueBoost);
		
		// Example 2: Hell difficulty boost
		nlohmann::json hellBoost;
		hellBoost["scope"] = "global";
		hellBoost["context"] = "monsterDrop";
		hellBoost["modType"] = "multiplier";
		hellBoost["value"] = 1.25f;
		hellBoost["priority"] = 5;
		hellBoost["description"] = "Increased drops in Hell difficulty";
		
		// Add condition for Hell difficulty
		nlohmann::json hellCondition;
		hellCondition["difficulty"] = 2; // Assuming 2 is Hell difficulty
		hellBoost["conditions"] = hellCondition;
		
		modifiersArray.push_back(hellBoost);
		
		// Add modifiers array to config
		configJson["modifiers"] = modifiersArray;
		
		// Write formatted JSON to file
		configFile << configJson.dump(4);
		configFile.close();
		
		LogInfo("Created default drop rate configuration at {}", filePath);
		return true;
	} catch (const std::exception& e) {
		LogError("Exception while creating default drop rate config: {}", e.what());
		return false;
	}
}

bool DropRateModifier::AppliesTo(ItemType itemType, item_class itemClass, _item_indexes itemId, bool isUnique) const
{
	switch (scope) {
		case DropRateScope::Global:
			return true;
			
		case DropRateScope::ItemType:
			return targetId == std::to_string(static_cast<int>(itemType));
			
		case DropRateScope::ItemClass:
			return targetId == std::to_string(static_cast<int>(itemClass));
			
		case DropRateScope::ItemId:
			return targetId == std::to_string(static_cast<int>(itemId));
			
		case DropRateScope::UniqueItem:
			return isUnique;
			
		default:
			return false;
	}
}

bool DropRateCondition::Evaluate(int monsterLevel, int dungeonLevel, int gameDifficulty, bool isMultiplayer) const
{
	// Check monster level conditions
	if (minMonsterLevel.has_value() && monsterLevel < minMonsterLevel.value()) {
		return false;
	}
	
	if (maxMonsterLevel.has_value() && monsterLevel > maxMonsterLevel.value()) {
		return false;
	}
	
	// Check dungeon level conditions
	if (minDungeonLevel.has_value() && dungeonLevel < minDungeonLevel.value()) {
		return false;
	}
	
	if (maxDungeonLevel.has_value() && dungeonLevel > maxDungeonLevel.value()) {
		return false;
	}
	
	// Check difficulty condition
	if (difficulty.has_value() && gameDifficulty != difficulty.value()) {
		return false;
	}
	
	// Check multiplayer condition
	if (multiplayerOnly.has_value() && multiplayerOnly.value() && !isMultiplayer) {
		return false;
	}
	
	// All conditions passed
	return true;
}

} // namespace devilution
