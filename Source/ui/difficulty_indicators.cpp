/**
 * @file difficulty_indicators.cpp
 *
 * Implementation of visual indicators to display difficulty levels on the minimap.
 */
#include "ui/difficulty_indicators.h"

#include <algorithm>
#include <cmath>
#include <sstream>

#include "utils/log.hpp"
#include "engine/render/text_render.hpp"
#include "engine/render/cel_render.hpp"
#include "engine/palette.h"
#include "player.h"
#include "minitext.h"
#include "automap.h"

namespace devilution {

DifficultyIndicators& DifficultyIndicators::GetInstance()
{
    static DifficultyIndicators instance;
    return instance;
}

void DifficultyIndicators::Initialize()
{
    if (initialized) {
        return;
    }
    
    // Initialize the difficulty manager
    DifficultyManager::GetInstance().Initialize();
    
    // Set default configuration
    config = DifficultyIndicatorConfig();
    
    // Initialize area difficulty information
    UpdateAreaDifficultyInfo();
    
    initialized = true;
    LogVerbose("Difficulty Indicators initialized");
}

void DifficultyIndicators::Update(float gameTime)
{
    // Update current game time for animations
    currentGameTime = gameTime;
    
    // Update area difficulty information periodically
    static float lastUpdateTime = 0.0f;
    if (gameTime - lastUpdateTime > 1.0f) { // Update every second
        UpdateAreaDifficultyInfo();
        lastUpdateTime = gameTime;
    }
}

void DifficultyIndicators::RenderMinimapIndicators()
{
    // Skip if disabled in config
    if (!config.showOnMinimap) {
        return;
    }
    
    // Only render if the minimap is visible
    if (!AutomapActive) {
        return;
    }
    
    // Render indicators for each area
    for (const auto &areaInfo : areaDifficultyInfo) {
        // Convert area position to minimap coordinates
        int x = areaInfo.position.x;
        int y = areaInfo.position.y;
        
        // Render the difficulty indicator
        RenderDifficultyIndicator(
            areaInfo.currentDifficulty,
            x, y,
            config.style,
            currentGameTime,
            areaInfo.isCurrentArea
        );
    }
    
    // Render legend if enabled
    if (config.showLegend) {
        // Position the legend in the bottom-right corner of the minimap
        RenderLegend(AutomapX + AutomapXPos - 100, AutomapY + AutomapYPos - 40);
    }
}

void DifficultyIndicators::RenderMainMapIndicators()
{
    // Skip if disabled in config
    if (!config.showOnMainMap) {
        return;
    }
    
    // Only render if the main map is visible
    // TODO: Add condition for main map visibility
    
    // Render indicators for each area
    for (const auto &areaInfo : areaDifficultyInfo) {
        // Convert area position to main map coordinates
        int x = areaInfo.position.x * 2; // Scale for main map
        int y = areaInfo.position.y * 2; // Scale for main map
        
        // Render the difficulty indicator
        RenderDifficultyIndicator(
            areaInfo.currentDifficulty,
            x, y,
            config.style,
            currentGameTime,
            areaInfo.isCurrentArea
        );
    }
    
    // Render legend if enabled
    if (config.showLegend) {
        // Position the legend in the bottom-right corner of the main map
        // TODO: Adjust coordinates for main map
        RenderLegend(100, 100);
    }
}

void DifficultyIndicators::RenderLegend(int x, int y)
{
    // Render legend title
    DrawString(x, y, "Difficulty:", UiFlags::ColorWhitegold);
    y += 12;
    
    // Render legend entries based on style
    switch (config.style) {
    case DifficultyIndicatorStyle::ColorGradient:
        // Render color gradient legend
        for (int i = 0; i < 5; i++) {
            float difficulty = i * 25.0f; // 0, 25, 50, 75, 100
            UiFlags color = GetDifficultyColor(difficulty);
            DrawString(x, y + i * 10, "■", color);
            
            std::string label;
            switch (i) {
            case 0: label = "Easy"; break;
            case 1: label = "Normal"; break;
            case 2: label = "Hard"; break;
            case 3: label = "Very Hard"; break;
            case 4: label = "Extreme"; break;
            }
            
            DrawString(x + 15, y + i * 10, label.c_str(), UiFlags::ColorWhite);
        }
        break;
        
    case DifficultyIndicatorStyle::NumberScale:
        // Render number scale legend
        for (int i = 1; i <= 5; i++) {
            float difficulty = (i - 1) * 25.0f; // 0, 25, 50, 75, 100
            
            std::stringstream ss;
            ss << i;
            DrawString(x, y + (i - 1) * 10, ss.str().c_str(), UiFlags::ColorWhite);
            
            std::string label;
            switch (i) {
            case 1: label = "Easy"; break;
            case 2: label = "Normal"; break;
            case 3: label = "Hard"; break;
            case 4: label = "Very Hard"; break;
            case 5: label = "Extreme"; break;
            }
            
            DrawString(x + 15, y + (i - 1) * 10, label.c_str(), UiFlags::ColorWhite);
        }
        break;
        
    case DifficultyIndicatorStyle::IconSet:
        // Render icon set legend
        for (int i = 0; i < 5; i++) {
            float difficulty = i * 25.0f; // 0, 25, 50, 75, 100
            int iconIndex = GetDifficultyIconIndex(difficulty);
            
            // TODO: Replace with actual icon rendering
            // For now, just use a placeholder
            std::stringstream ss;
            ss << "[" << iconIndex << "]";
            DrawString(x, y + i * 10, ss.str().c_str(), UiFlags::ColorWhite);
            
            std::string label;
            switch (i) {
            case 0: label = "Easy"; break;
            case 1: label = "Normal"; break;
            case 2: label = "Hard"; break;
            case 3: label = "Very Hard"; break;
            case 4: label = "Extreme"; break;
            }
            
            DrawString(x + 25, y + i * 10, label.c_str(), UiFlags::ColorWhite);
        }
        break;
        
    case DifficultyIndicatorStyle::PulseIntensity:
        // Render pulse intensity legend
        for (int i = 0; i < 5; i++) {
            float difficulty = i * 25.0f; // 0, 25, 50, 75, 100
            
            // TODO: Replace with actual pulse rendering
            // For now, just use a placeholder
            DrawString(x, y + i * 10, "●", UiFlags::ColorWhite);
            
            std::string label;
            switch (i) {
            case 0: label = "Easy"; break;
            case 1: label = "Normal"; break;
            case 2: label = "Hard"; break;
            case 3: label = "Very Hard"; break;
            case 4: label = "Extreme"; break;
            }
            
            DrawString(x + 15, y + i * 10, label.c_str(), UiFlags::ColorWhite);
        }
        break;
    }
}

DifficultyIndicatorConfig& DifficultyIndicators::GetConfig()
{
    return config;
}

void DifficultyIndicators::SetConfig(const DifficultyIndicatorConfig &newConfig)
{
    config = newConfig;
}

UiFlags DifficultyIndicators::GetDifficultyColor(float difficulty)
{
    // Color coding based on difficulty level
    if (difficulty < 20.0f) {
        return UiFlags::ColorGreen; // Green for easy difficulty
    } else if (difficulty < 40.0f) {
        return UiFlags::ColorBlue; // Blue for normal difficulty
    } else if (difficulty < 60.0f) {
        return UiFlags::ColorYellow; // Yellow for hard difficulty
    } else if (difficulty < 80.0f) {
        return UiFlags::ColorOrange; // Orange for very hard difficulty
    } else {
        return UiFlags::ColorRed; // Red for extreme difficulty
    }
}

int DifficultyIndicators::GetDifficultyIconIndex(float difficulty)
{
    // Icon index based on difficulty level (0-4)
    if (difficulty < 20.0f) {
        return 0; // Easy icon
    } else if (difficulty < 40.0f) {
        return 1; // Normal icon
    } else if (difficulty < 60.0f) {
        return 2; // Hard icon
    } else if (difficulty < 80.0f) {
        return 3; // Very hard icon
    } else {
        return 4; // Extreme icon
    }
}

int DifficultyIndicators::GetDifficultyScale(float difficulty)
{
    // Scale value based on difficulty level (1-5)
    if (difficulty < 20.0f) {
        return 1; // Easy
    } else if (difficulty < 40.0f) {
        return 2; // Normal
    } else if (difficulty < 60.0f) {
        return 3; // Hard
    } else if (difficulty < 80.0f) {
        return 4; // Very hard
    } else {
        return 5; // Extreme
    }
}

float DifficultyIndicators::GetDifficultyPulseIntensity(float difficulty, float gameTime)
{
    // Base pulse frequency based on difficulty
    float frequency = 0.5f + (difficulty / 100.0f) * 2.0f; // 0.5 to 2.5 Hz
    
    // Calculate pulse intensity (0.0-1.0)
    float pulse = (std::sin(gameTime * frequency * 6.28f) + 1.0f) / 2.0f;
    
    // Scale pulse intensity based on difficulty
    float baseIntensity = 0.3f + (difficulty / 100.0f) * 0.7f; // 0.3 to 1.0
    
    return baseIntensity * pulse;
}

const std::vector<AreaDifficultyInfo>& DifficultyIndicators::GetAreaDifficultyInfo() const
{
    return areaDifficultyInfo;
}

const AreaDifficultyInfo* DifficultyIndicators::GetAreaDifficultyInfo(int areaId) const
{
    // Find area info by ID
    for (const auto &areaInfo : areaDifficultyInfo) {
        if (areaInfo.areaId == areaId) {
            return &areaInfo;
        }
    }
    
    return nullptr;
}

void DifficultyIndicators::SetAreaDifficultyInfo(const AreaDifficultyInfo &areaInfo)
{
    // Find existing area info
    for (auto &existingAreaInfo : areaDifficultyInfo) {
        if (existingAreaInfo.areaId == areaInfo.areaId) {
            // Update existing area info
            existingAreaInfo = areaInfo;
            return;
        }
    }
    
    // Add new area info
    areaDifficultyInfo.push_back(areaInfo);
}

void DifficultyIndicators::UpdateAreaDifficultyInfo()
{
    // Clear existing area info
    areaDifficultyInfo.clear();
    
    // Get the current player
    const Player &player = plr[myplr];
    
    // Get the current area ID
    int currentAreaId = player.plrlevel;
    
    // Get the difficulty manager
    DifficultyManager &diffManager = DifficultyManager::GetInstance();
    
    // TODO: This is a simplified implementation that creates mock area data
    // In a real implementation, this would use actual area data from the game
    
    // Create mock area data for testing
    for (int i = 0; i < 16; i++) { // Assuming 16 areas in the game
        AreaDifficultyInfo areaInfo;
        areaInfo.areaId = i;
        
        // Set base difficulty based on area ID
        // Lower areas have lower base difficulty
        areaInfo.baseDifficulty = 10.0f + (i * 5.0f);
        
        // Calculate current difficulty based on player's gear level
        float playerInfluence = diffManager.CalculateDifficulty(player);
        areaInfo.currentDifficulty = areaInfo.baseDifficulty * (0.8f + (playerInfluence * 0.2f));
        
        // Set mock position on the minimap
        // This would use actual area positions in a real implementation
        int x = 50 + (i % 4) * 50;
        int y = 50 + (i / 4) * 50;
        areaInfo.position = Point(x, y);
        
        // Set whether this is the current area
        areaInfo.isCurrentArea = (i == currentAreaId);
        
        // Add to the list
        areaDifficultyInfo.push_back(areaInfo);
    }
}

void DifficultyIndicators::RenderDifficultyIndicator(float difficulty, int x, int y, DifficultyIndicatorStyle style, float gameTime, bool isCurrentArea)
{
    // Render the indicator based on the selected style
    switch (style) {
    case DifficultyIndicatorStyle::ColorGradient:
        RenderColorGradientIndicator(difficulty, x, y, isCurrentArea);
        break;
        
    case DifficultyIndicatorStyle::NumberScale:
        RenderNumberScaleIndicator(difficulty, x, y, isCurrentArea);
        break;
        
    case DifficultyIndicatorStyle::IconSet:
        RenderIconSetIndicator(difficulty, x, y, isCurrentArea);
        break;
        
    case DifficultyIndicatorStyle::PulseIntensity:
        RenderPulseIntensityIndicator(difficulty, x, y, gameTime, isCurrentArea);
        break;
    }
    
    // If this is the current area, add a highlight
    if (isCurrentArea) {
        // TODO: Replace with actual highlight rendering
        // For now, just draw a simple marker
        DrawString(x - 5, y - 5, "+", UiFlags::ColorWhite);
    }
    
    // If area transitions are enabled, check for transitions
    if (config.showAreaTransitions) {
        // TODO: Implement area transition highlighting
    }
}

void DifficultyIndicators::RenderColorGradientIndicator(float difficulty, int x, int y, bool isCurrentArea)
{
    // Get the color based on difficulty
    UiFlags color = GetDifficultyColor(difficulty);
    
    // Render a colored square
    // TODO: Replace with actual square rendering
    // For now, just use a colored character
    DrawString(x, y, "■", color);
}

void DifficultyIndicators::RenderNumberScaleIndicator(float difficulty, int x, int y, bool isCurrentArea)
{
    // Get the scale value based on difficulty (1-5)
    int scale = GetDifficultyScale(difficulty);
    
    // Format the scale value
    std::stringstream ss;
    ss << scale;
    
    // Render the scale value
    DrawString(x, y, ss.str().c_str(), UiFlags::ColorWhite);
}

void DifficultyIndicators::RenderIconSetIndicator(float difficulty, int x, int y, bool isCurrentArea)
{
    // Get the icon index based on difficulty (0-4)
    int iconIndex = GetDifficultyIconIndex(difficulty);
    
    // TODO: Replace with actual icon rendering
    // For now, just use a placeholder
    std::stringstream ss;
    ss << "[" << iconIndex << "]";
    DrawString(x, y, ss.str().c_str(), UiFlags::ColorWhite);
}

void DifficultyIndicators::RenderPulseIntensityIndicator(float difficulty, int x, int y, float gameTime, bool isCurrentArea)
{
    // Get the pulse intensity based on difficulty and game time
    float intensity = GetDifficultyPulseIntensity(difficulty, gameTime);
    
    // Get the color based on difficulty
    UiFlags color = GetDifficultyColor(difficulty);
    
    // TODO: Replace with actual pulse rendering
    // For now, just use a placeholder
    DrawString(x, y, "●", color);
}

} // namespace devilution
