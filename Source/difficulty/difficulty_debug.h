/**
 * @file difficulty_debug.h
 *
 * Debug utilities for the difficulty transition system.
 */
#pragma once

#include <string>
#include <vector>

#include "player.h"
#include "difficulty/difficulty_manager.h"

namespace devilution {

/**
 * @brief Structure for difficulty transition data point
 */
struct DifficultyDataPoint {
    float time;
    float rawGearLevel;
    float targetDifficulty;
    float currentDifficulty;
    float overpowerFactor;
};

/**
 * @brief Class for debugging and visualizing difficulty transitions
 */
class DifficultyDebug {
public:
    /**
     * @brief Gets the singleton instance
     * @return Reference to the singleton instance
     */
    static DifficultyDebug& GetInstance();
    
    /**
     * @brief Initializes the debug utilities
     */
    void Initialize();
    
    /**
     * @brief Simulates a gear level change and records the difficulty transition
     * @param initialGearLevel The initial gear level
     * @param finalGearLevel The final gear level
     * @param duration The duration of the simulation
     * @param timeStep The time step for the simulation
     * @param params The transition parameters to use
     * @return A vector of data points from the simulation
     */
    std::vector<DifficultyDataPoint> SimulateGearChange(
        float initialGearLevel,
        float finalGearLevel,
        float duration,
        float timeStep,
        const DifficultyTransitionParams &params
    );
    
    /**
     * @brief Simulates a series of gear level changes
     * @param gearLevels Vector of gear levels to transition through
     * @param timePerTransition Time to spend on each transition
     * @param timeStep The time step for the simulation
     * @param params The transition parameters to use
     * @return A vector of data points from the simulation
     */
    std::vector<DifficultyDataPoint> SimulateGearProgression(
        const std::vector<float> &gearLevels,
        float timePerTransition,
        float timeStep,
        const DifficultyTransitionParams &params
    );
    
    /**
     * @brief Generates a text visualization of a difficulty transition
     * @param dataPoints The data points from a simulation
     * @param width The width of the visualization
     * @param height The height of the visualization
     * @return A text visualization of the transition
     */
    std::string VisualizeTransition(
        const std::vector<DifficultyDataPoint> &dataPoints,
        int width = 80,
        int height = 20
    );
    
    /**
     * @brief Generates a CSV export of a difficulty transition
     * @param dataPoints The data points from a simulation
     * @return A CSV string of the transition data
     */
    std::string ExportTransitionCSV(const std::vector<DifficultyDataPoint> &dataPoints);
    
    /**
     * @brief Compares different transition parameter sets
     * @param initialGearLevel The initial gear level
     * @param finalGearLevel The final gear level
     * @param duration The duration of the simulation
     * @param timeStep The time step for the simulation
     * @param paramSets Vector of parameter sets to compare
     * @return A text comparison of the different parameter sets
     */
    std::string CompareTransitionParams(
        float initialGearLevel,
        float finalGearLevel,
        float duration,
        float timeStep,
        const std::vector<DifficultyTransitionParams> &paramSets
    );
    
    /**
     * @brief Tests the overpower effect
     * @param gearLevel The gear level to use
     * @param overpowerFactor The overpower factor
     * @param overpowerDuration The duration of the overpower effect
     * @param simulationDuration The total duration of the simulation
     * @param timeStep The time step for the simulation
     * @return A vector of data points from the simulation
     */
    std::vector<DifficultyDataPoint> TestOverpowerEffect(
        float gearLevel,
        float overpowerFactor,
        float overpowerDuration,
        float simulationDuration,
        float timeStep
    );
    
private:
    DifficultyDebug() = default;
    ~DifficultyDebug() = default;
    
    DifficultyDebug(const DifficultyDebug&) = delete;
    DifficultyDebug& operator=(const DifficultyDebug&) = delete;
    
    /**
     * @brief Creates a mock player for simulation
     * @param gearLevel The gear level to set
     * @return A mock player
     */
    Player CreateMockPlayer(float gearLevel);
    
    // Flag to track initialization
    bool initialized = false;
};

} // namespace devilution
