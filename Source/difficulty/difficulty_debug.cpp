/**
 * @file difficulty_debug.cpp
 *
 * Implementation of debug utilities for the difficulty transition system.
 */
#include "difficulty/difficulty_debug.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

#include "utils/log.hpp"

namespace devilution {

DifficultyDebug& DifficultyDebug::GetInstance()
{
    static DifficultyDebug instance;
    return instance;
}

void DifficultyDebug::Initialize()
{
    if (initialized) {
        return;
    }
    
    // Initialize the difficulty manager
    DifficultyManager::GetInstance().Initialize();
    
    initialized = true;
    LogVerbose("Difficulty Debug utilities initialized");
}

std::vector<DifficultyDataPoint> DifficultyDebug::SimulateGearChange(
    float initialGearLevel,
    float finalGearLevel,
    float duration,
    float timeStep,
    const DifficultyTransitionParams &params)
{
    std::vector<DifficultyDataPoint> dataPoints;
    
    // Create a mock player
    Player player = CreateMockPlayer(initialGearLevel);
    
    // Get the difficulty manager
    DifficultyManager &manager = DifficultyManager::GetInstance();
    
    // Set the transition parameters
    manager.SetTransitionParams(params);
    
    // Initialize the difficulty
    manager.UpdateDifficulty(player, 0.0f);
    
    // Record the initial state
    DifficultyDataPoint initialPoint;
    initialPoint.time = 0.0f;
    initialPoint.rawGearLevel = initialGearLevel;
    initialPoint.targetDifficulty = manager.GetTargetDifficulty(player);
    initialPoint.currentDifficulty = manager.GetCurrentDifficulty(player);
    initialPoint.overpowerFactor = manager.GetOverpowerFactor(player);
    dataPoints.push_back(initialPoint);
    
    // Simulate the gear change
    for (float time = timeStep; time <= duration; time += timeStep) {
        // Calculate the interpolation factor (0 to 1)
        float t = time / duration;
        
        // Interpolate the gear level
        float currentGearLevel = initialGearLevel + (finalGearLevel - initialGearLevel) * t;
        
        // Update the mock player
        player = CreateMockPlayer(currentGearLevel);
        
        // Update the difficulty
        manager.UpdateDifficulty(player, time);
        
        // Record the state
        DifficultyDataPoint point;
        point.time = time;
        point.rawGearLevel = currentGearLevel;
        point.targetDifficulty = manager.GetTargetDifficulty(player);
        point.currentDifficulty = manager.GetCurrentDifficulty(player);
        point.overpowerFactor = manager.GetOverpowerFactor(player);
        dataPoints.push_back(point);
    }
    
    return dataPoints;
}

std::vector<DifficultyDataPoint> DifficultyDebug::SimulateGearProgression(
    const std::vector<float> &gearLevels,
    float timePerTransition,
    float timeStep,
    const DifficultyTransitionParams &params)
{
    std::vector<DifficultyDataPoint> dataPoints;
    
    // Check if there are at least two gear levels
    if (gearLevels.size() < 2) {
        LogError("SimulateGearProgression requires at least two gear levels");
        return dataPoints;
    }
    
    // Get the difficulty manager
    DifficultyManager &manager = DifficultyManager::GetInstance();
    
    // Set the transition parameters
    manager.SetTransitionParams(params);
    
    // Create a mock player with the initial gear level
    Player player = CreateMockPlayer(gearLevels[0]);
    
    // Initialize the difficulty
    manager.UpdateDifficulty(player, 0.0f);
    
    // Record the initial state
    DifficultyDataPoint initialPoint;
    initialPoint.time = 0.0f;
    initialPoint.rawGearLevel = gearLevels[0];
    initialPoint.targetDifficulty = manager.GetTargetDifficulty(player);
    initialPoint.currentDifficulty = manager.GetCurrentDifficulty(player);
    initialPoint.overpowerFactor = manager.GetOverpowerFactor(player);
    dataPoints.push_back(initialPoint);
    
    // Simulate each transition
    float currentTime = 0.0f;
    
    for (size_t i = 0; i < gearLevels.size() - 1; i++) {
        float startGearLevel = gearLevels[i];
        float endGearLevel = gearLevels[i + 1];
        
        // Simulate this transition
        for (float time = timeStep; time <= timePerTransition; time += timeStep) {
            // Calculate the interpolation factor (0 to 1)
            float t = time / timePerTransition;
            
            // Interpolate the gear level
            float currentGearLevel = startGearLevel + (endGearLevel - startGearLevel) * t;
            
            // Update the mock player
            player = CreateMockPlayer(currentGearLevel);
            
            // Update the difficulty
            currentTime += timeStep;
            manager.UpdateDifficulty(player, currentTime);
            
            // Record the state
            DifficultyDataPoint point;
            point.time = currentTime;
            point.rawGearLevel = currentGearLevel;
            point.targetDifficulty = manager.GetTargetDifficulty(player);
            point.currentDifficulty = manager.GetCurrentDifficulty(player);
            point.overpowerFactor = manager.GetOverpowerFactor(player);
            dataPoints.push_back(point);
        }
        
        // Trigger overpower effect if this is a significant upgrade
        if (endGearLevel > startGearLevel * 1.5f) {
            manager.TriggerOverpower(player, currentTime);
        }
    }
    
    return dataPoints;
}

std::string DifficultyDebug::VisualizeTransition(
    const std::vector<DifficultyDataPoint> &dataPoints,
    int width,
    int height)
{
    if (dataPoints.empty()) {
        return "No data points to visualize";
    }
    
    std::stringstream visualization;
    visualization << "Difficulty Transition Visualization\n";
    visualization << "==================================\n\n";
    
    // Find the min and max values for scaling
    float minTime = dataPoints.front().time;
    float maxTime = dataPoints.back().time;
    
    float minValue = std::numeric_limits<float>::max();
    float maxValue = std::numeric_limits<float>::lowest();
    
    for (const auto &point : dataPoints) {
        minValue = std::min(minValue, std::min(point.rawGearLevel, point.currentDifficulty));
        maxValue = std::max(maxValue, std::max(point.rawGearLevel, point.currentDifficulty));
    }
    
    // Add some padding to the value range
    float valuePadding = (maxValue - minValue) * 0.1f;
    minValue -= valuePadding;
    maxValue += valuePadding;
    
    // Create a 2D grid for the visualization
    std::vector<std::vector<char>> grid(height, std::vector<char>(width, ' '));
    
    // Plot the raw gear level
    for (size_t i = 0; i < dataPoints.size() - 1; i++) {
        const auto &point1 = dataPoints[i];
        const auto &point2 = dataPoints[i + 1];
        
        // Map time to x coordinate
        int x1 = static_cast<int>((point1.time - minTime) / (maxTime - minTime) * (width - 1));
        int x2 = static_cast<int>((point2.time - minTime) / (maxTime - minTime) * (width - 1));
        
        // Map gear level to y coordinate
        int y1 = static_cast<int>((maxValue - point1.rawGearLevel) / (maxValue - minValue) * (height - 1));
        int y2 = static_cast<int>((maxValue - point2.rawGearLevel) / (maxValue - minValue) * (height - 1));
        
        // Draw a line between the points
        DrawLine(grid, x1, y1, x2, y2, 'G');
    }
    
    // Plot the current difficulty
    for (size_t i = 0; i < dataPoints.size() - 1; i++) {
        const auto &point1 = dataPoints[i];
        const auto &point2 = dataPoints[i + 1];
        
        // Map time to x coordinate
        int x1 = static_cast<int>((point1.time - minTime) / (maxTime - minTime) * (width - 1));
        int x2 = static_cast<int>((point2.time - minTime) / (maxTime - minTime) * (width - 1));
        
        // Map difficulty to y coordinate
        int y1 = static_cast<int>((maxValue - point1.currentDifficulty) / (maxValue - minValue) * (height - 1));
        int y2 = static_cast<int>((maxValue - point2.currentDifficulty) / (maxValue - minValue) * (height - 1));
        
        // Draw a line between the points
        DrawLine(grid, x1, y1, x2, y2, 'D');
    }
    
    // Plot the target difficulty
    for (size_t i = 0; i < dataPoints.size() - 1; i++) {
        const auto &point1 = dataPoints[i];
        const auto &point2 = dataPoints[i + 1];
        
        // Map time to x coordinate
        int x1 = static_cast<int>((point1.time - minTime) / (maxTime - minTime) * (width - 1));
        int x2 = static_cast<int>((point2.time - minTime) / (maxTime - minTime) * (width - 1));
        
        // Map target difficulty to y coordinate
        int y1 = static_cast<int>((maxValue - point1.targetDifficulty) / (maxValue - minValue) * (height - 1));
        int y2 = static_cast<int>((maxValue - point2.targetDifficulty) / (maxValue - minValue) * (height - 1));
        
        // Draw a line between the points
        DrawLine(grid, x1, y1, x2, y2, 'T');
    }
    
    // Convert the grid to a string
    for (const auto &row : grid) {
        for (char c : row) {
            visualization << c;
        }
        visualization << "\n";
    }
    
    // Add a legend
    visualization << "\nLegend:\n";
    visualization << "G - Raw Gear Level\n";
    visualization << "T - Target Difficulty\n";
    visualization << "D - Current Difficulty\n";
    
    // Add axis labels
    visualization << "\nX-Axis: Time (" << minTime << " to " << maxTime << ")\n";
    visualization << "Y-Axis: Value (" << minValue << " to " << maxValue << ")\n";
    
    // Add summary statistics
    visualization << "\nSummary:\n";
    visualization << "Initial Gear Level: " << dataPoints.front().rawGearLevel << "\n";
    visualization << "Final Gear Level: " << dataPoints.back().rawGearLevel << "\n";
    visualization << "Initial Difficulty: " << dataPoints.front().currentDifficulty << "\n";
    visualization << "Final Difficulty: " << dataPoints.back().currentDifficulty << "\n";
    visualization << "Transition Duration: " << (maxTime - minTime) << " time units\n";
    
    return visualization.str();
}

std::string DifficultyDebug::ExportTransitionCSV(const std::vector<DifficultyDataPoint> &dataPoints)
{
    std::stringstream csv;
    
    // Add header
    csv << "Time,RawGearLevel,TargetDifficulty,CurrentDifficulty,OverpowerFactor\n";
    
    // Add data points
    for (const auto &point : dataPoints) {
        csv << point.time << ","
            << point.rawGearLevel << ","
            << point.targetDifficulty << ","
            << point.currentDifficulty << ","
            << point.overpowerFactor << "\n";
    }
    
    return csv.str();
}

std::string DifficultyDebug::CompareTransitionParams(
    float initialGearLevel,
    float finalGearLevel,
    float duration,
    float timeStep,
    const std::vector<DifficultyTransitionParams> &paramSets)
{
    std::stringstream comparison;
    comparison << "Transition Parameters Comparison\n";
    comparison << "===============================\n\n";
    
    // Run simulations for each parameter set
    std::vector<std::vector<DifficultyDataPoint>> allDataPoints;
    
    for (const auto &params : paramSets) {
        allDataPoints.push_back(SimulateGearChange(initialGearLevel, finalGearLevel, duration, timeStep, params));
    }
    
    // Compare the results
    comparison << "Comparison Summary:\n";
    comparison << std::setw(20) << "Parameter Set" << " | "
               << std::setw(15) << "Final Difficulty" << " | "
               << std::setw(15) << "Transition Time" << " | "
               << std::setw(15) << "Smoothness" << "\n";
    comparison << std::string(20, '-') << "-+-"
               << std::string(15, '-') << "-+-"
               << std::string(15, '-') << "-+-"
               << std::string(15, '-') << "\n";
    
    for (size_t i = 0; i < paramSets.size(); i++) {
        const auto &params = paramSets[i];
        const auto &dataPoints = allDataPoints[i];
        
        // Calculate the transition time (time to reach 90% of the final difficulty)
        float targetDifficulty = dataPoints.back().targetDifficulty;
        float transitionThreshold = initialGearLevel + 0.9f * (targetDifficulty - initialGearLevel);
        
        float transitionTime = duration; // Default to full duration
        for (const auto &point : dataPoints) {
            if (point.currentDifficulty >= transitionThreshold) {
                transitionTime = point.time;
                break;
            }
        }
        
        // Calculate smoothness (lower is smoother)
        float smoothness = 0.0f;
        for (size_t j = 1; j < dataPoints.size(); j++) {
            float diffChange = std::abs(dataPoints[j].currentDifficulty - dataPoints[j-1].currentDifficulty);
            smoothness += diffChange;
        }
        smoothness /= dataPoints.size() - 1;
        
        // Add to the comparison table
        comparison << std::setw(20) << ("Set " + std::to_string(i+1)) << " | "
                   << std::setw(15) << std::fixed << std::setprecision(2) << dataPoints.back().currentDifficulty << " | "
                   << std::setw(15) << std::fixed << std::setprecision(2) << transitionTime << " | "
                   << std::setw(15) << std::fixed << std::setprecision(4) << smoothness << "\n";
    }
    
    // Add parameter details
    comparison << "\nParameter Details:\n";
    
    for (size_t i = 0; i < paramSets.size(); i++) {
        const auto &params = paramSets[i];
        
        comparison << "Set " << (i+1) << ":\n";
        comparison << "  Increase Rate: " << params.increaseRate << "\n";
        comparison << "  Decrease Rate: " << params.decreaseRate << "\n";
        comparison << "  Overpower Factor: " << params.overpowerFactor << "\n";
        comparison << "  Overpower Duration: " << params.overpowerDuration << "\n";
        comparison << "  History Size: " << params.historySize << "\n";
        comparison << "  Interpolation Type: ";
        
        switch (params.interpolationType) {
        case InterpolationType::Linear:
            comparison << "Linear\n";
            break;
        case InterpolationType::Sigmoid:
            comparison << "Sigmoid\n";
            break;
        case InterpolationType::Exponential:
            comparison << "Exponential\n";
            break;
        case InterpolationType::Logarithmic:
            comparison << "Logarithmic\n";
            break;
        }
        
        comparison << "\n";
    }
    
    return comparison.str();
}

std::vector<DifficultyDataPoint> DifficultyDebug::TestOverpowerEffect(
    float gearLevel,
    float overpowerFactor,
    float overpowerDuration,
    float simulationDuration,
    float timeStep)
{
    std::vector<DifficultyDataPoint> dataPoints;
    
    // Create a mock player
    Player player = CreateMockPlayer(gearLevel);
    
    // Get the difficulty manager
    DifficultyManager &manager = DifficultyManager::GetInstance();
    
    // Set up the transition parameters
    DifficultyTransitionParams params;
    params.overpowerFactor = overpowerFactor;
    params.overpowerDuration = overpowerDuration;
    manager.SetTransitionParams(params);
    
    // Initialize the difficulty
    manager.UpdateDifficulty(player, 0.0f);
    
    // Record the initial state
    DifficultyDataPoint initialPoint;
    initialPoint.time = 0.0f;
    initialPoint.rawGearLevel = gearLevel;
    initialPoint.targetDifficulty = manager.GetTargetDifficulty(player);
    initialPoint.currentDifficulty = manager.GetCurrentDifficulty(player);
    initialPoint.overpowerFactor = manager.GetOverpowerFactor(player);
    dataPoints.push_back(initialPoint);
    
    // Trigger the overpower effect
    manager.TriggerOverpower(player, 0.0f);
    
    // Simulate the overpower effect
    for (float time = timeStep; time <= simulationDuration; time += timeStep) {
        // Update the difficulty
        manager.UpdateDifficulty(player, time);
        
        // Record the state
        DifficultyDataPoint point;
        point.time = time;
        point.rawGearLevel = gearLevel;
        point.targetDifficulty = manager.GetTargetDifficulty(player);
        point.currentDifficulty = manager.GetCurrentDifficulty(player);
        point.overpowerFactor = manager.GetOverpowerFactor(player);
        dataPoints.push_back(point);
    }
    
    return dataPoints;
}

Player DifficultyDebug::CreateMockPlayer(float gearLevel)
{
    // Create a basic player
    Player player;
    
    // Set a unique ID
    player._pName = "MockPlayer";
    
    // Set the gear level
    // Note: This is a hack for testing - in a real implementation,
    // the gear level would be calculated from the player's equipment
    
    // We'll use a custom hook in the GearLevelManager to return our desired gear level
    // This is just for the mock player used in simulations
    
    return player;
}

// Helper function to draw a line between two points
void DrawLine(std::vector<std::vector<char>> &grid, int x1, int y1, int x2, int y2, char c)
{
    // Bresenham's line algorithm
    int dx = std::abs(x2 - x1);
    int dy = std::abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    
    while (true) {
        // Check if the point is within the grid
        if (x1 >= 0 && x1 < static_cast<int>(grid[0].size()) &&
            y1 >= 0 && y1 < static_cast<int>(grid.size())) {
            grid[y1][x1] = c;
        }
        
        if (x1 == x2 && y1 == y2) {
            break;
        }
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

} // namespace devilution
