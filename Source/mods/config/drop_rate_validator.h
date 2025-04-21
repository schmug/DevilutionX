/**
 * @file drop_rate_validator.h
 *
 * Validation utilities for the item drop rate configuration system.
 */

#pragma once

#include <string>
#include <vector>
#include <optional>

#include "mods/config/drop_rate_config.h"

namespace devilution {

/**
 * @brief Validation result for a drop rate configuration
 */
struct DropRateValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

/**
 * @brief Validator for drop rate configurations
 * 
 * This class provides methods to validate drop rate configurations
 * and identify potential issues before they cause problems in-game.
 */
class DropRateValidator {
public:
    /**
     * @brief Validate a drop rate configuration file
     * @param configPath Path to the configuration file
     * @return Validation result
     */
    static DropRateValidationResult ValidateConfigFile(const std::string& configPath);
    
    /**
     * @brief Validate a drop rate configuration
     * @param config The configuration to validate
     * @return Validation result
     */
    static DropRateValidationResult ValidateConfig(const DropRateConfig& config);
    
    /**
     * @brief Check if a target ID is valid for a given scope
     * @param scope The scope of the modifier
     * @param targetId The target ID to check
     * @return True if the target ID is valid for the scope
     */
    static bool IsValidTargetId(DropRateScope scope, const std::string& targetId);
    
    /**
     * @brief Get a list of valid target IDs for a given scope
     * @param scope The scope to get valid target IDs for
     * @return A vector of valid target IDs
     */
    static std::vector<std::string> GetValidTargetIds(DropRateScope scope);
    
    /**
     * @brief Check if a modifier could potentially conflict with other modifiers
     * @param modifier The modifier to check
     * @param otherModifiers Other modifiers to check against
     * @return An optional warning message if a conflict is detected
     */
    static std::optional<std::string> CheckForConflicts(
        const DropRateModifier& modifier,
        const std::vector<DropRateModifier>& otherModifiers);

private:
    /**
     * @brief Validate a single drop rate modifier
     * @param modifier The modifier to validate
     * @param index Index of the modifier in the configuration
     * @param errors Vector to add errors to
     * @param warnings Vector to add warnings to
     */
    static void ValidateModifier(
        const DropRateModifier& modifier,
        size_t index,
        std::vector<std::string>& errors,
        std::vector<std::string>& warnings);
    
    /**
     * @brief Check for potential balance issues in a configuration
     * @param config The configuration to check
     * @param warnings Vector to add warnings to
     */
    static void CheckForBalanceIssues(
        const DropRateConfig& config,
        std::vector<std::string>& warnings);
};

} // namespace devilution
