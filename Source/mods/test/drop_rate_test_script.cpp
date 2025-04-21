/**
 * @file drop_rate_test_script.cpp
 *
 * Test script for the item drop rate modification system.
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <fstream>

#include "utils/log.hpp"
#include "utils/paths.h"
#include "mods/config/drop_rate_config.h"
#include "mods/config/drop_rate_validator.h"
#include "mods/drop_rate_modifier.h"
#include "mods/drop_rate_test.h"

namespace devilution {

/**
 * @brief Test the drop rate validation system
 * @param testDir Directory to store test files
 * @return True if all tests pass
 */
bool TestValidation(const std::string& testDir)
{
    std::cout << "\n=== Testing Validation System ===\n";
    
    // Create test directory if it doesn't exist
    if (!std::filesystem::exists(testDir)) {
        std::filesystem::create_directories(testDir);
    }
    
    bool allTestsPassed = true;
    
    // Test 1: Valid configuration
    {
        std::cout << "Test 1: Valid configuration... ";
        
        std::string validConfigPath = testDir + "/valid_config.json";
        
        // Create a valid configuration
        DropRateConfig validConfig;
        validConfig.SetMaxDropRateMultiplier(10.0f);
        validConfig.SetMinDropRateMultiplier(0.1f);
        
        // Add a valid modifier
        DropRateModifier validModifier;
        validModifier.scope = DropRateScope::Global;
        validModifier.targetId = "";
        validModifier.context = DropRateContext::Always;
        validModifier.modType = DropRateModType::Multiplier;
        validModifier.value = 1.5f;
        validModifier.priority = 0;
        validModifier.description = "Global 50% increase";
        
        validConfig.AddModifier(validModifier);
        validConfig.SaveToFile(validConfigPath);
        
        // Validate the configuration
        auto result = DropRateValidator::ValidateConfigFile(validConfigPath);
        
        if (result.isValid && result.errors.empty()) {
            std::cout << "PASSED\n";
        } else {
            std::cout << "FAILED\n";
            for (const auto& error : result.errors) {
                std::cout << "  Error: " << error << "\n";
            }
            allTestsPassed = false;
        }
    }
    
    // Test 2: Invalid configuration (negative multiplier)
    {
        std::cout << "Test 2: Invalid configuration (negative multiplier)... ";
        
        std::string invalidConfigPath = testDir + "/invalid_config_negative.json";
        
        // Create an invalid configuration with negative multiplier
        std::ofstream configFile(invalidConfigPath);
        configFile << "{\n"
                   << "  \"maxDropRateMultiplier\": 10.0,\n"
                   << "  \"minDropRateMultiplier\": 0.1,\n"
                   << "  \"modifiers\": [\n"
                   << "    {\n"
                   << "      \"scope\": \"global\",\n"
                   << "      \"modType\": \"multiplier\",\n"
                   << "      \"value\": -1.5,\n"
                   << "      \"context\": \"always\",\n"
                   << "      \"priority\": 0\n"
                   << "    }\n"
                   << "  ]\n"
                   << "}\n";
        configFile.close();
        
        // Validate the configuration
        auto result = DropRateValidator::ValidateConfigFile(invalidConfigPath);
        
        if (!result.isValid && !result.errors.empty()) {
            std::cout << "PASSED\n";
        } else {
            std::cout << "FAILED (Should have detected negative multiplier)\n";
            allTestsPassed = false;
        }
    }
    
    // Test 3: Invalid configuration (min > max)
    {
        std::cout << "Test 3: Invalid configuration (min > max)... ";
        
        std::string invalidConfigPath = testDir + "/invalid_config_minmax.json";
        
        // Create an invalid configuration with min > max
        std::ofstream configFile(invalidConfigPath);
        configFile << "{\n"
                   << "  \"maxDropRateMultiplier\": 0.1,\n"
                   << "  \"minDropRateMultiplier\": 10.0,\n"
                   << "  \"modifiers\": []\n"
                   << "}\n";
        configFile.close();
        
        // Validate the configuration
        auto result = DropRateValidator::ValidateConfigFile(invalidConfigPath);
        
        if (!result.isValid && !result.errors.empty()) {
            std::cout << "PASSED\n";
        } else {
            std::cout << "FAILED (Should have detected min > max)\n";
            allTestsPassed = false;
        }
    }
    
    // Test 4: Invalid configuration (missing targetId)
    {
        std::cout << "Test 4: Invalid configuration (missing targetId)... ";
        
        std::string invalidConfigPath = testDir + "/invalid_config_targetid.json";
        
        // Create an invalid configuration with missing targetId
        std::ofstream configFile(invalidConfigPath);
        configFile << "{\n"
                   << "  \"maxDropRateMultiplier\": 10.0,\n"
                   << "  \"minDropRateMultiplier\": 0.1,\n"
                   << "  \"modifiers\": [\n"
                   << "    {\n"
                   << "      \"scope\": \"itemType\",\n"
                   << "      \"modType\": \"multiplier\",\n"
                   << "      \"value\": 1.5,\n"
                   << "      \"context\": \"always\",\n"
                   << "      \"priority\": 0\n"
                   << "    }\n"
                   << "  ]\n"
                   << "}\n";
        configFile.close();
        
        // Validate the configuration
        auto result = DropRateValidator::ValidateConfigFile(invalidConfigPath);
        
        if (!result.isValid && !result.errors.empty()) {
            std::cout << "PASSED\n";
        } else {
            std::cout << "FAILED (Should have detected missing targetId)\n";
            allTestsPassed = false;
        }
    }
    
    // Test 5: Warning configuration (very high multiplier)
    {
        std::cout << "Test 5: Warning configuration (very high multiplier)... ";
        
        std::string warningConfigPath = testDir + "/warning_config.json";
        
        // Create a configuration with a very high multiplier
        std::ofstream configFile(warningConfigPath);
        configFile << "{\n"
                   << "  \"maxDropRateMultiplier\": 100.0,\n"
                   << "  \"minDropRateMultiplier\": 0.1,\n"
                   << "  \"modifiers\": [\n"
                   << "    {\n"
                   << "      \"scope\": \"global\",\n"
                   << "      \"modType\": \"multiplier\",\n"
                   << "      \"value\": 50.0,\n"
                   << "      \"context\": \"always\",\n"
                   << "      \"priority\": 0\n"
                   << "    }\n"
                   << "  ]\n"
                   << "}\n";
        configFile.close();
        
        // Validate the configuration
        auto result = DropRateValidator::ValidateConfigFile(warningConfigPath);
        
        if (result.isValid && !result.warnings.empty()) {
            std::cout << "PASSED\n";
        } else {
            std::cout << "FAILED (Should have generated warnings)\n";
            allTestsPassed = false;
        }
    }
    
    return allTestsPassed;
}

/**
 * @brief Test the configuration loading and saving
 * @param testDir Directory to store test files
 * @return True if all tests pass
 */
bool TestConfigurationIO(const std::string& testDir)
{
    std::cout << "\n=== Testing Configuration I/O ===\n";
    
    bool allTestsPassed = true;
    
    // Test 1: Save and load configuration
    {
        std::cout << "Test 1: Save and load configuration... ";
        
        std::string configPath = testDir + "/test_config_io.json";
        
        // Create a configuration
        DropRateConfig config;
        config.SetMaxDropRateMultiplier(5.0f);
        config.SetMinDropRateMultiplier(0.2f);
        
        // Add some modifiers
        DropRateModifier modifier1;
        modifier1.scope = DropRateScope::Global;
        modifier1.targetId = "";
        modifier1.context = DropRateContext::Always;
        modifier1.modType = DropRateModType::Multiplier;
        modifier1.value = 1.2f;
        modifier1.priority = 0;
        modifier1.description = "Global 20% increase";
        
        DropRateModifier modifier2;
        modifier2.scope = DropRateScope::ItemClass;
        modifier2.targetId = "Weapon";
        modifier2.context = DropRateContext::MonsterDrop;
        modifier2.modType = DropRateModType::Multiplier;
        modifier2.value = 1.5f;
        modifier2.priority = 10;
        modifier2.description = "Weapon 50% increase from monsters";
        
        config.AddModifier(modifier1);
        config.AddModifier(modifier2);
        
        // Save the configuration
        bool saveResult = config.SaveToFile(configPath);
        
        // Load the configuration
        DropRateConfig loadedConfig;
        bool loadResult = loadedConfig.LoadFromFile(configPath, false);
        
        // Check if the loaded configuration matches the original
        bool maxMatches = loadedConfig.GetMaxDropRateMultiplier() == 5.0f;
        bool minMatches = loadedConfig.GetMinDropRateMultiplier() == 0.2f;
        bool modifiersMatch = loadedConfig.GetModifiers().size() == 2;
        
        if (saveResult && loadResult && maxMatches && minMatches && modifiersMatch) {
            std::cout << "PASSED\n";
        } else {
            std::cout << "FAILED\n";
            if (!saveResult) std::cout << "  Failed to save configuration\n";
            if (!loadResult) std::cout << "  Failed to load configuration\n";
            if (!maxMatches) std::cout << "  Max multiplier doesn't match\n";
            if (!minMatches) std::cout << "  Min multiplier doesn't match\n";
            if (!modifiersMatch) std::cout << "  Modifiers don't match\n";
            allTestsPassed = false;
        }
    }
    
    // Test 2: Create default configuration
    {
        std::cout << "Test 2: Create default configuration... ";
        
        std::string configPath = testDir + "/default_config.json";
        
        // Delete the file if it exists
        if (std::filesystem::exists(configPath)) {
            std::filesystem::remove(configPath);
        }
        
        // Create a default configuration
        bool createResult = DropRateConfig::getInstance().CreateDefaultConfig(configPath);
        
        // Check if the file exists
        bool fileExists = std::filesystem::exists(configPath);
        
        if (createResult && fileExists) {
            std::cout << "PASSED\n";
        } else {
            std::cout << "FAILED\n";
            if (!createResult) std::cout << "  Failed to create default configuration\n";
            if (!fileExists) std::cout << "  Default configuration file doesn't exist\n";
            allTestsPassed = false;
        }
    }
    
    return allTestsPassed;
}

/**
 * @brief Test the drop rate modifications
 * @param testDir Directory to store test files
 * @return True if all tests pass
 */
bool TestDropRateModifications(const std::string& testDir)
{
    std::cout << "\n=== Testing Drop Rate Modifications ===\n";
    
    bool allTestsPassed = true;
    
    // Test 1: Global multiplier
    {
        std::cout << "Test 1: Global multiplier... ";
        
        // Create a configuration with a global multiplier
        DropRateConfig config;
        config.SetMaxDropRateMultiplier(10.0f);
        config.SetMinDropRateMultiplier(0.1f);
        
        DropRateModifier modifier;
        modifier.scope = DropRateScope::Global;
        modifier.targetId = "";
        modifier.context = DropRateContext::Always;
        modifier.modType = DropRateModType::Multiplier;
        modifier.value = 2.0f;
        modifier.priority = 0;
        
        config.AddModifier(modifier);
        
        // Create a test item with a drop rate of 10
        ItemData testItem;
        testItem.dropRate = 10;
        
        // Get the modified drop rate
        float modifiedRate = config.GetModifiedDropRate(testItem, DropRateContext::MonsterDrop, 30, 16, false);
        
        // Check if the modified rate is correct (should be 20)
        if (modifiedRate == 20.0f) {
            std::cout << "PASSED\n";
        } else {
            std::cout << "FAILED (Expected 20.0, got " << modifiedRate << ")\n";
            allTestsPassed = false;
        }
    }
    
    // Test 2: Item class multiplier
    {
        std::cout << "Test 2: Item class multiplier... ";
        
        // Create a configuration with an item class multiplier
        DropRateConfig config;
        config.SetMaxDropRateMultiplier(10.0f);
        config.SetMinDropRateMultiplier(0.1f);
        
        DropRateModifier modifier;
        modifier.scope = DropRateScope::ItemClass;
        modifier.targetId = "Weapon";
        modifier.context = DropRateContext::Always;
        modifier.modType = DropRateModType::Multiplier;
        modifier.value = 3.0f;
        modifier.priority = 0;
        
        config.AddModifier(modifier);
        
        // Create a test weapon item with a drop rate of 10
        ItemData testItem;
        testItem.dropRate = 10;
        testItem._iClass = ICLASS_WEAPON;
        
        // Get the modified drop rate
        float modifiedRate = config.GetModifiedDropRate(testItem, DropRateContext::MonsterDrop, 30, 16, false);
        
        // Check if the modified rate is correct (should be 30)
        if (modifiedRate == 30.0f) {
            std::cout << "PASSED\n";
        } else {
            std::cout << "FAILED (Expected 30.0, got " << modifiedRate << ")\n";
            allTestsPassed = false;
        }
    }
    
    // Test 3: Context-specific modifier
    {
        std::cout << "Test 3: Context-specific modifier... ";
        
        // Create a configuration with a context-specific modifier
        DropRateConfig config;
        config.SetMaxDropRateMultiplier(10.0f);
        config.SetMinDropRateMultiplier(0.1f);
        
        DropRateModifier modifier;
        modifier.scope = DropRateScope::Global;
        modifier.targetId = "";
        modifier.context = DropRateContext::MonsterDrop;
        modifier.modType = DropRateModType::Multiplier;
        modifier.value = 2.0f;
        modifier.priority = 0;
        
        config.AddModifier(modifier);
        
        // Create a test item with a drop rate of 10
        ItemData testItem;
        testItem.dropRate = 10;
        
        // Get the modified drop rate for monster drops
        float monsterDropRate = config.GetModifiedDropRate(testItem, DropRateContext::MonsterDrop, 30, 16, false);
        
        // Get the modified drop rate for chest drops
        float chestDropRate = config.GetModifiedDropRate(testItem, DropRateContext::ChestDrop, 30, 16, false);
        
        // Check if the modified rates are correct
        // Monster drop rate should be 20, chest drop rate should be 10
        if (monsterDropRate == 20.0f && chestDropRate == 10.0f) {
            std::cout << "PASSED\n";
        } else {
            std::cout << "FAILED (Expected 20.0 and 10.0, got " << monsterDropRate << " and " << chestDropRate << ")\n";
            allTestsPassed = false;
        }
    }
    
    // Test 4: Condition-based modifier
    {
        std::cout << "Test 4: Condition-based modifier... ";
        
        // Create a configuration with a condition-based modifier
        DropRateConfig config;
        config.SetMaxDropRateMultiplier(10.0f);
        config.SetMinDropRateMultiplier(0.1f);
        
        DropRateModifier modifier;
        modifier.scope = DropRateScope::Global;
        modifier.targetId = "";
        modifier.context = DropRateContext::Always;
        modifier.modType = DropRateModType::Multiplier;
        modifier.value = 2.0f;
        modifier.priority = 0;
        modifier.condition.minMonsterLevel = 20;
        
        config.AddModifier(modifier);
        
        // Create a test item with a drop rate of 10
        ItemData testItem;
        testItem.dropRate = 10;
        
        // Get the modified drop rate for monster level 30 (should apply)
        float highLevelRate = config.GetModifiedDropRate(testItem, DropRateContext::MonsterDrop, 30, 16, false);
        
        // Get the modified drop rate for monster level 10 (should not apply)
        float lowLevelRate = config.GetModifiedDropRate(testItem, DropRateContext::MonsterDrop, 10, 16, false);
        
        // Check if the modified rates are correct
        // High level rate should be 20, low level rate should be 10
        if (highLevelRate == 20.0f && lowLevelRate == 10.0f) {
            std::cout << "PASSED\n";
        } else {
            std::cout << "FAILED (Expected 20.0 and 10.0, got " << highLevelRate << " and " << lowLevelRate << ")\n";
            allTestsPassed = false;
        }
    }
    
    // Test 5: Multiple modifiers with priorities
    {
        std::cout << "Test 5: Multiple modifiers with priorities... ";
        
        // Create a configuration with multiple modifiers
        DropRateConfig config;
        config.SetMaxDropRateMultiplier(10.0f);
        config.SetMinDropRateMultiplier(0.1f);
        
        // Global modifier with low priority
        DropRateModifier modifier1;
        modifier1.scope = DropRateScope::Global;
        modifier1.targetId = "";
        modifier1.context = DropRateContext::Always;
        modifier1.modType = DropRateModType::Multiplier;
        modifier1.value = 2.0f;
        modifier1.priority = 0;
        
        // Item class modifier with high priority
        DropRateModifier modifier2;
        modifier2.scope = DropRateScope::ItemClass;
        modifier2.targetId = "Weapon";
        modifier2.context = DropRateContext::Always;
        modifier2.modType = DropRateModType::Multiplier;
        modifier2.value = 3.0f;
        modifier2.priority = 10;
        
        config.AddModifier(modifier1);
        config.AddModifier(modifier2);
        
        // Create a test weapon item with a drop rate of 10
        ItemData testItem;
        testItem.dropRate = 10;
        testItem._iClass = ICLASS_WEAPON;
        
        // Get the modified drop rate
        // High priority modifier should be applied first, then low priority
        // 10 * 3 * 2 = 60
        float modifiedRate = config.GetModifiedDropRate(testItem, DropRateContext::MonsterDrop, 30, 16, false);
        
        // Check if the modified rate is correct
        if (modifiedRate == 60.0f) {
            std::cout << "PASSED\n";
        } else {
            std::cout << "FAILED (Expected 60.0, got " << modifiedRate << ")\n";
            allTestsPassed = false;
        }
    }
    
    return allTestsPassed;
}

/**
 * @brief Run all tests and generate a report
 * @param testDir Directory to store test files
 */
void RunAllTests(const std::string& testDir)
{
    std::cout << "=== Item Drop Rate Modification System Tests ===\n";
    std::cout << "Test directory: " << testDir << "\n";
    
    // Create the test directory if it doesn't exist
    if (!std::filesystem::exists(testDir)) {
        std::filesystem::create_directories(testDir);
    }
    
    // Run all tests
    bool validationPassed = TestValidation(testDir);
    bool configIOPassed = TestConfigurationIO(testDir);
    bool modificationsPassed = TestDropRateModifications(testDir);
    
    // Generate a report
    std::cout << "\n=== Test Results ===\n";
    std::cout << "Validation tests: " << (validationPassed ? "PASSED" : "FAILED") << "\n";
    std::cout << "Configuration I/O tests: " << (configIOPassed ? "PASSED" : "FAILED") << "\n";
    std::cout << "Drop rate modification tests: " << (modificationsPassed ? "PASSED" : "FAILED") << "\n";
    std::cout << "Overall: " << ((validationPassed && configIOPassed && modificationsPassed) ? "PASSED" : "FAILED") << "\n";
    
    // Generate a drop rate report using the test tools
    std::cout << "\nGenerating drop rate report...\n";
    std::string reportPath = testDir + "/drop_rate_report.md";
    bool reportGenerated = DropRateTest::getInstance().GenerateReport(reportPath);
    
    if (reportGenerated) {
        std::cout << "Drop rate report generated: " << reportPath << "\n";
    } else {
        std::cout << "Failed to generate drop rate report\n";
    }
}

} // namespace devilution

/**
 * Entry point for the test script
 */
int main(int argc, char* argv[])
{
    // Set up the test directory
    std::string testDir = "test_output";
    
    if (argc > 1) {
        testDir = argv[1];
    }
    
    // Run all tests
    devilution::RunAllTests(testDir);
    
    return 0;
}
