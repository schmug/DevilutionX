/**
 * @file drop_rate_validator.cpp
 *
 * Implementation of validation utilities for the item drop rate configuration system.
 */

#include "mods/config/drop_rate_validator.h"

#include <fstream>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include "utils/log.hpp"
#include "itemdat.h"

namespace devilution {

DropRateValidationResult DropRateValidator::ValidateConfigFile(const std::string& configPath)
{
    DropRateValidationResult result;
    result.isValid = true;
    
    try {
        // Try to open and parse the file
        std::ifstream configFile(configPath);
        if (!configFile.is_open()) {
            result.isValid = false;
            result.errors.push_back("Could not open configuration file: " + configPath);
            return result;
        }
        
        nlohmann::json configJson;
        try {
            configFile >> configJson;
        } catch (const nlohmann::json::parse_error& e) {
            result.isValid = false;
            result.errors.push_back("JSON parse error: " + std::string(e.what()));
            return result;
        }
        
        // Check for required fields
        if (!configJson.contains("maxDropRateMultiplier")) {
            result.warnings.push_back("Missing 'maxDropRateMultiplier' field, using default value");
        }
        
        if (!configJson.contains("minDropRateMultiplier")) {
            result.warnings.push_back("Missing 'minDropRateMultiplier' field, using default value");
        }
        
        if (!configJson.contains("modifiers")) {
            result.warnings.push_back("Missing 'modifiers' field, no modifiers will be applied");
        } else if (!configJson["modifiers"].is_array()) {
            result.isValid = false;
            result.errors.push_back("'modifiers' field must be an array");
        }
        
        // If we can parse the config, validate it
        if (result.isValid) {
            DropRateConfig config;
            config.LoadFromFile(configPath);
            auto configResult = ValidateConfig(config);
            
            result.isValid = configResult.isValid;
            result.errors.insert(result.errors.end(), configResult.errors.begin(), configResult.errors.end());
            result.warnings.insert(result.warnings.end(), configResult.warnings.begin(), configResult.warnings.end());
        }
    } catch (const std::exception& e) {
        result.isValid = false;
        result.errors.push_back("Exception while validating configuration: " + std::string(e.what()));
    }
    
    return result;
}

DropRateValidationResult DropRateValidator::ValidateConfig(const DropRateConfig& config)
{
    DropRateValidationResult result;
    result.isValid = true;
    
    // Check global settings
    if (config.GetMaxDropRateMultiplier() <= 0) {
        result.isValid = false;
        result.errors.push_back("maxDropRateMultiplier must be greater than 0");
    }
    
    if (config.GetMinDropRateMultiplier() <= 0) {
        result.isValid = false;
        result.errors.push_back("minDropRateMultiplier must be greater than 0");
    }
    
    if (config.GetMinDropRateMultiplier() > config.GetMaxDropRateMultiplier()) {
        result.isValid = false;
        result.errors.push_back("minDropRateMultiplier cannot be greater than maxDropRateMultiplier");
    }
    
    // Validate each modifier
    const auto& modifiers = config.GetModifiers();
    for (size_t i = 0; i < modifiers.size(); i++) {
        ValidateModifier(modifiers[i], i, result.errors, result.warnings);
    }
    
    // Check for balance issues
    CheckForBalanceIssues(config, result.warnings);
    
    // Check for duplicate modifiers
    std::unordered_map<std::string, std::vector<size_t>> modifiersByKey;
    for (size_t i = 0; i < modifiers.size(); i++) {
        const auto& modifier = modifiers[i];
        std::string key = std::to_string(static_cast<int>(modifier.scope)) + "_" + 
                         modifier.targetId + "_" + 
                         std::to_string(static_cast<int>(modifier.context));
        
        modifiersByKey[key].push_back(i);
    }
    
    for (const auto& [key, indices] : modifiersByKey) {
        if (indices.size() > 1) {
            std::string indexList;
            for (size_t i = 0; i < indices.size(); i++) {
                if (i > 0) indexList += ", ";
                indexList += std::to_string(indices[i]);
            }
            
            result.warnings.push_back("Potential duplicate modifiers found at indices: " + indexList);
        }
    }
    
    return result;
}

void DropRateValidator::ValidateModifier(
    const DropRateModifier& modifier,
    size_t index,
    std::vector<std::string>& errors,
    std::vector<std::string>& warnings)
{
    std::string indexStr = std::to_string(index);
    
    // Check scope and targetId
    if (modifier.scope != DropRateScope::Global && modifier.targetId.empty()) {
        errors.push_back("Modifier at index " + indexStr + " has non-global scope but empty targetId");
    }
    
    if (modifier.scope == DropRateScope::Global && !modifier.targetId.empty()) {
        warnings.push_back("Modifier at index " + indexStr + " has global scope but non-empty targetId, targetId will be ignored");
    }
    
    // Check if targetId is valid for the scope
    if (!modifier.targetId.empty() && !IsValidTargetId(modifier.scope, modifier.targetId)) {
        warnings.push_back("Modifier at index " + indexStr + " has targetId '" + modifier.targetId + 
                          "' which may not be valid for scope " + std::to_string(static_cast<int>(modifier.scope)));
    }
    
    // Check value based on modType
    if (modifier.modType == DropRateModType::Multiplier && modifier.value <= 0) {
        errors.push_back("Modifier at index " + indexStr + " has multiplier type but value <= 0");
    }
    
    if (modifier.modType == DropRateModType::SetAbsolute && modifier.value < 0) {
        errors.push_back("Modifier at index " + indexStr + " has setAbsolute type but negative value");
    }
    
    // Check for potential condition issues
    if (modifier.condition.difficulty.has_value() && 
        (modifier.condition.difficulty.value() < 0 || modifier.condition.difficulty.value() > 2)) {
        warnings.push_back("Modifier at index " + indexStr + " has difficulty condition outside range 0-2");
    }
    
    if (modifier.condition.minMonsterLevel.has_value() && modifier.condition.maxMonsterLevel.has_value() &&
        modifier.condition.minMonsterLevel.value() > modifier.condition.maxMonsterLevel.value()) {
        errors.push_back("Modifier at index " + indexStr + " has minMonsterLevel > maxMonsterLevel");
    }
    
    if (modifier.condition.minDungeonLevel.has_value() && modifier.condition.maxDungeonLevel.has_value() &&
        modifier.condition.minDungeonLevel.value() > modifier.condition.maxDungeonLevel.value()) {
        errors.push_back("Modifier at index " + indexStr + " has minDungeonLevel > maxDungeonLevel");
    }
}

void DropRateValidator::CheckForBalanceIssues(
    const DropRateConfig& config,
    std::vector<std::string>& warnings)
{
    const auto& modifiers = config.GetModifiers();
    
    // Check for very high multipliers
    for (size_t i = 0; i < modifiers.size(); i++) {
        const auto& modifier = modifiers[i];
        
        if (modifier.modType == DropRateModType::Multiplier && modifier.value > 5.0f) {
            warnings.push_back("Modifier at index " + std::to_string(i) + " has a very high multiplier (" + 
                              std::to_string(modifier.value) + "), which may cause balance issues");
        }
        
        if (modifier.modType == DropRateModType::AddFlat && modifier.value > 100.0f) {
            warnings.push_back("Modifier at index " + std::to_string(i) + " adds a very high flat value (" + 
                              std::to_string(modifier.value) + "), which may cause balance issues");
        }
        
        if (modifier.modType == DropRateModType::SetAbsolute && modifier.value > 200.0f) {
            warnings.push_back("Modifier at index " + std::to_string(i) + " sets an absolute value that is very high (" + 
                              std::to_string(modifier.value) + "), which may cause balance issues");
        }
    }
    
    // Check for potential stacking issues
    std::unordered_map<DropRateScope, float> maxMultiplierByScope;
    for (const auto& modifier : modifiers) {
        if (modifier.modType == DropRateModType::Multiplier) {
            float& maxMultiplier = maxMultiplierByScope[modifier.scope];
            maxMultiplier = std::max(maxMultiplier, modifier.value);
            
            if (modifier.scope == DropRateScope::Global && modifier.value > 2.0f) {
                warnings.push_back("Global multiplier of " + std::to_string(modifier.value) + 
                                  " may cause balance issues when combined with other modifiers");
            }
        }
    }
    
    // Check for potential conflicts between modifiers
    for (size_t i = 0; i < modifiers.size(); i++) {
        std::vector<DropRateModifier> otherModifiers;
        for (size_t j = 0; j < modifiers.size(); j++) {
            if (i != j) {
                otherModifiers.push_back(modifiers[j]);
            }
        }
        
        auto conflictWarning = CheckForConflicts(modifiers[i], otherModifiers);
        if (conflictWarning.has_value()) {
            warnings.push_back(conflictWarning.value());
        }
    }
}

bool DropRateValidator::IsValidTargetId(DropRateScope scope, const std::string& targetId)
{
    auto validIds = GetValidTargetIds(scope);
    return std::find(validIds.begin(), validIds.end(), targetId) != validIds.end();
}

std::vector<std::string> DropRateValidator::GetValidTargetIds(DropRateScope scope)
{
    std::vector<std::string> validIds;
    
    switch (scope) {
        case DropRateScope::ItemType: {
            // Add all item types
            validIds = {
                "Sword", "Axe", "Bow", "Club", "Staff", "Mace", "Shield", 
                "LightArmor", "HeavyArmor", "Helm", "Ring", "Amulet", "Potion"
            };
            break;
        }
        case DropRateScope::ItemClass: {
            // Add all item classes
            validIds = {"Weapon", "Armor", "Jewelry", "Misc", "Gold"};
            break;
        }
        case DropRateScope::ItemId: {
            // Add all item IDs
            for (size_t i = 0; i < AllItemsList.size(); i++) {
                validIds.push_back(std::to_string(i));
            }
            break;
        }
        case DropRateScope::UniqueItem:
        case DropRateScope::Global:
            // These scopes don't need targetId validation
            break;
    }
    
    return validIds;
}

std::optional<std::string> DropRateValidator::CheckForConflicts(
    const DropRateModifier& modifier,
    const std::vector<DropRateModifier>& otherModifiers)
{
    for (const auto& other : otherModifiers) {
        // Check for overriding modifiers
        if (modifier.priority < other.priority) {
            if (modifier.scope == other.scope && 
                modifier.targetId == other.targetId && 
                modifier.context == other.context &&
                modifier.modType == DropRateModType::SetAbsolute && 
                other.modType == DropRateModType::SetAbsolute) {
                
                return "Modifier with priority " + std::to_string(modifier.priority) + 
                       " may be overridden by modifier with priority " + std::to_string(other.priority);
            }
        }
        
        // Check for conflicting conditions
        bool hasConflictingConditions = false;
        
        if (modifier.condition.difficulty.has_value() && other.condition.difficulty.has_value() &&
            modifier.condition.difficulty.value() == other.condition.difficulty.value()) {
            hasConflictingConditions = true;
        }
        
        if (hasConflictingConditions && 
            modifier.scope == other.scope && 
            modifier.targetId == other.targetId && 
            modifier.context == other.context) {
            
            return "Modifier may conflict with another modifier with similar conditions";
        }
    }
    
    return std::nullopt;
}

} // namespace devilution
