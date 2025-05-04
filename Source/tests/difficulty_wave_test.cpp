#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <cmath>

#include "difficulty/difficulty_wave.h"
#include "utils/log.hpp"

namespace devilution {
namespace {

class DifficultyWaveTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize logging for tests
        LogInit();
        
        // Initialize the difficulty wave system
        difficultyWave = &DifficultyWave::GetInstance();
        difficultyWave->Initialize();
    }

    void TearDown() override {
        // Clean up after tests
        LogClose();
    }
    
    // Helper method to set wave parameters
    void SetWaveParameters(WavePatternType patternType, float amplitude, float frequency, float phase, float baseline) {
        WaveParameters params;
        params.patternType = patternType;
        params.amplitude = amplitude;
        params.frequency = frequency;
        params.phase = phase;
        params.baseline = baseline;
        difficultyWave->SetWaveParameters(params);
    }

    DifficultyWave* difficultyWave;
};

TEST_F(DifficultyWaveTest, SineWaveGeneratesCorrectValues) {
    // Test that sine wave generates correct values
    
    // Set up sine wave parameters
    SetWaveParameters(WavePatternType::Sine, 1.0f, 1.0f, 0.0f, 0.0f);
    
    // Calculate values at specific points in the wave cycle
    float value0 = difficultyWave->CalculateSineWave(0.0f, 1.0f, 1.0f, 0.0f);
    float value25 = difficultyWave->CalculateSineWave(0.25f, 1.0f, 1.0f, 0.0f);
    float value50 = difficultyWave->CalculateSineWave(0.5f, 1.0f, 1.0f, 0.0f);
    float value75 = difficultyWave->CalculateSineWave(0.75f, 1.0f, 1.0f, 0.0f);
    float value100 = difficultyWave->CalculateSineWave(1.0f, 1.0f, 1.0f, 0.0f);
    
    // Verify values at key points in the sine wave
    EXPECT_NEAR(value0, 0.0f, 0.01f) << "Sine wave should be 0 at t=0";
    EXPECT_NEAR(value25, 1.0f, 0.01f) << "Sine wave should be 1 at t=0.25";
    EXPECT_NEAR(value50, 0.0f, 0.01f) << "Sine wave should be 0 at t=0.5";
    EXPECT_NEAR(value75, -1.0f, 0.01f) << "Sine wave should be -1 at t=0.75";
    EXPECT_NEAR(value100, 0.0f, 0.01f) << "Sine wave should be 0 at t=1.0";
}

TEST_F(DifficultyWaveTest, SquareWaveGeneratesCorrectValues) {
    // Test that square wave generates correct values
    
    // Set up square wave parameters
    SetWaveParameters(WavePatternType::Square, 1.0f, 1.0f, 0.0f, 0.0f);
    
    // Calculate values at specific points in the wave cycle
    float value0 = difficultyWave->CalculateSquareWave(0.0f, 1.0f, 1.0f, 0.0f);
    float value24 = difficultyWave->CalculateSquareWave(0.24f, 1.0f, 1.0f, 0.0f);
    float value26 = difficultyWave->CalculateSquareWave(0.26f, 1.0f, 1.0f, 0.0f);
    float value49 = difficultyWave->CalculateSquareWave(0.49f, 1.0f, 1.0f, 0.0f);
    float value51 = difficultyWave->CalculateSquareWave(0.51f, 1.0f, 1.0f, 0.0f);
    float value74 = difficultyWave->CalculateSquareWave(0.74f, 1.0f, 1.0f, 0.0f);
    float value76 = difficultyWave->CalculateSquareWave(0.76f, 1.0f, 1.0f, 0.0f);
    
    // Verify values at key points in the square wave
    EXPECT_EQ(value0, 1.0f) << "Square wave should be 1 at t=0";
    EXPECT_EQ(value24, 1.0f) << "Square wave should be 1 at t=0.24";
    EXPECT_EQ(value26, 1.0f) << "Square wave should be 1 at t=0.26";
    EXPECT_EQ(value49, 1.0f) << "Square wave should be 1 at t=0.49";
    EXPECT_EQ(value51, -1.0f) << "Square wave should be -1 at t=0.51";
    EXPECT_EQ(value74, -1.0f) << "Square wave should be -1 at t=0.74";
    EXPECT_EQ(value76, -1.0f) << "Square wave should be -1 at t=0.76";
}

TEST_F(DifficultyWaveTest, TriangleWaveGeneratesCorrectValues) {
    // Test that triangle wave generates correct values
    
    // Set up triangle wave parameters
    SetWaveParameters(WavePatternType::Triangle, 1.0f, 1.0f, 0.0f, 0.0f);
    
    // Calculate values at specific points in the wave cycle
    float value0 = difficultyWave->CalculateTriangleWave(0.0f, 1.0f, 1.0f, 0.0f);
    float value25 = difficultyWave->CalculateTriangleWave(0.25f, 1.0f, 1.0f, 0.0f);
    float value50 = difficultyWave->CalculateTriangleWave(0.5f, 1.0f, 1.0f, 0.0f);
    float value75 = difficultyWave->CalculateTriangleWave(0.75f, 1.0f, 1.0f, 0.0f);
    float value100 = difficultyWave->CalculateTriangleWave(1.0f, 1.0f, 1.0f, 0.0f);
    
    // Verify values at key points in the triangle wave
    EXPECT_NEAR(value0, -0.5f, 0.01f) << "Triangle wave should be -0.5 at t=0";
    EXPECT_NEAR(value25, 0.0f, 0.01f) << "Triangle wave should be 0 at t=0.25";
    EXPECT_NEAR(value50, 0.5f, 0.01f) << "Triangle wave should be 0.5 at t=0.5";
    EXPECT_NEAR(value75, 0.0f, 0.01f) << "Triangle wave should be 0 at t=0.75";
    EXPECT_NEAR(value100, -0.5f, 0.01f) << "Triangle wave should be -0.5 at t=1.0";
}

TEST_F(DifficultyWaveTest, SawtoothWaveGeneratesCorrectValues) {
    // Test that sawtooth wave generates correct values
    
    // Set up sawtooth wave parameters
    SetWaveParameters(WavePatternType::Sawtooth, 1.0f, 1.0f, 0.0f, 0.0f);
    
    // Calculate values at specific points in the wave cycle
    float value0 = difficultyWave->CalculateSawtoothWave(0.0f, 1.0f, 1.0f, 0.0f);
    float value25 = difficultyWave->CalculateSawtoothWave(0.25f, 1.0f, 1.0f, 0.0f);
    float value50 = difficultyWave->CalculateSawtoothWave(0.5f, 1.0f, 1.0f, 0.0f);
    float value75 = difficultyWave->CalculateSawtoothWave(0.75f, 1.0f, 1.0f, 0.0f);
    float value99 = difficultyWave->CalculateSawtoothWave(0.99f, 1.0f, 1.0f, 0.0f);
    float value100 = difficultyWave->CalculateSawtoothWave(1.0f, 1.0f, 1.0f, 0.0f);
    
    // Verify values at key points in the sawtooth wave
    EXPECT_NEAR(value0, -1.0f, 0.01f) << "Sawtooth wave should be -1 at t=0";
    EXPECT_NEAR(value25, -0.5f, 0.01f) << "Sawtooth wave should be -0.5 at t=0.25";
    EXPECT_NEAR(value50, 0.0f, 0.01f) << "Sawtooth wave should be 0 at t=0.5";
    EXPECT_NEAR(value75, 0.5f, 0.01f) << "Sawtooth wave should be 0.5 at t=0.75";
    EXPECT_NEAR(value99, 0.98f, 0.01f) << "Sawtooth wave should be 0.98 at t=0.99";
    EXPECT_NEAR(value100, -1.0f, 0.01f) << "Sawtooth wave should be -1 at t=1.0";
}

TEST_F(DifficultyWaveTest, RandomWaveGeneratesVariableValues) {
    // Test that random wave generates variable values
    
    // Set up random wave parameters
    SetWaveParameters(WavePatternType::Random, 1.0f, 1.0f, 0.0f, 0.0f);
    
    // Calculate multiple values
    std::vector<float> values;
    for (int i = 0; i < 10; i++) {
        float value = difficultyWave->CalculateRandomWave(static_cast<float>(i) * 0.1f, 1.0f, 1.0f, 0.0f);
        values.push_back(value);
    }
    
    // Verify that values vary and are within the expected range
    bool valuesVary = false;
    for (size_t i = 1; i < values.size(); i++) {
        if (std::abs(values[i] - values[i-1]) > 0.01f) {
            valuesVary = true;
            break;
        }
    }
    
    EXPECT_TRUE(valuesVary) << "Random wave should generate varying values";
    
    // Check that values are within the expected range (-amplitude to +amplitude)
    for (float value : values) {
        EXPECT_GE(value, -1.0f) << "Random wave value should be >= -amplitude";
        EXPECT_LE(value, 1.0f) << "Random wave value should be <= +amplitude";
    }
}

TEST_F(DifficultyWaveTest, CompoundWaveGeneratesCorrectValues) {
    // Test that compound wave generates correct values
    
    // Set up compound wave parameters
    SetWaveParameters(WavePatternType::Compound, 1.0f, 1.0f, 0.0f, 0.0f);
    
    // Create compound wave components
    std::vector<CompoundWaveComponent> components;
    
    CompoundWaveComponent component1;
    component1.patternType = WavePatternType::Sine;
    component1.amplitude = 1.0f;
    component1.frequency = 1.0f;
    component1.phase = 0.0f;
    component1.weight = 1.0f;
    components.push_back(component1);
    
    CompoundWaveComponent component2;
    component2.patternType = WavePatternType::Sine;
    component2.amplitude = 0.5f;
    component2.frequency = 2.0f;
    component2.phase = 0.0f;
    component2.weight = 0.5f;
    components.push_back(component2);
    
    difficultyWave->SetCompoundWaveComponents(components);
    
    // Calculate compound wave value
    float value = difficultyWave->CalculateCompoundWave(0.25f, components);
    
    // Calculate expected value
    // At t=0.25:
    // Component 1: sine(2*pi*1*0.25) = sine(pi/2) = 1.0
    // Component 2: sine(2*pi*2*0.25) = sine(pi) = 0.0
    // Weighted sum: (1.0 * 1.0 + 0.0 * 0.5) / (1.0 + 0.5) = 1.0 / 1.5 = 0.667
    float expectedValue = 0.667f;
    
    EXPECT_NEAR(value, expectedValue, 0.01f) << "Compound wave should generate the correct weighted sum of component waves";
}

TEST_F(DifficultyWaveTest, WaveEffectModifiesDifficulty) {
    // Test that wave effect modifies difficulty correctly
    
    // Set up sine wave parameters
    SetWaveParameters(WavePatternType::Sine, 0.2f, 1.0f, 0.0f, 1.0f);
    
    // Base difficulty to test with
    float baseDifficulty = 50.0f;
    
    // Apply wave effect at different time points
    float gameTime1 = 0.0f;
    float modifiedDifficulty1 = difficultyWave->ApplyWaveEffect(baseDifficulty, gameTime1);
    
    float gameTime2 = 0.25f; // Quarter of a sine wave cycle
    float modifiedDifficulty2 = difficultyWave->ApplyWaveEffect(baseDifficulty, gameTime2);
    
    float gameTime3 = 0.5f; // Half of a sine wave cycle
    float modifiedDifficulty3 = difficultyWave->ApplyWaveEffect(baseDifficulty, gameTime3);
    
    // Expected values:
    // At t=0.0: baseline + sin(0) * amplitude = 1.0 + 0.0 * 0.2 = 1.0
    // Modified difficulty = 50.0 * 1.0 = 50.0
    float expectedDifficulty1 = 50.0f;
    
    // At t=0.25: baseline + sin(pi/2) * amplitude = 1.0 + 1.0 * 0.2 = 1.2
    // Modified difficulty = 50.0 * 1.2 = 60.0
    float expectedDifficulty2 = 60.0f;
    
    // At t=0.5: baseline + sin(pi) * amplitude = 1.0 + 0.0 * 0.2 = 1.0
    // Modified difficulty = 50.0 * 1.0 = 50.0
    float expectedDifficulty3 = 50.0f;
    
    // Verify that difficulty is modified correctly
    EXPECT_NEAR(modifiedDifficulty1, expectedDifficulty1, 0.1f) << "Wave effect should modify difficulty correctly at t=0.0";
    EXPECT_NEAR(modifiedDifficulty2, expectedDifficulty2, 0.1f) << "Wave effect should modify difficulty correctly at t=0.25";
    EXPECT_NEAR(modifiedDifficulty3, expectedDifficulty3, 0.1f) << "Wave effect should modify difficulty correctly at t=0.5";
}

TEST_F(DifficultyWaveTest, WaveParametersAffectOutput) {
    // Test that changing wave parameters affects the output
    
    // Base difficulty to test with
    float baseDifficulty = 50.0f;
    float gameTime = 0.25f;
    
    // Test different amplitudes
    SetWaveParameters(WavePatternType::Sine, 0.1f, 1.0f, 0.0f, 1.0f);
    float lowAmplitudeDifficulty = difficultyWave->ApplyWaveEffect(baseDifficulty, gameTime);
    
    SetWaveParameters(WavePatternType::Sine, 0.3f, 1.0f, 0.0f, 1.0f);
    float highAmplitudeDifficulty = difficultyWave->ApplyWaveEffect(baseDifficulty, gameTime);
    
    // Test different frequencies
    SetWaveParameters(WavePatternType::Sine, 0.2f, 0.5f, 0.0f, 1.0f);
    float lowFrequencyDifficulty = difficultyWave->ApplyWaveEffect(baseDifficulty, gameTime);
    
    SetWaveParameters(WavePatternType::Sine, 0.2f, 2.0f, 0.0f, 1.0f);
    float highFrequencyDifficulty = difficultyWave->ApplyWaveEffect(baseDifficulty, gameTime);
    
    // Test different phases
    SetWaveParameters(WavePatternType::Sine, 0.2f, 1.0f, 0.0f, 1.0f);
    float zeroPhaseValue = difficultyWave->ApplyWaveEffect(baseDifficulty, gameTime);
    
    SetWaveParameters(WavePatternType::Sine, 0.2f, 1.0f, 3.14159f, 1.0f);
    float piPhaseValue = difficultyWave->ApplyWaveEffect(baseDifficulty, gameTime);
    
    // Test different baselines
    SetWaveParameters(WavePatternType::Sine, 0.2f, 1.0f, 0.0f, 0.9f);
    float lowBaselineDifficulty = difficultyWave->ApplyWaveEffect(baseDifficulty, gameTime);
    
    SetWaveParameters(WavePatternType::Sine, 0.2f, 1.0f, 0.0f, 1.1f);
    float highBaselineDifficulty = difficultyWave->ApplyWaveEffect(baseDifficulty, gameTime);
    
    // Verify that parameters affect the output
    EXPECT_GT(highAmplitudeDifficulty, lowAmplitudeDifficulty) << "Higher amplitude should result in higher difficulty at t=0.25";
    EXPECT_NE(lowFrequencyDifficulty, highFrequencyDifficulty) << "Different frequencies should result in different difficulties";
    EXPECT_NE(zeroPhaseValue, piPhaseValue) << "Different phases should result in different difficulties";
    EXPECT_GT(highBaselineDifficulty, lowBaselineDifficulty) << "Higher baseline should result in higher difficulty";
}

} // namespace
} // namespace devilution
