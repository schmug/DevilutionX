/**
 * @file difficulty_wave.cpp
 *
 * Implementation of wave-like variations in difficulty to provide varied gameplay.
 */
#include "difficulty/difficulty_wave.h"

#include <algorithm>
#include <cmath>
#include <random>
#include <sstream>

#include "utils/log.hpp"

namespace devilution {

DifficultyWave& DifficultyWave::GetInstance()
{
    static DifficultyWave instance;
    return instance;
}

void DifficultyWave::Initialize()
{
    if (initialized) {
        return;
    }
    
    // Initialize wave parameters with default values
    waveParams = WaveParameters();
    
    // Initialize compound wave components
    compoundComponents.clear();
    
    // Add a few default components for compound waves
    CompoundWaveComponent component1;
    component1.patternType = WavePatternType::Sine;
    component1.amplitude = 0.1f;
    component1.frequency = 0.1f;
    component1.phase = 0.0f;
    component1.weight = 1.0f;
    compoundComponents.push_back(component1);
    
    CompoundWaveComponent component2;
    component2.patternType = WavePatternType::Sine;
    component2.amplitude = 0.05f;
    component2.frequency = 0.3f;
    component2.phase = 0.5f;
    component2.weight = 0.5f;
    compoundComponents.push_back(component2);
    
    // Initialize wave state
    waveState = WaveState();
    
    // Initialize random seed
    std::random_device rd;
    randomSeed = rd();
    
    initialized = true;
    LogVerbose("Difficulty Wave system initialized");
}

void DifficultyWave::Update(float gameTime)
{
    // Skip if not initialized
    if (!initialized) {
        return;
    }
    
    // Calculate the current wave value
    float waveValue = CalculateWaveValue(gameTime);
    
    // Update wave state
    waveState.currentTime = gameTime;
    waveState.currentValue = waveValue;
    waveState.lastUpdateTime = gameTime;
    
    // Add to history (keep last 100 values)
    waveState.history.push_back(waveValue);
    if (waveState.history.size() > 100) {
        waveState.history.erase(waveState.history.begin());
    }
}

float DifficultyWave::CalculateWaveValue(float gameTime)
{
    // Calculate wave value based on pattern type
    float waveValue = 0.0f;
    
    switch (waveParams.patternType) {
    case WavePatternType::Sine:
        waveValue = CalculateSineWave(
            gameTime,
            waveParams.amplitude,
            waveParams.frequency,
            waveParams.phase
        );
        break;
        
    case WavePatternType::Square:
        waveValue = CalculateSquareWave(
            gameTime,
            waveParams.amplitude,
            waveParams.frequency,
            waveParams.phase
        );
        break;
        
    case WavePatternType::Triangle:
        waveValue = CalculateTriangleWave(
            gameTime,
            waveParams.amplitude,
            waveParams.frequency,
            waveParams.phase
        );
        break;
        
    case WavePatternType::Sawtooth:
        waveValue = CalculateSawtoothWave(
            gameTime,
            waveParams.amplitude,
            waveParams.frequency,
            waveParams.phase
        );
        break;
        
    case WavePatternType::Random:
        waveValue = CalculateRandomWave(
            gameTime,
            waveParams.amplitude,
            waveParams.frequency,
            waveParams.phase
        );
        break;
        
    case WavePatternType::Compound:
        waveValue = CalculateCompoundWave(
            gameTime,
            compoundComponents
        );
        break;
    }
    
    // Add baseline
    waveValue += waveParams.baseline;
    
    return waveValue;
}

float DifficultyWave::GetCurrentWaveValue() const
{
    return waveState.currentValue;
}

WaveParameters& DifficultyWave::GetWaveParameters()
{
    return waveParams;
}

void DifficultyWave::SetWaveParameters(const WaveParameters &params)
{
    waveParams = params;
}

std::vector<CompoundWaveComponent>& DifficultyWave::GetCompoundWaveComponents()
{
    return compoundComponents;
}

void DifficultyWave::SetCompoundWaveComponents(const std::vector<CompoundWaveComponent> &components)
{
    compoundComponents = components;
}

const WaveState& DifficultyWave::GetWaveState() const
{
    return waveState;
}

float DifficultyWave::ApplyWaveEffect(float baseDifficulty, float gameTime)
{
    // Calculate the current wave value
    float waveValue = CalculateWaveValue(gameTime);
    
    // Apply the wave effect to the base difficulty
    // The wave value is centered around the baseline, so we need to adjust it
    float waveEffect = waveValue - waveParams.baseline;
    
    // Apply the effect as a percentage of the base difficulty
    float modifiedDifficulty = baseDifficulty * (1.0f + waveEffect);
    
    // Ensure difficulty doesn't go below a minimum threshold
    modifiedDifficulty = std::max(modifiedDifficulty, 0.1f);
    
    return modifiedDifficulty;
}

float DifficultyWave::CalculateSineWave(float time, float amplitude, float frequency, float phase)
{
    // Calculate sine wave: amplitude * sin(2 * pi * frequency * time + phase)
    return amplitude * std::sin(2.0f * 3.14159f * frequency * time + phase);
}

float DifficultyWave::CalculateSquareWave(float time, float amplitude, float frequency, float phase)
{
    // Calculate square wave: amplitude * sign(sin(2 * pi * frequency * time + phase))
    float sineValue = std::sin(2.0f * 3.14159f * frequency * time + phase);
    return amplitude * (sineValue >= 0.0f ? 1.0f : -1.0f);
}

float DifficultyWave::CalculateTriangleWave(float time, float amplitude, float frequency, float phase)
{
    // Calculate triangle wave: amplitude * (2 * abs(frac(frequency * time + phase / (2 * pi)) - 0.5) - 0.5)
    float t = frequency * time + phase / (2.0f * 3.14159f);
    t = t - std::floor(t); // Fractional part
    return amplitude * (2.0f * std::abs(t - 0.5f) - 0.5f);
}

float DifficultyWave::CalculateSawtoothWave(float time, float amplitude, float frequency, float phase)
{
    // Calculate sawtooth wave: amplitude * (2 * frac(frequency * time + phase / (2 * pi)) - 1)
    float t = frequency * time + phase / (2.0f * 3.14159f);
    t = t - std::floor(t); // Fractional part
    return amplitude * (2.0f * t - 1.0f);
}

float DifficultyWave::CalculateRandomWave(float time, float amplitude, float frequency, float phase)
{
    // Calculate random wave: amplitude * random value between -1 and 1
    // Use time and frequency to determine when to generate new random values
    
    // Seed the random number generator with a combination of time, frequency, and phase
    unsigned int seed = randomSeed + static_cast<unsigned int>(time * frequency * 1000.0f);
    std::mt19937 gen(seed);
    std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
    
    return amplitude * dis(gen);
}

float DifficultyWave::CalculateCompoundWave(float time, const std::vector<CompoundWaveComponent> &components)
{
    // Calculate compound wave: weighted sum of component waves
    float compoundValue = 0.0f;
    float totalWeight = 0.0f;
    
    for (const auto &component : components) {
        float componentValue = 0.0f;
        
        switch (component.patternType) {
        case WavePatternType::Sine:
            componentValue = CalculateSineWave(
                time,
                component.amplitude,
                component.frequency,
                component.phase
            );
            break;
            
        case WavePatternType::Square:
            componentValue = CalculateSquareWave(
                time,
                component.amplitude,
                component.frequency,
                component.phase
            );
            break;
            
        case WavePatternType::Triangle:
            componentValue = CalculateTriangleWave(
                time,
                component.amplitude,
                component.frequency,
                component.phase
            );
            break;
            
        case WavePatternType::Sawtooth:
            componentValue = CalculateSawtoothWave(
                time,
                component.amplitude,
                component.frequency,
                component.phase
            );
            break;
            
        case WavePatternType::Random:
            componentValue = CalculateRandomWave(
                time,
                component.amplitude,
                component.frequency,
                component.phase
            );
            break;
            
        case WavePatternType::Compound:
            // Avoid recursive compound waves
            componentValue = 0.0f;
            break;
        }
        
        compoundValue += componentValue * component.weight;
        totalWeight += component.weight;
    }
    
    // Normalize by total weight
    if (totalWeight > 0.0f) {
        compoundValue /= totalWeight;
    }
    
    return compoundValue;
}

std::string DifficultyWave::GetWavePatternName(WavePatternType patternType)
{
    switch (patternType) {
    case WavePatternType::Sine:
        return "sine";
    case WavePatternType::Square:
        return "square";
    case WavePatternType::Triangle:
        return "triangle";
    case WavePatternType::Sawtooth:
        return "sawtooth";
    case WavePatternType::Random:
        return "random";
    case WavePatternType::Compound:
        return "compound";
    default:
        return "unknown";
    }
}

WavePatternType DifficultyWave::GetWavePatternType(const std::string &patternName)
{
    if (patternName == "sine") {
        return WavePatternType::Sine;
    } else if (patternName == "square") {
        return WavePatternType::Square;
    } else if (patternName == "triangle") {
        return WavePatternType::Triangle;
    } else if (patternName == "sawtooth") {
        return WavePatternType::Sawtooth;
    } else if (patternName == "random") {
        return WavePatternType::Random;
    } else if (patternName == "compound") {
        return WavePatternType::Compound;
    } else {
        return WavePatternType::Sine; // Default to sine
    }
}

} // namespace devilution
