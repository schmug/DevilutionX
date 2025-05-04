#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "data/file.hpp"
#include "engine/assets.hpp"
#include "utils/log.hpp"
#include "utils/paths.h"

namespace devilution {
namespace testing {

// Forward declaration of LoadMonsterDataFromFile to avoid including monstdat.cpp
void LoadMonsterDataFromFile(const std::string_view filename, ankerl::unordered_dense::map<std::string, size_t> &spritePathToId);

class MonsterDataLoadTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize logging
        LogInit();
    }

    void TearDown() override {
        // Clean up after tests
        LogClose();
    }
};

TEST_F(MonsterDataLoadTest, CanFindMonsterDataFile) {
    // Test that we can find the monster data file
    const std::vector<std::string> monsterDataPaths = {
        "assets/txtdata/monsters/monstdat.tsv",
        "txtdata/monsters/monstdat.tsv",
        "monsters/monstdat.tsv",
        "monstdat.tsv"
    };
    
    bool fileFound = false;
    std::string foundPath;
    
    for (const auto& path : monsterDataPaths) {
        AssetRef ref = FindAsset(path);
        if (ref.ok()) {
            fileFound = true;
            foundPath = path;
            break;
        }
    }
    
    ASSERT_TRUE(fileFound) << "Monster data file not found in any of the search paths";
    std::cout << "Found monster data file at: " << foundPath << std::endl;
}

TEST_F(MonsterDataLoadTest, CanOpenMonsterDataFile) {
    // Test that we can open the monster data file
    const std::vector<std::string> monsterDataPaths = {
        "assets/txtdata/monsters/monstdat.tsv",
        "txtdata/monsters/monstdat.tsv",
        "monsters/monstdat.tsv",
        "monstdat.tsv"
    };
    
    bool fileOpened = false;
    std::string openedPath;
    
    for (const auto& path : monsterDataPaths) {
        AssetRef ref = FindAsset(path);
        if (ref.ok()) {
            AssetHandle handle = OpenAsset(std::move(ref));
            if (handle.ok()) {
                fileOpened = true;
                openedPath = path;
                break;
            }
        }
    }
    
    ASSERT_TRUE(fileOpened) << "Monster data file could not be opened";
    std::cout << "Successfully opened monster data file at: " << openedPath << std::endl;
}

TEST_F(MonsterDataLoadTest, CanReadMonsterDataFile) {
    // Test that we can read the monster data file
    const std::vector<std::string> monsterDataPaths = {
        "assets/txtdata/monsters/monstdat.tsv",
        "txtdata/monsters/monstdat.tsv",
        "monsters/monstdat.tsv",
        "monstdat.tsv"
    };
    
    bool fileRead = false;
    std::string readPath;
    std::string fileContent;
    
    for (const auto& path : monsterDataPaths) {
        AssetRef ref = FindAsset(path);
        if (ref.ok()) {
            AssetHandle handle = OpenAsset(std::move(ref));
            if (handle.ok()) {
                const size_t size = handle.size();
                if (size > 0) {
                    std::vector<char> buffer(size);
                    if (handle.read(buffer.data(), size)) {
                        fileContent = std::string(buffer.data(), size);
                        fileRead = true;
                        readPath = path;
                        break;
                    }
                }
            }
        }
    }
    
    ASSERT_TRUE(fileRead) << "Monster data file could not be read";
    ASSERT_FALSE(fileContent.empty()) << "Monster data file is empty";
    
    // Verify that the file content looks like a TSV file
    ASSERT_NE(fileContent.find('\t'), std::string::npos) << "File does not contain tab characters (not a TSV file)";
    ASSERT_NE(fileContent.find('\n'), std::string::npos) << "File does not contain newline characters (not a TSV file)";
    
    std::cout << "Successfully read monster data file at: " << readPath << std::endl;
    std::cout << "File size: " << fileContent.size() << " bytes" << std::endl;
    std::cout << "First 100 characters: " << fileContent.substr(0, 100) << "..." << std::endl;
}

TEST_F(MonsterDataLoadTest, CanLoadMonsterDataFromFile) {
    // Test that we can load the monster data from the file
    const std::vector<std::string> monsterDataPaths = {
        "assets/txtdata/monsters/monstdat.tsv",
        "txtdata/monsters/monstdat.tsv",
        "monsters/monstdat.tsv",
        "monstdat.tsv"
    };
    
    bool fileLoaded = false;
    std::string loadedPath;
    
    for (const auto& path : monsterDataPaths) {
        try {
            ankerl::unordered_dense::map<std::string, size_t> spritePathToId;
            LoadMonsterDataFromFile(path, spritePathToId);
            fileLoaded = true;
            loadedPath = path;
            break;
        } catch (const std::runtime_error& e) {
            std::cout << "Failed to load monster data from " << path << ": " << e.what() << std::endl;
        }
    }
    
    ASSERT_TRUE(fileLoaded) << "Monster data could not be loaded from any path";
    std::cout << "Successfully loaded monster data from: " << loadedPath << std::endl;
}

} // namespace testing
} // namespace devilution
