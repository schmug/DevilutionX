#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <memory>

#include "player.h"
#include "items.h"
#include "gear/gear_score.h"
#include "gear/gear_config.h"
#include "utils/log.hpp"

namespace devilution {
namespace {

// Mock item for testing
class MockItem : public Item {
public:
    MockItem(item_type type, item_quality quality, int iLvl)
    {
        _itype = type;
        _iquality = quality;
        _iILvl = iLvl;
        
        // Initialize other properties
        _ivalue = 1000;
        _iMinDam = 5;
        _iMaxDam = 10;
        _iAC = 15;
        _iPLDam = 0;
        _iPLToHit = 0;
        _iPLAC = 0;
        _iPLStr = 0;
        _iPLMag = 0;
        _iPLDex = 0;
        _iPLVit = 0;
        _iPLFR = 0;
        _iPLLR = 0;
        _iPLMR = 0;
        _iPLMana = 0;
        _iPLHP = 0;
        _iPLDamMod = 0;
        _iPLGetHit = 0;
        _iPLLight = 0;
        _iSplLvlAdd = 0;
        _iUid = 0;
        _iPrePower = 0;
        _iSufPower = 0;
        _iMagical = quality != ITEM_QUALITY_NORMAL;
    }
    
    void SetDamage(int minDam, int maxDam)
    {
        _iMinDam = minDam;
        _iMaxDam = maxDam;
    }
    
    void SetArmor(int ac)
    {
        _iAC = ac;
    }
    
    void AddPrefix(int power)
    {
        _iPrePower = power;
        _iMagical = true;
    }
    
    void AddSuffix(int power)
    {
        _iSufPower = power;
        _iMagical = true;
    }
};

class GearScoreTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize logging for tests
        LogInit();
        
        // Initialize the gear scoring system
        gearScoring = &GearLevelManager::GetInstance();
        gearScoring->Initialize();
    }

    void TearDown() override {
        // Clean up after tests
        LogClose();
    }
    
    // Helper method to create a test weapon
    std::unique_ptr<MockItem> CreateTestWeapon(item_quality quality, int iLvl, int minDam, int maxDam)
    {
        auto weapon = std::make_unique<MockItem>(ITYPE_SWORD, quality, iLvl);
        weapon->SetDamage(minDam, maxDam);
        return weapon;
    }
    
    // Helper method to create a test armor
    std::unique_ptr<MockItem> CreateTestArmor(item_quality quality, int iLvl, int ac)
    {
        auto armor = std::make_unique<MockItem>(ITYPE_PLATE, quality, iLvl);
        armor->SetArmor(ac);
        return armor;
    }
    
    // Helper method to create a test jewelry
    std::unique_ptr<MockItem> CreateTestJewelry(item_quality quality, int iLvl)
    {
        return std::make_unique<MockItem>(ITYPE_RING, quality, iLvl);
    }

    GearLevelManager* gearScoring;
};

TEST_F(GearScoreTest, CalculateItemScoreReturnsExpectedValues) {
    // Test that item scores are calculated correctly for different item types and qualities
    
    // Create test items of different types and qualities
    auto normalWeapon = CreateTestWeapon(ITEM_QUALITY_NORMAL, 10, 5, 10);
    auto magicWeapon = CreateTestWeapon(ITEM_QUALITY_MAGIC, 20, 10, 20);
    auto rareWeapon = CreateTestWeapon(ITEM_QUALITY_RARE, 30, 15, 30);
    
    auto normalArmor = CreateTestArmor(ITEM_QUALITY_NORMAL, 10, 10);
    auto magicArmor = CreateTestArmor(ITEM_QUALITY_MAGIC, 20, 20);
    auto rareArmor = CreateTestArmor(ITEM_QUALITY_RARE, 30, 30);
    
    auto normalJewelry = CreateTestJewelry(ITEM_QUALITY_NORMAL, 10);
    auto magicJewelry = CreateTestJewelry(ITEM_QUALITY_MAGIC, 20);
    auto rareJewelry = CreateTestJewelry(ITEM_QUALITY_RARE, 30);
    
    // Calculate scores for each item
    float normalWeaponScore = gearScoring->CalculateItemScore(*normalWeapon);
    float magicWeaponScore = gearScoring->CalculateItemScore(*magicWeapon);
    float rareWeaponScore = gearScoring->CalculateItemScore(*rareWeapon);
    
    float normalArmorScore = gearScoring->CalculateItemScore(*normalArmor);
    float magicArmorScore = gearScoring->CalculateItemScore(*magicArmor);
    float rareArmorScore = gearScoring->CalculateItemScore(*rareArmor);
    
    float normalJewelryScore = gearScoring->CalculateItemScore(*normalJewelry);
    float magicJewelryScore = gearScoring->CalculateItemScore(*magicJewelry);
    float rareJewelryScore = gearScoring->CalculateItemScore(*rareJewelry);
    
    // Verify that higher quality items have higher scores
    EXPECT_GT(magicWeaponScore, normalWeaponScore) << "Magic weapon should have higher score than normal weapon";
    EXPECT_GT(rareWeaponScore, magicWeaponScore) << "Rare weapon should have higher score than magic weapon";
    
    EXPECT_GT(magicArmorScore, normalArmorScore) << "Magic armor should have higher score than normal armor";
    EXPECT_GT(rareArmorScore, magicArmorScore) << "Rare armor should have higher score than magic armor";
    
    EXPECT_GT(magicJewelryScore, normalJewelryScore) << "Magic jewelry should have higher score than normal jewelry";
    EXPECT_GT(rareJewelryScore, magicJewelryScore) << "Rare jewelry should have higher score than magic jewelry";
}

TEST_F(GearScoreTest, AffixesIncreaseItemScore) {
    // Test that affixes increase item score
    
    // Create a base weapon
    auto baseWeapon = CreateTestWeapon(ITEM_QUALITY_NORMAL, 20, 10, 20);
    float baseScore = gearScoring->CalculateItemScore(*baseWeapon);
    
    // Add a prefix
    auto prefixWeapon = CreateTestWeapon(ITEM_QUALITY_MAGIC, 20, 10, 20);
    prefixWeapon->AddPrefix(1); // Add a simple prefix
    float prefixScore = gearScoring->CalculateItemScore(*prefixWeapon);
    
    // Add a suffix
    auto suffixWeapon = CreateTestWeapon(ITEM_QUALITY_MAGIC, 20, 10, 20);
    suffixWeapon->AddSuffix(1); // Add a simple suffix
    float suffixScore = gearScoring->CalculateItemScore(*suffixWeapon);
    
    // Add both prefix and suffix
    auto bothWeapon = CreateTestWeapon(ITEM_QUALITY_RARE, 20, 10, 20);
    bothWeapon->AddPrefix(1); // Add a simple prefix
    bothWeapon->AddSuffix(1); // Add a simple suffix
    float bothScore = gearScoring->CalculateItemScore(*bothWeapon);
    
    // Verify that affixes increase the score
    EXPECT_GT(prefixScore, baseScore) << "Adding a prefix should increase the item score";
    EXPECT_GT(suffixScore, baseScore) << "Adding a suffix should increase the item score";
    EXPECT_GT(bothScore, prefixScore) << "Adding both prefix and suffix should increase the score more than just a prefix";
    EXPECT_GT(bothScore, suffixScore) << "Adding both prefix and suffix should increase the score more than just a suffix";
}

TEST_F(GearScoreTest, ItemLevelAffectsScore) {
    // Test that item level affects the score
    
    // Create weapons with different item levels
    auto lowLevelWeapon = CreateTestWeapon(ITEM_QUALITY_MAGIC, 10, 10, 20);
    auto midLevelWeapon = CreateTestWeapon(ITEM_QUALITY_MAGIC, 30, 10, 20);
    auto highLevelWeapon = CreateTestWeapon(ITEM_QUALITY_MAGIC, 50, 10, 20);
    
    // Calculate scores
    float lowLevelScore = gearScoring->CalculateItemScore(*lowLevelWeapon);
    float midLevelScore = gearScoring->CalculateItemScore(*midLevelWeapon);
    float highLevelScore = gearScoring->CalculateItemScore(*highLevelWeapon);
    
    // Verify that higher level items have higher scores
    EXPECT_GT(midLevelScore, lowLevelScore) << "Higher level item should have higher score";
    EXPECT_GT(highLevelScore, midLevelScore) << "Higher level item should have higher score";
}

TEST_F(GearScoreTest, DamageAffectsWeaponScore) {
    // Test that damage affects weapon score
    
    // Create weapons with different damage values
    auto lowDamageWeapon = CreateTestWeapon(ITEM_QUALITY_MAGIC, 20, 5, 10);
    auto midDamageWeapon = CreateTestWeapon(ITEM_QUALITY_MAGIC, 20, 10, 20);
    auto highDamageWeapon = CreateTestWeapon(ITEM_QUALITY_MAGIC, 20, 20, 40);
    
    // Calculate scores
    float lowDamageScore = gearScoring->CalculateItemScore(*lowDamageWeapon);
    float midDamageScore = gearScoring->CalculateItemScore(*midDamageWeapon);
    float highDamageScore = gearScoring->CalculateItemScore(*highDamageWeapon);
    
    // Verify that higher damage weapons have higher scores
    EXPECT_GT(midDamageScore, lowDamageScore) << "Higher damage weapon should have higher score";
    EXPECT_GT(highDamageScore, midDamageScore) << "Higher damage weapon should have higher score";
}

TEST_F(GearScoreTest, ArmorClassAffectsArmorScore) {
    // Test that armor class affects armor score
    
    // Create armors with different AC values
    auto lowACarmor = CreateTestArmor(ITEM_QUALITY_MAGIC, 20, 10);
    auto midACarmor = CreateTestArmor(ITEM_QUALITY_MAGIC, 20, 20);
    auto highACarmor = CreateTestArmor(ITEM_QUALITY_MAGIC, 20, 40);
    
    // Calculate scores
    float lowACscore = gearScoring->CalculateItemScore(*lowACarmor);
    float midACscore = gearScoring->CalculateItemScore(*midACarmor);
    float highACscore = gearScoring->CalculateItemScore(*highACarmor);
    
    // Verify that higher AC armors have higher scores
    EXPECT_GT(midACscore, lowACscore) << "Higher AC armor should have higher score";
    EXPECT_GT(highACscore, midACscore) << "Higher AC armor should have higher score";
}

} // namespace
} // namespace devilution
