/**
 * @file test_drop_rates.cpp
 *
 * Test program for the drop rate modification system.
 */

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "mods/drop_rate_modifier.h"
#include "mods/config/drop_rate_config.h"
#include "utils/paths.h"
#include "utils/log.hpp"

using namespace devilution;

// Mock ItemData structure for testing
struct MockItemData {
    std::string name;
    int itemType;
    int quality;
    int level;
    bool isUnique;
    
    // Constructor to match the expected ItemData interface
    MockItemData(const std::string& name, int type, int quality, int level, bool unique)
        : name(name), itemType(type), quality(quality), level(level), isUnique(unique)
    {
    }
};

// Convert MockItemData to ItemData for testing
ItemData ConvertToItemData(const MockItemData& mockItem)
{
    ItemData item;
    // Set the necessary fields that are used by the drop rate system
    item._itype = static_cast<item_type>(mockItem.itemType);
    item._iquality = static_cast<item_quality>(mockItem.quality);
    item._iMinMagic = mockItem.level;
    item._iIName = mockItem.name;
    return item;
}

// Test different drop contexts
void TestDropRates(DropRateManager& dropRateManager)
{
    std::cout << "=== Testing Drop Rates ===" << std::endl;
    
    // Create some test items
    std::vector<MockItemData> testItems = {
        MockItemData("Sword", 1, 0, 1, false),         // Normal sword, low level
        MockItemData("Magic Sword", 1, 1, 5, false),   // Magic sword, mid level
        MockItemData("Rare Sword", 1, 2, 10, false),   // Rare sword, high level
        MockItemData("Unique Sword", 1, 3, 15, true),  // Unique sword, very high level
        MockItemData("Potion", 2, 0, 1, false),        // Normal potion, low level
        MockItemData("Magic Armor", 3, 1, 8, false),   // Magic armor, high level
        MockItemData("Rare Shield", 4, 2, 12, false),  // Rare shield, very high level
    };
    
    // Test monster drops at different levels
    std::cout << "\nMonster Drop Rates:" << std::endl;
    for (const auto& mockItem : testItems) {
        ItemData item = ConvertToItemData(mockItem);
        
        std::cout << "Item: " << mockItem.name << std::endl;
        for (int monsterLevel = 1; monsterLevel <= 30; monsterLevel += 10) {
            float dropRate = dropRateManager.GetMonsterDropRate(item, monsterLevel, mockItem.isUnique);
            std::cout << "  Monster Level " << monsterLevel << ": " << dropRate << std::endl;
        }
        std::cout << std::endl;
    }
    
    // Test chest drops at different dungeon levels
    std::cout << "\nChest Drop Rates:" << std::endl;
    for (const auto& mockItem : testItems) {
        ItemData item = ConvertToItemData(mockItem);
        
        std::cout << "Item: " << mockItem.name << std::endl;
        for (int dungeonLevel = 1; dungeonLevel <= 16; dungeonLevel += 5) {
            float dropRate = dropRateManager.GetChestDropRate(item, dungeonLevel, mockItem.isUnique);
            std::cout << "  Dungeon Level " << dungeonLevel << ": " << dropRate << std::endl;
        }
        std::cout << std::endl;
    }
    
    // Test ground drops
    std::cout << "\nGround Drop Rates:" << std::endl;
    for (const auto& mockItem : testItems) {
        ItemData item = ConvertToItemData(mockItem);
        
        std::cout << "Item: " << mockItem.name << std::endl;
        for (int dungeonLevel = 1; dungeonLevel <= 16; dungeonLevel += 5) {
            float dropRate = dropRateManager.GetGroundDropRate(item, dungeonLevel, mockItem.isUnique);
            std::cout << "  Dungeon Level " << dungeonLevel << ": " << dropRate << std::endl;
        }
        std::cout << std::endl;
    }
    
    // Test quest rewards
    std::cout << "\nQuest Reward Drop Rates:" << std::endl;
    for (const auto& mockItem : testItems) {
        ItemData item = ConvertToItemData(mockItem);
        
        std::cout << "Item: " << mockItem.name << std::endl;
        for (int dungeonLevel = 1; dungeonLevel <= 16; dungeonLevel += 5) {
            float dropRate = dropRateManager.GetQuestRewardDropRate(item, dungeonLevel, mockItem.isUnique);
            std::cout << "  Dungeon Level " << dungeonLevel << ": " << dropRate << std::endl;
        }
        std::cout << std::endl;
    }
}

// Create a sample configuration file for testing
void CreateSampleConfig(const std::string& configPath)
{
    std::cout << "Creating sample configuration file: " << configPath << std::endl;
    
    // Create a directory for the config if it doesn't exist
    std::string configDir = configPath.substr(0, configPath.find_last_of("/\\"));
    std::system(("mkdir -p \"" + configDir + "\"").c_str());
    
    // Create a sample JSON configuration file
    FILE* file = fopen(configPath.c_str(), "w");
    if (file) {
        fprintf(file, "{\n");
        fprintf(file, "  \"modifiers\": [\n");
        
        // Monster drop modifiers
        fprintf(file, "    {\n");
        fprintf(file, "      \"context\": \"MonsterDrop\",\n");
        fprintf(file, "      \"itemType\": \"Sword\",\n");
        fprintf(file, "      \"multiplier\": 2.0,\n");
        fprintf(file, "      \"conditions\": {\n");
        fprintf(file, "        \"monsterLevelMin\": 5,\n");
        fprintf(file, "        \"monsterLevelMax\": 15\n");
        fprintf(file, "      }\n");
        fprintf(file, "    },\n");
        
        // Unique item modifier
        fprintf(file, "    {\n");
        fprintf(file, "      \"context\": \"Always\",\n");
        fprintf(file, "      \"isUnique\": true,\n");
        fprintf(file, "      \"multiplier\": 0.5\n");
        fprintf(file, "    },\n");
        
        // Chest drop modifier
        fprintf(file, "    {\n");
        fprintf(file, "      \"context\": \"ChestDrop\",\n");
        fprintf(file, "      \"itemQuality\": \"Magic\",\n");
        fprintf(file, "      \"multiplier\": 1.5,\n");
        fprintf(file, "      \"conditions\": {\n");
        fprintf(file, "        \"dungeonLevelMin\": 8\n");
        fprintf(file, "      }\n");
        fprintf(file, "    },\n");
        
        // Potion drop modifier
        fprintf(file, "    {\n");
        fprintf(file, "      \"context\": \"Always\",\n");
        fprintf(file, "      \"itemType\": \"Potion\",\n");
        fprintf(file, "      \"multiplier\": 1.25\n");
        fprintf(file, "    }\n");
        
        fprintf(file, "  ]\n");
        fprintf(file, "}\n");
        fclose(file);
        std::cout << "Sample configuration file created successfully." << std::endl;
    } else {
        std::cerr << "Failed to create sample configuration file!" << std::endl;
    }
}

// Mock function for ItemsGetCurrlevel
int ItemsGetCurrlevel()
{
    // Return a default dungeon level for testing
    return 5;
}

int main(int argc, char* argv[])
{
    // Initialize random seed
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    std::cout << "Drop Rate Modification System Test" << std::endl;
    std::cout << "=================================" << std::endl;
    
    // Create a sample configuration file
    std::string configPath = "mods/config/drop_rate_config.json";
    CreateSampleConfig(configPath);
    
    // Get the drop rate manager instance
    DropRateManager& dropRateManager = DropRateManager::getInstance();
    
    // Load the configuration
    if (dropRateManager.LoadConfig(configPath)) {
        std::cout << "Configuration loaded successfully." << std::endl;
        
        // Test the drop rates
        TestDropRates(dropRateManager);
        
        // Test reloading the configuration
        if (dropRateManager.ReloadConfig(configPath)) {
            std::cout << "\nConfiguration reloaded successfully." << std::endl;
        } else {
            std::cerr << "\nFailed to reload configuration!" << std::endl;
        }
    } else {
        std::cerr << "Failed to load configuration!" << std::endl;
    }
    
    return 0;
}
