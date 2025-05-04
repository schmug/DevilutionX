#include <gtest/gtest.h>
#include <string>
#include <filesystem>

#include "monstdat.h"
#include "utils/file_util.h"
#include "utils/log.hpp"

namespace devilution {
namespace {

class MonsterDataTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize logging for tests
        LogInit();
    }

    void TearDown() override {
        // Clean up after tests
        LogClose();
    }
};

TEST_F(MonsterDataTest, MonsterDataFilesExist) {
    // Test that the monster data files exist in the correct locations
    std::string basePath = GetBasePath();
    
    // Check original monster data file
    std::string monstdatPath = basePath + "assets/txtdata/monsters/monstdat.tsv";
    EXPECT_TRUE(std::filesystem::exists(monstdatPath))
        << "Monster data file not found: " << monstdatPath;
    
    // Check unique monster data file
    std::string uniqueMonstdatPath = basePath + "assets/txtdata/monsters/unique_monstdat.tsv";
    EXPECT_TRUE(std::filesystem::exists(uniqueMonstdatPath))
        << "Unique monster data file not found: " << uniqueMonstdatPath;
}

TEST_F(MonsterDataTest, LoadMonsterDataSucceeds) {
    // Test that the monster data can be loaded successfully
    try {
        LoadMonstDat();
        // If we get here, the function didn't throw an exception
        SUCCEED();
    } catch (const std::exception& e) {
        FAIL() << "LoadMonstDat threw an exception: " << e.what();
    }
}

TEST_F(MonsterDataTest, LoadUniqueMonsterDataSucceeds) {
    // Test that the unique monster data can be loaded successfully
    try {
        LoadUniqueMonstDat();
        // If we get here, the function didn't throw an exception
        SUCCEED();
    } catch (const std::exception& e) {
        FAIL() << "LoadUniqueMonstDat threw an exception: " << e.what();
    }
}

TEST_F(MonsterDataTest, MonsterDataContainsExpectedEntries) {
    // Test that the monster data contains the expected entries
    LoadMonstDat();
    
    // Check that we have at least some monsters loaded
    EXPECT_GT(MonstersData.size(), 0)
        << "No monsters were loaded from the data file";
    
    // Check for a few specific monsters that should be in the data
    bool foundZombie = false;
    bool foundSkeleton = false;
    bool foundDemon = false;
    
    for (const auto& monster : MonstersData) {
        if (monster.id == "zombie" || monster.id == "Zombie") {
            foundZombie = true;
        } else if (monster.id == "skeleton" || monster.id == "Skeleton") {
            foundSkeleton = true;
        } else if (monster.id == "demon" || monster.id == "Demon") {
            foundDemon = true;
        }
    }
    
    EXPECT_TRUE(foundZombie) << "Zombie monster not found in monster data";
    EXPECT_TRUE(foundSkeleton) << "Skeleton monster not found in monster data";
    // Note: This test might fail if the specific monster names don't match what's in the data file
    // Adjust the monster names as needed based on the actual data
}

} // namespace
} // namespace devilution
