#include <gtest/gtest.h>
#include <iostream>
#include <string>

#include "utils/log.hpp"

// This is the main entry point for running all verification tests
int main(int argc, char** argv) {
    // Initialize Google Test
    ::testing::InitGoogleTest(&argc, argv);
    
    // Initialize logging
    devilution::LogInit();
    
    std::cout << "=== DevilutionX Dynamic Difficulty System Verification Tests ===" << std::endl;
    std::cout << "Running all test scenarios..." << std::endl;
    
    // Run all tests
    int result = RUN_ALL_TESTS();
    
    // Clean up
    devilution::LogClose();
    
    // Output summary
    if (result == 0) {
        std::cout << "All verification tests passed!" << std::endl;
        std::cout << "See difficulty_system_verification_report.txt for detailed results." << std::endl;
    } else {
        std::cout << "Some verification tests failed!" << std::endl;
        std::cout << "See difficulty_system_verification_report.txt for detailed results." << std::endl;
        std::cout << "Check Source/difficulty/EDGE_CASES.md for information on handling edge cases." << std::endl;
    }
    
    return result;
}
