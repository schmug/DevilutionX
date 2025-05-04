/**
 * @file gear_level_ui.cpp
 *
 * Implementation of UI elements to display the player's gear level.
 */
#include "ui/gear_level_ui.h"

#include <algorithm>
#include <cmath>
#include <sstream>

#include "utils/log.hpp"
#include "engine/render/text_render.hpp"
#include "engine/render/cel_render.hpp"
#include "engine/palette.h"

namespace devilution {

GearLevelUI& GearLevelUI::GetInstance()
{
    static GearLevelUI instance;
    return instance;
}

void GearLevelUI::Initialize()
{
    if (initialized) {
        return;
    }
    
    // Initialize the gear level manager
    GearLevelManager::GetInstance().Initialize();
    
    // Set default configuration
    config = GearLevelDisplayConfig();
    
    initialized = true;
    LogVerbose("Gear Level UI initialized");
}

void GearLevelUI::RenderCharScreenGearLevel(const Player &player, int x, int y)
{
    // Skip if disabled in config
    if (!config.showInCharScreen) {
        return;
    }
    
    // Get the player's gear level
    float gearLevel = GearLevelManager::GetInstance().GetCurrentDifficulty(player);
    
    // Render a label
    DrawString(x, y, "Gear Level:", UiFlags::ColorWhitegold);
    
    // Render the gear level
    RenderGearLevel(gearLevel, x + 80, y, config.style);
}

void GearLevelUI::RenderInventoryGearLevel(const Player &player, int x, int y)
{
    // Skip if disabled in config
    if (!config.showInInventory) {
        return;
    }
    
    // Get the player's gear level
    float gearLevel = GearLevelManager::GetInstance().GetCurrentDifficulty(player);
    
    // Render a label
    DrawString(x, y, "GL:", UiFlags::ColorWhitegold);
    
    // Render the gear level
    RenderGearLevel(gearLevel, x + 20, y, config.style);
}

void GearLevelUI::RenderItemCompareGearLevel(const Player &player, const Item &currentItem, const Item &newItem, inv_body_loc slot, int x, int y)
{
    // Skip if disabled in config
    if (!config.showInItemCompare) {
        return;
    }
    
    // Get the gear level manager
    GearLevelManager &manager = GearLevelManager::GetInstance();
    
    // Get the current gear level
    float currentGearLevel = manager.GetCurrentDifficulty(player);
    
    // Get the potential gear level with the new item
    float potentialGearLevel = manager.GetPotentialGearLevel(player, newItem, slot);
    
    // Render a label
    DrawString(x, y, "Gear Level:", UiFlags::ColorWhitegold);
    
    // Render the current gear level
    RenderGearLevel(currentGearLevel, x + 80, y, config.style);
    
    // Render an arrow
    DrawString(x + 130, y, "->", UiFlags::ColorWhitegold);
    
    // Render the potential gear level
    RenderGearLevel(potentialGearLevel, x + 150, y, config.style);
    
    // Render the change indicator
    if (config.showChangeIndicator) {
        RenderGearLevelChange(currentGearLevel, potentialGearLevel, x + 200, y);
    }
}

GearLevelDisplayConfig& GearLevelUI::GetConfig()
{
    return config;
}

void GearLevelUI::SetConfig(const GearLevelDisplayConfig &newConfig)
{
    config = newConfig;
}

UiFlags GearLevelUI::GetGearLevelColor(float gearLevel)
{
    // Color coding based on gear level
    if (gearLevel < 20.0f) {
        return UiFlags::ColorWhite; // White for low gear level
    } else if (gearLevel < 40.0f) {
        return UiFlags::ColorBlue; // Blue for moderate gear level
    } else if (gearLevel < 60.0f) {
        return UiFlags::ColorYellow; // Yellow for good gear level
    } else if (gearLevel < 80.0f) {
        return UiFlags::ColorOrange; // Orange for very good gear level
    } else {
        return UiFlags::ColorRed; // Red for excellent gear level
    }
}

int GearLevelUI::GetGearLevelStars(float gearLevel)
{
    // Star rating based on gear level (1-5 stars)
    if (gearLevel < 20.0f) {
        return 1; // 1 star for low gear level
    } else if (gearLevel < 40.0f) {
        return 2; // 2 stars for moderate gear level
    } else if (gearLevel < 60.0f) {
        return 3; // 3 stars for good gear level
    } else if (gearLevel < 80.0f) {
        return 4; // 4 stars for very good gear level
    } else {
        return 5; // 5 stars for excellent gear level
    }
}

std::string GearLevelUI::GetGearLevelTier(float gearLevel)
{
    // Tier name based on gear level
    if (gearLevel < 20.0f) {
        return "Basic"; // Basic tier for low gear level
    } else if (gearLevel < 40.0f) {
        return "Magic"; // Magic tier for moderate gear level
    } else if (gearLevel < 60.0f) {
        return "Rare"; // Rare tier for good gear level
    } else if (gearLevel < 80.0f) {
        return "Epic"; // Epic tier for very good gear level
    } else {
        return "Legendary"; // Legendary tier for excellent gear level
    }
}

std::string GearLevelUI::FormatGearLevel(float gearLevel, GearLevelDisplayStyle style)
{
    std::stringstream ss;
    
    switch (style) {
    case GearLevelDisplayStyle::Numeric:
        // Simple numeric display
        ss << std::fixed << std::setprecision(1) << gearLevel;
        break;
        
    case GearLevelDisplayStyle::Stars:
        // Star rating (handled separately in rendering)
        ss << GetGearLevelStars(gearLevel) << "/5";
        break;
        
    case GearLevelDisplayStyle::ColorCoded:
        // Color-coded numeric display (color handled separately in rendering)
        ss << std::fixed << std::setprecision(1) << gearLevel;
        break;
        
    case GearLevelDisplayStyle::Tier:
        // Tier-based display
        ss << GetGearLevelTier(gearLevel);
        break;
    }
    
    return ss.str();
}

void GearLevelUI::RenderGearLevel(float gearLevel, int x, int y, GearLevelDisplayStyle style)
{
    if (style == GearLevelDisplayStyle::Stars) {
        // Render stars
        int stars = GetGearLevelStars(gearLevel);
        RenderStars(stars, x, y);
    } else {
        // Format the gear level
        std::string formattedGearLevel = FormatGearLevel(gearLevel, style);
        
        // Get the color
        UiFlags color = (style == GearLevelDisplayStyle::ColorCoded)
            ? GetGearLevelColor(gearLevel)
            : UiFlags::ColorWhite;
        
        // Render the gear level
        DrawString(x, y, formattedGearLevel.c_str(), color);
    }
}

void GearLevelUI::RenderGearLevelChange(float oldGearLevel, float newGearLevel, int x, int y)
{
    // Calculate the difference
    float diff = newGearLevel - oldGearLevel;
    
    // Skip if no change
    if (std::abs(diff) < 0.1f) {
        return;
    }
    
    // Format the difference
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1);
    
    if (diff > 0.0f) {
        ss << "+" << diff;
        DrawString(x, y, ss.str().c_str(), UiFlags::ColorGreen);
    } else {
        ss << diff;
        DrawString(x, y, ss.str().c_str(), UiFlags::ColorRed);
    }
}

void GearLevelUI::RenderStars(int stars, int x, int y)
{
    // Ensure stars is in range 1-5
    stars = std::clamp(stars, 1, 5);
    
    // Render filled stars
    for (int i = 0; i < stars; i++) {
        // TODO: Replace with actual star sprite rendering
        // For now, just use asterisks
        DrawString(x + i * 10, y, "*", UiFlags::ColorYellow);
    }
    
    // Render empty stars
    for (int i = stars; i < 5; i++) {
        // TODO: Replace with actual empty star sprite rendering
        // For now, just use dots
        DrawString(x + i * 10, y, ".", UiFlags::ColorWhite);
    }
}

} // namespace devilution
