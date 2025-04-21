/**
 * @file drop_rate_modifier.cpp
 *
 * Implementation of the item drop rate modification interface.
 */

#include "mods/drop_rate_modifier.h"

#include "utils/paths.h"
#include "utils/log.hpp"
#include "utils/file_util.h"
#include "diablo.h"
#include "mods/config/drop_rate_validator.h"

namespace devilution {

DropRateManager& DropRateManager::getInstance()
{
	static DropRateManager instance;
	return instance;
}

bool DropRateManager::LoadConfig(const std::string& configPath)
{
	LogVerbose("Loading drop rate configuration from: {}", configPath);
	
	// Validate the configuration before loading
	auto validationResult = DropRateValidator::ValidateConfigFile(configPath);
	if (!validationResult.isValid) {
		LogError("Drop rate configuration validation failed:");
		for (const auto& error : validationResult.errors) {
			LogError("  Error: {}", error);
		}
		return false;
	}
	
	// Load the configuration
	return config.LoadFromFile(configPath);
}

bool DropRateManager::ReloadConfig(const std::string& configPath)
{
	// If no config path is provided, use the last loaded config path
	std::string path = configPath;
	if (path.empty()) {
		path = paths::PrefPath() + "mods/config/drop_rate_config.json";
	}
	
	LogVerbose("Reloading drop rate configuration from: {}", path);
	
	// Check if the file exists
	if (!FileExists(path)) {
		LogError("Drop rate configuration file not found: {}", path);
		return false;
	}
	
	// Validate the configuration before reloading
	auto validationResult = DropRateValidator::ValidateConfigFile(path);
	if (!validationResult.isValid) {
		LogError("Drop rate configuration validation failed:");
		for (const auto& error : validationResult.errors) {
			LogError("  Error: {}", error);
		}
		
		if (!validationResult.warnings.empty()) {
			LogWarning("Drop rate configuration has warnings:");
			for (const auto& warning : validationResult.warnings) {
				LogWarning("  Warning: {}", warning);
			}
		}
		
		return false;
	} else if (!validationResult.warnings.empty()) {
		LogWarning("Drop rate configuration has warnings:");
		for (const auto& warning : validationResult.warnings) {
			LogWarning("  Warning: {}", warning);
		}
	}
	
	// Reload the configuration
	return config.LoadFromFile(path);
}

float DropRateManager::GetMonsterDropRate(const ItemData& item, int monsterLevel, bool isUnique) const
{
	int dungeonLevel = ItemsGetCurrlevel();
	return config.GetModifiedDropRate(item, DropRateContext::MonsterDrop, monsterLevel, dungeonLevel, isUnique);
}

float DropRateManager::GetChestDropRate(const ItemData& item, int dungeonLevel, bool isUnique) const
{
	return config.GetModifiedDropRate(item, DropRateContext::ChestDrop, 0, dungeonLevel, isUnique);
}

float DropRateManager::GetGroundDropRate(const ItemData& item, int dungeonLevel, bool isUnique) const
{
	return config.GetModifiedDropRate(item, DropRateContext::GroundDrop, 0, dungeonLevel, isUnique);
}

float DropRateManager::GetQuestRewardDropRate(const ItemData& item, int dungeonLevel, bool isUnique) const
{
	return config.GetModifiedDropRate(item, DropRateContext::QuestReward, 0, dungeonLevel, isUnique);
}

} // namespace devilution