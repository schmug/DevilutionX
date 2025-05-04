/**
 * @file gear_level_ui.h
 *
 * Interface for UI elements to display the player's gear level.
 */
#pragma once

#include <string>

#include "player.h"
#include "engine/render/text_render.hpp"
#include "gear/gear_manager.h"

namespace devilution {

/**
 * @brief Enum for gear level display style
 */
enum class GearLevelDisplayStyle {
    Numeric,    // Simple numeric display
    Stars,      // Star rating (1-5 stars)
    ColorCoded, // Color-coded numeric display
    Tier        // Tier-based display (Basic, Magic, Rare, etc.)
};

/**
 * @brief Structure for gear level display configuration
 */
struct GearLevelDisplayConfig {
    bool showInCharScreen;    // Whether to show gear level in character screen
    bool showInInventory;     // Whether to show gear level in inventory
    bool showInItemCompare;   // Whether to show gear level when comparing items
    bool showChangeIndicator; // Whether to show up/down indicators for changes
    GearLevelDisplayStyle style; // Display style to use
    
    // Default constructor
    GearLevelDisplayConfig()
        : showInCharScreen(true), showInInventory(true), showInItemCompare(true),
          showChangeIndicator(true), style(GearLevelDisplayStyle::ColorCoded)
    {
    }
};

/**
 * @brief Class for managing gear level UI elements
 */
class GearLevelUI {
public:
    /**
     * @brief Gets the singleton instance
     * @return Reference to the singleton instance
     */
    static GearLevelUI& GetInstance();
    
    /**
     * @brief Initializes the gear level UI
     */
    void Initialize();
    
    /**
     * @brief Renders the gear level in the character screen
     * @param player The player to render gear level for
     * @param x The x-coordinate to render at
     * @param y The y-coordinate to render at
     */
    void RenderCharScreenGearLevel(const Player &player, int x, int y);
    
    /**
     * @brief Renders the gear level in the inventory
     * @param player The player to render gear level for
     * @param x The x-coordinate to render at
     * @param y The y-coordinate to render at
     */
    void RenderInventoryGearLevel(const Player &player, int x, int y);
    
    /**
     * @brief Renders the gear level comparison between two items
     * @param player The player to render comparison for
     * @param currentItem The currently equipped item
     * @param newItem The new item being compared
     * @param slot The equipment slot being compared
     * @param x The x-coordinate to render at
     * @param y The y-coordinate to render at
     */
    void RenderItemCompareGearLevel(const Player &player, const Item &currentItem, const Item &newItem, inv_body_loc slot, int x, int y);
    
    /**
     * @brief Gets the display configuration
     * @return Reference to the display configuration
     */
    GearLevelDisplayConfig& GetConfig();
    
    /**
     * @brief Sets the display configuration
     * @param config The new configuration
     */
    void SetConfig(const GearLevelDisplayConfig &config);
    
    /**
     * @brief Gets a color for a gear level
     * @param gearLevel The gear level to get a color for
     * @return The color for the gear level
     */
    UiFlags GetGearLevelColor(float gearLevel);
    
    /**
     * @brief Gets a star rating for a gear level
     * @param gearLevel The gear level to get a rating for
     * @return The star rating (1-5)
     */
    int GetGearLevelStars(float gearLevel);
    
    /**
     * @brief Gets a tier name for a gear level
     * @param gearLevel The gear level to get a tier for
     * @return The tier name
     */
    std::string GetGearLevelTier(float gearLevel);
    
    /**
     * @brief Formats a gear level for display
     * @param gearLevel The gear level to format
     * @param style The display style to use
     * @return The formatted gear level string
     */
    std::string FormatGearLevel(float gearLevel, GearLevelDisplayStyle style);
    
private:
    GearLevelUI() = default;
    ~GearLevelUI() = default;
    
    GearLevelUI(const GearLevelUI&) = delete;
    GearLevelUI& operator=(const GearLevelUI&) = delete;
    
    /**
     * @brief Renders a gear level value
     * @param gearLevel The gear level to render
     * @param x The x-coordinate to render at
     * @param y The y-coordinate to render at
     * @param style The display style to use
     */
    void RenderGearLevel(float gearLevel, int x, int y, GearLevelDisplayStyle style);
    
    /**
     * @brief Renders a gear level change indicator
     * @param oldGearLevel The old gear level
     * @param newGearLevel The new gear level
     * @param x The x-coordinate to render at
     * @param y The y-coordinate to render at
     */
    void RenderGearLevelChange(float oldGearLevel, float newGearLevel, int x, int y);
    
    /**
     * @brief Renders stars for a gear level
     * @param stars The number of stars to render
     * @param x The x-coordinate to render at
     * @param y The y-coordinate to render at
     */
    void RenderStars(int stars, int x, int y);
    
    // Display configuration
    GearLevelDisplayConfig config;
    
    // Flag to track initialization
    bool initialized = false;
};

} // namespace devilution
