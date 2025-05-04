/**
 * @file difficulty_indicators.h
 *
 * Interface for visual indicators to display difficulty levels on the minimap.
 */
#pragma once

#include <array>
#include <vector>

#include "engine/render/cel_render.hpp"
#include "engine/render/text_render.hpp"
#include "engine/point.hpp"
#include "difficulty/difficulty_manager.h"

namespace devilution {

/**
 * @brief Enum for difficulty indicator style
 */
enum class DifficultyIndicatorStyle {
    ColorGradient,  // Color gradient from green (easy) to red (hard)
    NumberScale,    // Numeric scale (1-5)
    IconSet,        // Different icons for each difficulty level
    PulseIntensity  // Pulsing indicator with varying intensity
};

/**
 * @brief Structure for difficulty indicator configuration
 */
struct DifficultyIndicatorConfig {
    bool showOnMinimap;          // Whether to show indicators on the minimap
    bool showOnMainMap;          // Whether to show indicators on the main map
    bool showAreaTransitions;    // Whether to highlight area transitions with different difficulty
    bool showLegend;             // Whether to show a legend for the indicators
    DifficultyIndicatorStyle style; // Display style to use
    
    // Default constructor
    DifficultyIndicatorConfig()
        : showOnMinimap(true), showOnMainMap(true), showAreaTransitions(true),
          showLegend(true), style(DifficultyIndicatorStyle::ColorGradient)
    {
    }
};

/**
 * @brief Structure for area difficulty information
 */
struct AreaDifficultyInfo {
    int areaId;                  // ID of the area
    float baseDifficulty;        // Base difficulty of the area
    float currentDifficulty;     // Current difficulty of the area (with player influence)
    Point position;              // Position on the map
    bool isCurrentArea;          // Whether this is the player's current area
    
    // Default constructor
    AreaDifficultyInfo()
        : areaId(0), baseDifficulty(1.0f), currentDifficulty(1.0f),
          position(0, 0), isCurrentArea(false)
    {
    }
};

/**
 * @brief Class for managing difficulty indicators on the minimap
 */
class DifficultyIndicators {
public:
    /**
     * @brief Gets the singleton instance
     * @return Reference to the singleton instance
     */
    static DifficultyIndicators& GetInstance();
    
    /**
     * @brief Initializes the difficulty indicators
     */
    void Initialize();
    
    /**
     * @brief Updates the difficulty indicators
     * @param gameTime Current game time
     */
    void Update(float gameTime);
    
    /**
     * @brief Renders difficulty indicators on the minimap
     */
    void RenderMinimapIndicators();
    
    /**
     * @brief Renders difficulty indicators on the main map
     */
    void RenderMainMapIndicators();
    
    /**
     * @brief Renders a legend for the difficulty indicators
     * @param x The x-coordinate to render at
     * @param y The y-coordinate to render at
     */
    void RenderLegend(int x, int y);
    
    /**
     * @brief Gets the display configuration
     * @return Reference to the display configuration
     */
    DifficultyIndicatorConfig& GetConfig();
    
    /**
     * @brief Sets the display configuration
     * @param config The new configuration
     */
    void SetConfig(const DifficultyIndicatorConfig &config);
    
    /**
     * @brief Gets a color for a difficulty level
     * @param difficulty The difficulty level to get a color for
     * @return The color for the difficulty level
     */
    UiFlags GetDifficultyColor(float difficulty);
    
    /**
     * @brief Gets an icon index for a difficulty level
     * @param difficulty The difficulty level to get an icon for
     * @return The icon index for the difficulty level
     */
    int GetDifficultyIconIndex(float difficulty);
    
    /**
     * @brief Gets a numeric scale value for a difficulty level
     * @param difficulty The difficulty level to get a scale value for
     * @return The scale value (1-5) for the difficulty level
     */
    int GetDifficultyScale(float difficulty);
    
    /**
     * @brief Gets the pulse intensity for a difficulty level
     * @param difficulty The difficulty level to get pulse intensity for
     * @param gameTime Current game time for animation
     * @return The pulse intensity (0.0-1.0) for the difficulty level
     */
    float GetDifficultyPulseIntensity(float difficulty, float gameTime);
    
    /**
     * @brief Gets the difficulty information for all areas
     * @return Vector of area difficulty information
     */
    const std::vector<AreaDifficultyInfo>& GetAreaDifficultyInfo() const;
    
    /**
     * @brief Gets the difficulty information for a specific area
     * @param areaId The ID of the area
     * @return Pointer to the area difficulty information, or nullptr if not found
     */
    const AreaDifficultyInfo* GetAreaDifficultyInfo(int areaId) const;
    
    /**
     * @brief Sets the difficulty information for an area
     * @param areaInfo The area difficulty information
     */
    void SetAreaDifficultyInfo(const AreaDifficultyInfo &areaInfo);
    
    /**
     * @brief Updates the difficulty information for all areas
     */
    void UpdateAreaDifficultyInfo();
    
private:
    DifficultyIndicators() = default;
    ~DifficultyIndicators() = default;
    
    DifficultyIndicators(const DifficultyIndicators&) = delete;
    DifficultyIndicators& operator=(const DifficultyIndicators&) = delete;
    
    /**
     * @brief Renders a difficulty indicator
     * @param difficulty The difficulty level to render
     * @param x The x-coordinate to render at
     * @param y The y-coordinate to render at
     * @param style The display style to use
     * @param gameTime Current game time for animation
     * @param isCurrentArea Whether this is the player's current area
     */
    void RenderDifficultyIndicator(float difficulty, int x, int y, DifficultyIndicatorStyle style, float gameTime, bool isCurrentArea);
    
    /**
     * @brief Renders a color gradient indicator
     * @param difficulty The difficulty level to render
     * @param x The x-coordinate to render at
     * @param y The y-coordinate to render at
     * @param isCurrentArea Whether this is the player's current area
     */
    void RenderColorGradientIndicator(float difficulty, int x, int y, bool isCurrentArea);
    
    /**
     * @brief Renders a number scale indicator
     * @param difficulty The difficulty level to render
     * @param x The x-coordinate to render at
     * @param y The y-coordinate to render at
     * @param isCurrentArea Whether this is the player's current area
     */
    void RenderNumberScaleIndicator(float difficulty, int x, int y, bool isCurrentArea);
    
    /**
     * @brief Renders an icon set indicator
     * @param difficulty The difficulty level to render
     * @param x The x-coordinate to render at
     * @param y The y-coordinate to render at
     * @param isCurrentArea Whether this is the player's current area
     */
    void RenderIconSetIndicator(float difficulty, int x, int y, bool isCurrentArea);
    
    /**
     * @brief Renders a pulse intensity indicator
     * @param difficulty The difficulty level to render
     * @param x The x-coordinate to render at
     * @param y The y-coordinate to render at
     * @param gameTime Current game time for animation
     * @param isCurrentArea Whether this is the player's current area
     */
    void RenderPulseIntensityIndicator(float difficulty, int x, int y, float gameTime, bool isCurrentArea);
    
    // Display configuration
    DifficultyIndicatorConfig config;
    
    // Area difficulty information
    std::vector<AreaDifficultyInfo> areaDifficultyInfo;
    
    // Current game time for animations
    float currentGameTime = 0.0f;
    
    // Flag to track initialization
    bool initialized = false;
};

} // namespace devilution
