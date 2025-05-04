/**
 * @file difficulty_wave.h
 *
 * Interface for generating wave-like variations in difficulty to provide varied gameplay.
 */
#pragma once

#include <array>
#include <string>
#include <vector>
#include <unordered_map>

#include "player.h"
#include "difficulty/difficulty_manager.h"

namespace devilution {

/**
 * @brief Enum for difficulty wave pattern type
 */
enum class WavePatternType {
    Sine,       // Smooth sine wave pattern
    Square,     // Abrupt square wave pattern
    Triangle,   // Linear triangle wave pattern
    Sawtooth,   // Gradual increase, sudden drop
    Random,     // Random fluctuations
    Compound    // Combination of multiple patterns
};

/**
 * @brief Structure for difficulty wave parameters
 */
struct WaveParameters {
    WavePatternType patternType;  // Type of wave pattern
    float amplitude;              // Wave amplitude (how much difficulty varies)
    float frequency;              // Wave frequency (how often difficulty changes)
    float phase;                  // Wave phase (offset in the cycle)
    float baseline;               // Baseline difficulty (center of the wave)
    
    // Default constructor
    WaveParameters()
        : patternType(WavePatternType::Sine), amplitude(0.2f), frequency(0.1f),
          phase(0.0f), baseline(1.0f)
    {
    }
};

/**
 * @brief Structure for compound wave component
 */
struct CompoundWaveComponent {
    WavePatternType patternType;  // Type of wave pattern
    float amplitude;              // Wave amplitude
    float frequency;              // Wave frequency
    float phase;                  // Wave phase
    float weight;                 // Component weight in the compound wave
    
    // Default constructor
    CompoundWaveComponent()
        : patternType(WavePatternType::Sine), amplitude(0.1f), frequency(0.1f),
          phase(0.0f), weight(1.0f)
    {
    }
};

/**
 * @brief Structure for difficulty wave state
 */
struct WaveState {
    float currentTime;            // Current time in the wave cycle
    float currentValue;           // Current wave value
    float lastUpdateTime;         // Last time the wave was updated
    std::vector<float> history;   // History of wave values
    
    // Default constructor
    WaveState()
        : currentTime(0.0f), currentValue(0.0f), lastUpdateTime(0.0f)
    {
    }
};

/**
 * @brief Class for managing difficulty wave effects
 */
class DifficultyWave {
public:
    /**
     * @brief Gets the singleton instance
     * @return Reference to the singleton instance
     */
    static DifficultyWave& GetInstance();
    
    /**
     * @brief Initializes the difficulty wave system
     */
    void Initialize();
    
    /**
     * @brief Updates the difficulty wave
     * @param gameTime Current game time
     */
    void Update(float gameTime);
    
    /**
     * @brief Calculates the current wave value
     * @param gameTime Current game time
     * @return Current wave value
     */
    float CalculateWaveValue(float gameTime);
    
    /**
     * @brief Gets the current wave value
     * @return Current wave value
     */
    float GetCurrentWaveValue() const;
    
    /**
     * @brief Gets the wave parameters
     * @return Reference to the wave parameters
     */
    WaveParameters& GetWaveParameters();
    
    /**
     * @brief Sets the wave parameters
     * @param params The new wave parameters
     */
    void SetWaveParameters(const WaveParameters &params);
    
    /**
     * @brief Gets the compound wave components
     * @return Reference to the compound wave components
     */
    std::vector<CompoundWaveComponent>& GetCompoundWaveComponents();
    
    /**
     * @brief Sets the compound wave components
     * @param components The new compound wave components
     */
    void SetCompoundWaveComponents(const std::vector<CompoundWaveComponent> &components);
    
    /**
     * @brief Gets the wave state
     * @return Reference to the wave state
     */
    const WaveState& GetWaveState() const;
    
    /**
     * @brief Applies the wave effect to a difficulty value
     * @param baseDifficulty The base difficulty value
     * @param gameTime Current game time
     * @return The modified difficulty value
     */
    float ApplyWaveEffect(float baseDifficulty, float gameTime);
    
    /**
     * @brief Calculates a sine wave value
     * @param time Current time
     * @param amplitude Wave amplitude
     * @param frequency Wave frequency
     * @param phase Wave phase
     * @return Sine wave value
     */
    float CalculateSineWave(float time, float amplitude, float frequency, float phase);
    
    /**
     * @brief Calculates a square wave value
     * @param time Current time
     * @param amplitude Wave amplitude
     * @param frequency Wave frequency
     * @param phase Wave phase
     * @return Square wave value
     */
    float CalculateSquareWave(float time, float amplitude, float frequency, float phase);
    
    /**
     * @brief Calculates a triangle wave value
     * @param time Current time
     * @param amplitude Wave amplitude
     * @param frequency Wave frequency
     * @param phase Wave phase
     * @return Triangle wave value
     */
    float CalculateTriangleWave(float time, float amplitude, float frequency, float phase);
    
    /**
     * @brief Calculates a sawtooth wave value
     * @param time Current time
     * @param amplitude Wave amplitude
     * @param frequency Wave frequency
     * @param phase Wave phase
     * @return Sawtooth wave value
     */
    float CalculateSawtoothWave(float time, float amplitude, float frequency, float phase);
    
    /**
     * @brief Calculates a random wave value
     * @param time Current time
     * @param amplitude Wave amplitude
     * @param frequency Wave frequency
     * @param phase Wave phase
     * @return Random wave value
     */
    float CalculateRandomWave(float time, float amplitude, float frequency, float phase);
    
    /**
     * @brief Calculates a compound wave value
     * @param time Current time
     * @param components Compound wave components
     * @return Compound wave value
     */
    float CalculateCompoundWave(float time, const std::vector<CompoundWaveComponent> &components);
    
    /**
     * @brief Gets a wave pattern name
     * @param patternType Wave pattern type
     * @return Wave pattern name
     */
    std::string GetWavePatternName(WavePatternType patternType);
    
    /**
     * @brief Gets a wave pattern type from name
     * @param patternName Wave pattern name
     * @return Wave pattern type
     */
    WavePatternType GetWavePatternType(const std::string &patternName);
    
private:
    DifficultyWave() = default;
    ~DifficultyWave() = default;
    
    DifficultyWave(const DifficultyWave&) = delete;
    DifficultyWave& operator=(const DifficultyWave&) = delete;
    
    // Wave parameters
    WaveParameters waveParams;
    
    // Compound wave components
    std::vector<CompoundWaveComponent> compoundComponents;
    
    // Wave state
    WaveState waveState;
    
    // Flag to track initialization
    bool initialized = false;
    
    // Random number generator seed
    unsigned int randomSeed = 0;
};

} // namespace devilution
