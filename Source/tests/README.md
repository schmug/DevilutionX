# Dynamic Difficulty System Tests

## Overview

This directory contains tests for the dynamic difficulty system, including unit tests for individual components and integration tests for the system as a whole. The tests verify that the system correctly calculates difficulty levels, applies wave effects, scales monster stats, and handles file loading properly.

## Test Files

### `monster_data_test.cpp`

Tests for monster data file loading:
- Verifies that monster data files exist in the correct locations
- Tests that monster data can be loaded successfully
- Checks that the loaded data contains expected entries

### `difficulty_system_test.cpp`

Tests for the dynamic difficulty system:
- Difficulty calculation for different player types
- Difficulty transition smoothing
- Difficulty wave effects
- Monster stat scaling
- Integration tests for the complete system

## Running the Tests

To run the tests, use the following command:

```bash
cd build
cmake --build . --target test
```

## File Path Fix

We fixed an issue with monster data file loading by updating the file paths in `monstdat.cpp`. The correct paths are:

- `assets/txtdata/monsters/monstdat.tsv` (was `monsters/monstdat.tsv`)
- `assets/txtdata/monsters/unique_monstdat.tsv` (was `monsters/unique_monstdat.tsv`)
- `assets/txtdata/monsters/fixed_monsters.tsv` (was `monsters/fixed_monsters.tsv`)
- `assets/txtdata/monsters/new_monsters.tsv` (was `monsters/new_monsters.tsv`)

This fix ensures that the monster data files are loaded from the correct locations, preventing the "File not found" error.

## Test Coverage

The tests cover the following aspects of the dynamic difficulty system:

1. **File Loading**
   - Monster data file existence
   - Successful loading of monster data
   - Handling of missing files

2. **Difficulty Calculation**
   - Calculating difficulty based on player level and gear
   - Handling different player types (low level/gear, high level/gear, outliers)

3. **Difficulty Transitions**
   - Smooth transitions between difficulty levels
   - Proper application of smoothing algorithms

4. **Wave Effects**
   - Generation of different wave patterns
   - Application of wave effects to base difficulty
   - Variation of difficulty over time

5. **Monster Scaling**
   - Scaling of monster stats based on difficulty
   - Verification that higher difficulty results in stronger monsters

6. **Integration**
   - End-to-end testing of the complete system
   - Verification that all components work together correctly

## Edge Cases

The tests also cover several edge cases:

- Players with mismatched level and gear (e.g., low level with high gear)
- Extreme difficulty values (very low or very high)
- Different wave patterns and parameters
- Boundary conditions for monster scaling
