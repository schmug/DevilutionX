/**
 * @file drop_rate_cli.cpp
 *
 * Command-line interface for testing the item drop rate modification system.
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <filesystem>

#include "utils/log.hpp"
#include "utils/paths.h"
#include "mods/drop_rate_test.h"
#include "mods/drop_rate_modifier.h"
#include "mods/config/drop_rate_config.h"

namespace devilution {

/**
 * @brief Command-line interface for the drop rate testing tool
 */
class DropRateCliTool {
public:
    /**
     * @brief Run the CLI tool with the given arguments
     * @param args Command-line arguments
     * @return Exit code (0 for success)
     */
    int Run(const std::vector<std::string>& args)
    {
        if (args.size() < 2) {
            PrintUsage();
            return 1;
        }
        
        const std::string& command = args[1];
        
        if (command == "help") {
            PrintUsage();
            return 0;
        } else if (command == "report") {
            return GenerateReport(args);
        } else if (command == "simulate") {
            return SimulateDrops(args);
        } else if (command == "compare") {
            return CompareDropRates(args);
        } else if (command == "reload") {
            return ReloadConfig(args);
        } else {
            std::cerr << "Unknown command: " << command << std::endl;
            PrintUsage();
            return 1;
        }
    }

private:
    /**
     * @brief Print usage information
     */
    void PrintUsage()
    {
        std::cout << "DevilutionX Drop Rate Testing Tool\n\n";
        std::cout << "Usage: droprate <command> [options]\n\n";
        std::cout << "Commands:\n";
        std::cout << "  help                 Show this help message\n";
        std::cout << "  report [output]      Generate a report of drop rate modifications\n";
        std::cout << "  simulate <count> [level] [dungeon]  Simulate drops and show distribution\n";
        std::cout << "  compare [level] [dungeon]  Compare original and modified drop rates\n";
        std::cout << "  reload [config]      Reload the drop rate configuration\n\n";
        std::cout << "Options:\n";
        std::cout << "  output   Path to write the report to (default: drop_rate_report.md)\n";
        std::cout << "  count    Number of drops to simulate (default: 1000)\n";
        std::cout << "  level    Monster level to use (default: 30)\n";
        std::cout << "  dungeon  Dungeon level to use (default: 16)\n";
        std::cout << "  config   Path to the configuration file (default: user config)\n";
    }
    
    /**
     * @brief Generate a drop rate report
     * @param args Command-line arguments
     * @return Exit code (0 for success)
     */
    int GenerateReport(const std::vector<std::string>& args)
    {
        std::string outputPath = "drop_rate_report.md";
        if (args.size() > 2) {
            outputPath = args[2];
        }
        
        std::cout << "Generating drop rate report to " << outputPath << "..." << std::endl;
        
        bool success = DropRateTest::getInstance().GenerateReport(outputPath);
        if (success) {
            std::cout << "Report generated successfully!" << std::endl;
            return 0;
        } else {
            std::cerr << "Failed to generate report." << std::endl;
            return 1;
        }
    }
    
    /**
     * @brief Simulate item drops
     * @param args Command-line arguments
     * @return Exit code (0 for success)
     */
    int SimulateDrops(const std::vector<std::string>& args)
    {
        int numDrops = 1000;
        int monsterLevel = 30;
        int dungeonLevel = 16;
        
        if (args.size() > 2) {
            try {
                numDrops = std::stoi(args[2]);
            } catch (const std::exception&) {
                std::cerr << "Invalid number of drops: " << args[2] << std::endl;
                return 1;
            }
        }
        
        if (args.size() > 3) {
            try {
                monsterLevel = std::stoi(args[3]);
            } catch (const std::exception&) {
                std::cerr << "Invalid monster level: " << args[3] << std::endl;
                return 1;
            }
        }
        
        if (args.size() > 4) {
            try {
                dungeonLevel = std::stoi(args[4]);
            } catch (const std::exception&) {
                std::cerr << "Invalid dungeon level: " << args[4] << std::endl;
                return 1;
            }
        }
        
        std::cout << "Simulating " << numDrops << " drops (Monster Level: " << monsterLevel 
                  << ", Dungeon Level: " << dungeonLevel << ")..." << std::endl;
        
        auto results = DropRateTest::getInstance().SimulateDrops(numDrops, monsterLevel, dungeonLevel);
        
        std::cout << "\nDrop Simulation Results:\n";
        std::cout << "------------------------\n";
        
        int totalDrops = 0;
        for (const auto& [itemType, count] : results) {
            totalDrops += count;
        }
        
        for (const auto& [itemType, count] : results) {
            float percentage = static_cast<float>(count) / totalDrops * 100.0f;
            std::cout << itemType << ": " << count << " (" 
                      << std::fixed << std::setprecision(2) << percentage << "%)\n";
        }
        
        return 0;
    }
    
    /**
     * @brief Compare original and modified drop rates
     * @param args Command-line arguments
     * @return Exit code (0 for success)
     */
    int CompareDropRates(const std::vector<std::string>& args)
    {
        int monsterLevel = 30;
        int dungeonLevel = 16;
        
        if (args.size() > 2) {
            try {
                monsterLevel = std::stoi(args[2]);
            } catch (const std::exception&) {
                std::cerr << "Invalid monster level: " << args[2] << std::endl;
                return 1;
            }
        }
        
        if (args.size() > 3) {
            try {
                dungeonLevel = std::stoi(args[3]);
            } catch (const std::exception&) {
                std::cerr << "Invalid dungeon level: " << args[3] << std::endl;
                return 1;
            }
        }
        
        std::cout << "Comparing drop rates (Monster Level: " << monsterLevel 
                  << ", Dungeon Level: " << dungeonLevel << ")..." << std::endl;
        
        auto comparison = DropRateTest::getInstance().CompareDropRates(monsterLevel, dungeonLevel);
        
        std::cout << "\nDrop Rate Comparison:\n";
        std::cout << "--------------------\n";
        std::cout << std::left << std::setw(30) << "Item" 
                  << std::setw(15) << "Original" 
                  << std::setw(15) << "Modified" 
                  << "Change\n";
        std::cout << std::string(70, '-') << "\n";
        
        for (const auto& [itemName, rates] : comparison) {
            float originalRate = rates.first;
            float modifiedRate = rates.second;
            float changeFactor = originalRate > 0 ? modifiedRate / originalRate : 0;
            
            std::cout << std::left << std::setw(30) << itemName 
                      << std::setw(15) << std::fixed << std::setprecision(2) << originalRate 
                      << std::setw(15) << std::fixed << std::setprecision(2) << modifiedRate 
                      << std::fixed << std::setprecision(2) << changeFactor << "x\n";
        }
        
        return 0;
    }
    
    /**
     * @brief Reload the drop rate configuration
     * @param args Command-line arguments
     * @return Exit code (0 for success)
     */
    int ReloadConfig(const std::vector<std::string>& args)
    {
        std::string configPath;
        
        if (args.size() > 2) {
            configPath = args[2];
        } else {
            configPath = paths::PrefPath() + "mods/config/drop_rate_config.json";
        }
        
        std::cout << "Reloading drop rate configuration from " << configPath << "..." << std::endl;
        
        bool success = DropRateManager::getInstance().ReloadConfig(configPath);
        if (success) {
            std::cout << "Configuration reloaded successfully!" << std::endl;
            return 0;
        } else {
            std::cerr << "Failed to reload configuration." << std::endl;
            return 1;
        }
    }
};

} // namespace devilution

/**
 * Entry point for the drop rate CLI tool
 */
int main(int argc, char* argv[])
{
    std::vector<std::string> args;
    for (int i = 0; i < argc; i++) {
        args.push_back(argv[i]);
    }
    
    devilution::DropRateCliTool tool;
    return tool.Run(args);
}
