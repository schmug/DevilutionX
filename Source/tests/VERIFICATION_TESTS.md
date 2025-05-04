# Dynamic Difficulty System Verification Tests

This document provides instructions for running the verification tests for the dynamic difficulty system and interpreting the results.

## Overview

The verification test suite includes:

1. **Unit Tests** - Test individual components in isolation
2. **Integration Tests** - Test interactions between components
3. **System Verification Tests** - Simulate complete game scenarios

## Running the Tests

### Prerequisites

- CMake 3.15 or higher
- C++14 compatible compiler
- Google Test framework (automatically downloaded by CMake)

### Build Tests

```bash
# From the project root directory
mkdir build
cd build
cmake .. -DDEVILUTIONX_TEST=ON
cmake --build . --target tests
```

### Run All Tests

```bash
# From the build directory
ctest -V
```

### Run Specific Test Categories

```bash
# Run only unit tests
./bin/unit_tests

# Run only integration tests
./bin/integration_tests

# Run only system verification tests
./bin/system_verification_tests
```

### Run Individual Tests

```bash
# Run specific test by name
./bin/unit_tests --gtest_filter=GearScoreTest.CalculateItemScore
./bin/integration_tests --gtest_filter=DifficultySystemTest.MonsterScaling
./bin/system_verification_tests --gtest_filter=DifficultySystemVerificationTest.RunAllScenarios
```

## Test Categories

### Unit Tests

| Test File | Description |
|-----------|-------------|
| `gear_score_test.cpp` | Tests gear scoring calculations |
| `difficulty_wave_test.cpp` | Tests wave pattern generation |
| `monster_data_test.cpp` | Tests monster data loading |

### Integration Tests

| Test File | Description |
|-----------|-------------|
| `difficulty_system_test.cpp` | Tests difficulty calculation and monster scaling |
| `difficulty_integration_test.cpp` | Tests complete system flow |

### System Verification Tests

| Test Scenario | Description |
|---------------|-------------|
| `BasicFunctionalityScenario` | Tests core functionality |
| `GearProgressionScenario` | Tests difficulty changes as gear improves |
| `MultiplayerScenario` | Tests multiplayer difficulty calculation |
| `WaveEffectsScenario` | Tests wave-like variations in difficulty |
| `SaveLoadScenario` | Tests saving and loading difficulty state |
| `EdgeCaseScenario` | Tests boundary conditions and extreme cases |

## Test Reports

### Automated Test Reports

The system verification tests automatically generate a report file:

```
difficulty_system_verification_report.txt
```

This report contains:
- Overall test success rate
- Results for each test scenario
- Detailed information about each test step

### Interpreting Test Results

#### Success Criteria

A successful test run should show:

1. All unit tests pass
2. All integration tests pass
3. All system verification scenarios pass

#### Common Failure Patterns

| Failure Pattern | Possible Cause | Resolution |
|-----------------|----------------|------------|
| Monster scaling test failures | Incorrect file paths or missing monster data | Check file paths in `LoadMonstDat()` function |
| Wave effect inconsistencies | Incorrect wave parameters | Verify wave configuration in `difficulty_wave_config.json` |
| Multiplayer difficulty imbalance | Incorrect weighting factors | Adjust weights in `mp_difficulty_config.json` |
| Save/load test failures | Incompatible save format | Update save format version handling |

## Debugging Failed Tests

### Logging

Enable verbose logging to see detailed test execution:

```bash
# Set environment variable before running tests
export DEVILUTIONX_LOG_LEVEL=VERBOSE
./bin/system_verification_tests
```

### Debugging Specific Components

1. **Gear Scoring Issues**:
   - Check `gear_score.cpp` implementation
   - Verify item base scores and affix values

2. **Difficulty Calculation Issues**:
   - Check `difficulty_manager.cpp` implementation
   - Verify transition smoothing logic

3. **Monster Scaling Issues**:
   - Check `monster_scaling.cpp` implementation
   - Verify scaling factors for different monster types

4. **Wave Effect Issues**:
   - Check `difficulty_wave.cpp` implementation
   - Verify wave pattern generation and application

## Extending the Test Suite

### Adding New Unit Tests

1. Create a new test file in `Source/tests/`
2. Include the appropriate headers
3. Define test fixtures and test cases using Google Test
4. Add the test file to `CMakeLists.txt`

### Adding New System Verification Scenarios

1. Open `difficulty_system_verification_tests.cpp`
2. Add a new method to `DifficultySystemVerificationTest` class
3. Create a new `TestScenario` with setup and test steps
4. Add the scenario to the `SetUp()` method

Example:

```cpp
void AddNewScenario() {
    auto scenario = std::make_unique<TestScenario>(
        "New Scenario Name",
        "Description of what this scenario tests"
    );
    
    // Add setup steps
    scenario->AddSetupStep([](GameSimulation& sim) {
        // Setup code here
    });
    
    // Add test steps
    scenario->AddTestStep([](GameSimulation& sim, std::string& message) {
        // Test code here
        message = "Test result details";
        return testPassed; // true or false
    });
    
    verificationTest->AddScenario(std::move(scenario));
}
```

## Continuous Integration

The test suite is designed to be run as part of a CI/CD pipeline. The following steps are recommended:

1. Run unit tests on every commit
2. Run integration tests on every pull request
3. Run system verification tests before each release

## Troubleshooting

### Common Issues

1. **Tests fail to build**:
   - Ensure Google Test is properly installed
   - Check CMake configuration

2. **Tests crash during execution**:
   - Check for null pointers and uninitialized variables
   - Verify that all required files exist

3. **Inconsistent test results**:
   - Check for race conditions in multithreaded code
   - Verify that tests clean up after themselves

### Getting Help

If you encounter issues with the test suite:

1. Check the logs for detailed error information
2. Review the edge cases documentation in `Source/difficulty/EDGE_CASES.md`
3. Contact the development team for assistance
