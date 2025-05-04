#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "monster.h"
#include "monstdat.h"
#include "utils/log.hpp"

namespace devilution {
namespace {

class MonsterTypeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize logging for tests
        LogInit();
        
        // Load monster data to ensure MonstersData is populated
        LoadMonstDat();
        LoadUniqueMonstDat();
        
        // Reset LevelMonsterTypes and LevelMonsterTypeCount for clean testing
        LevelMonsterTypeCount = 0;
        for (size_t i = 0; i < MaxLvlMTypes; i++) {
            LevelMonsterTypes[i].placeFlags = 0;
        }
    }

    void TearDown() override {
        // Clean up after tests
        LogClose();
    }
};

TEST_F(MonsterTypeTest, IsGolemHandlesAllGolemTypes) {
    // Test that IsGolem correctly identifies all Golem types
    
    // Test MT_GOLEM (should return true)
    EXPECT_TRUE(IsGolem(MT_GOLEM)) << "IsGolem should identify MT_GOLEM as a Golem";
    
    // Test monster type 138 (should return true after fix)
    EXPECT_TRUE(IsGolem(static_cast<_monster_id>(138))) << "IsGolem should identify monster type 138 as a Golem";
    
    // Test monster type 139 (should return true)
    EXPECT_TRUE(IsGolem(static_cast<_monster_id>(139))) << "IsGolem should identify monster type 139 as a Golem";
    
    // Test some non-Golem monster types (should return false)
    EXPECT_FALSE(IsGolem(MT_ZOMBIE)) << "IsGolem should not identify MT_ZOMBIE as a Golem";
    EXPECT_FALSE(IsGolem(MT_DIABLO)) << "IsGolem should not identify MT_DIABLO as a Golem";
    EXPECT_FALSE(IsGolem(static_cast<_monster_id>(137))) << "IsGolem should not identify monster type 137 as a Golem";
    EXPECT_FALSE(IsGolem(static_cast<_monster_id>(140))) << "IsGolem should not identify monster type 140 as a Golem";
}

TEST_F(MonsterTypeTest, AddMonsterTypeHandlesAllGolemTypes) {
    // Test that AddMonsterType correctly handles all Golem types
    
    // Test adding MT_GOLEM (should succeed)
    auto result = AddMonsterType(MT_GOLEM, PLACE_SCATTER);
    EXPECT_TRUE(result.has_value()) << "AddMonsterType should succeed with MT_GOLEM";
    EXPECT_EQ(LevelMonsterTypeCount, 1) << "LevelMonsterTypeCount should be incremented";
    
    // Test adding monster type 138 (should succeed after fix)
    result = AddMonsterType(static_cast<_monster_id>(138), PLACE_SCATTER);
    EXPECT_TRUE(result.has_value()) << "AddMonsterType should handle monster type 138 as a Golem";
    EXPECT_EQ(LevelMonsterTypeCount, 2) << "LevelMonsterTypeCount should be incremented";
    
    // Test adding monster type 139 (should succeed)
    result = AddMonsterType(static_cast<_monster_id>(139), PLACE_SCATTER);
    EXPECT_TRUE(result.has_value()) << "AddMonsterType should handle monster type 139 as a Golem";
    EXPECT_EQ(LevelMonsterTypeCount, 3) << "LevelMonsterTypeCount should be incremented";
    
    // Verify that the monster types were added with the correct place flags
    EXPECT_EQ(LevelMonsterTypes[0].type, MT_GOLEM);
    EXPECT_EQ(LevelMonsterTypes[0].placeFlags, PLACE_SCATTER);
    
    EXPECT_EQ(LevelMonsterTypes[1].type, static_cast<_monster_id>(138));
    EXPECT_EQ(LevelMonsterTypes[1].placeFlags, PLACE_SCATTER);
    
    EXPECT_EQ(LevelMonsterTypes[2].type, static_cast<_monster_id>(139));
    EXPECT_EQ(LevelMonsterTypes[2].placeFlags, PLACE_SCATTER);
}

TEST_F(MonsterTypeTest, AddMonsterTypeHandlesValidMonsterTypes) {
    // Test that AddMonsterType correctly handles valid monster types
    
    // Test adding some valid monster types
    std::vector<_monster_id> validTypes = {
        MT_ZOMBIE, MT_GOLEM, MT_DIABLO, MT_BAAL, MT_NAKRUL
    };
    
    for (const auto& type : validTypes) {
        auto result = AddMonsterType(type, PLACE_SCATTER);
        EXPECT_TRUE(result.has_value()) << "AddMonsterType should succeed with valid monster type: " 
                                        << static_cast<int>(type);
    }
    
    // Verify that LevelMonsterTypeCount was incremented correctly
    EXPECT_EQ(LevelMonsterTypeCount, validTypes.size()) << "LevelMonsterTypeCount should match the number of added types";
}

TEST_F(MonsterTypeTest, AddMonsterTypeRejectsInvalidMonsterTypes) {
    // Test that AddMonsterType correctly rejects invalid monster types
    
    // Test adding an invalid negative monster type
    auto result = AddMonsterType(static_cast<_monster_id>(-1), PLACE_SCATTER);
    EXPECT_FALSE(result.has_value()) << "AddMonsterType should reject invalid negative monster type";
    
    // Test adding an invalid monster type that's too large
    result = AddMonsterType(static_cast<_monster_id>(1000), PLACE_SCATTER);
    EXPECT_FALSE(result.has_value()) << "AddMonsterType should reject invalid large monster type";
    
    // Verify that LevelMonsterTypeCount was not incremented
    EXPECT_EQ(LevelMonsterTypeCount, 0) << "LevelMonsterTypeCount should not be incremented for invalid types";
}

TEST_F(MonsterTypeTest, GetMonsterTypeIndexHandlesAllGolemTypes) {
    // Test that GetMonsterTypeIndex correctly handles all Golem types
    
    // Add all Golem types to LevelMonsterTypes
    AddMonsterType(MT_GOLEM, PLACE_SCATTER);
    AddMonsterType(static_cast<_monster_id>(138), PLACE_SCATTER);
    AddMonsterType(static_cast<_monster_id>(139), PLACE_SCATTER);
    
    // Test finding MT_GOLEM
    size_t typeIndex = GetMonsterTypeIndex(MT_GOLEM);
    EXPECT_NE(typeIndex, LevelMonsterTypeCount) << "GetMonsterTypeIndex should find MT_GOLEM";
    EXPECT_EQ(typeIndex, 0) << "GetMonsterTypeIndex should return the correct index for MT_GOLEM";
    
    // Test finding monster type 138
    typeIndex = GetMonsterTypeIndex(static_cast<_monster_id>(138));
    EXPECT_NE(typeIndex, LevelMonsterTypeCount) << "GetMonsterTypeIndex should find monster type 138";
    EXPECT_EQ(typeIndex, 1) << "GetMonsterTypeIndex should return the correct index for monster type 138";
    
    // Test finding monster type 139
    typeIndex = GetMonsterTypeIndex(static_cast<_monster_id>(139));
    EXPECT_NE(typeIndex, LevelMonsterTypeCount) << "GetMonsterTypeIndex should find monster type 139";
    EXPECT_EQ(typeIndex, 2) << "GetMonsterTypeIndex should return the correct index for monster type 139";
}

TEST_F(MonsterTypeTest, AddMonsterTypeHandlesPlaceFlags) {
    // Test that AddMonsterType correctly handles different place flags
    
    // Add monster type 138 with PLACE_SCATTER
    auto result = AddMonsterType(static_cast<_monster_id>(138), PLACE_SCATTER);
    EXPECT_TRUE(result.has_value()) << "AddMonsterType should handle monster type 138 with PLACE_SCATTER";
    
    // Add monster type 138 again with PLACE_SPECIAL (should update place flags, not add a new entry)
    result = AddMonsterType(static_cast<_monster_id>(138), PLACE_SPECIAL);
    EXPECT_TRUE(result.has_value()) << "AddMonsterType should handle monster type 138 with PLACE_SPECIAL";
    
    // Verify that LevelMonsterTypeCount is still 1 (no new entry added)
    EXPECT_EQ(LevelMonsterTypeCount, 1) << "LevelMonsterTypeCount should still be 1";
    
    // Verify that the place flags were updated correctly (should have both PLACE_SCATTER and PLACE_SPECIAL)
    EXPECT_EQ(LevelMonsterTypes[0].placeFlags, PLACE_SCATTER | PLACE_SPECIAL) 
        << "Place flags should be updated correctly";
}

TEST_F(MonsterTypeTest, IntegrationTestMonsterType138) {
    // Integration test that simulates the game's monster creation workflow with type 138
    
    // Step 1: Add monster type 138
    auto result = AddMonsterType(static_cast<_monster_id>(138), PLACE_SCATTER);
    EXPECT_TRUE(result.has_value()) << "AddMonsterType should handle monster type 138";
    
    // Step 2: Verify it was added correctly
    size_t typeIndex = GetMonsterTypeIndex(static_cast<_monster_id>(138));
    EXPECT_NE(typeIndex, LevelMonsterTypeCount) << "GetMonsterTypeIndex should find monster type 138";
    
    // Step 3: Verify the monster type and place flags
    EXPECT_EQ(LevelMonsterTypes[typeIndex].type, static_cast<_monster_id>(138));
    EXPECT_EQ(LevelMonsterTypes[typeIndex].placeFlags, PLACE_SCATTER);
    
    // Step 4: Add the same monster type with a different place flag
    result = AddMonsterType(static_cast<_monster_id>(138), PLACE_UNIQUE);
    EXPECT_TRUE(result.has_value()) << "AddMonsterType should handle monster type 138 with PLACE_UNIQUE";
    
    // Step 5: Verify the place flags were updated correctly
    EXPECT_EQ(LevelMonsterTypes[typeIndex].placeFlags, PLACE_SCATTER | PLACE_UNIQUE) 
        << "Place flags should be updated correctly";
}

} // namespace
} // namespace devilution
