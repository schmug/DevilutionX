# Monster Additions Project

## Overview
This document tracks the progress of adding new monsters to DevilutionX, including Diablo 2, 3, 4, and Path of Exile inspired monsters.

## Current Status
- Added new monster types in `monstdat.h`
- Created monster data files in `assets/txtdata/monsters/`
- Added support for loading monster data from TSV files
- Implemented new monster sprite mapping system

## Build Issues and Resolution
We encountered build issues when making extensive changes to multiple monster-related files simultaneously. The specific problems included:

1. Corrupted or improperly merged code in `monstdat.cpp` and `monstdat.h`
2. Build failures with Ninja-based builds
3. Dependency issues with the Lua library

### Resolution
- Restored core files to a clean state
- Used Visual Studio-based build instead of Ninja
- Implemented changes incrementally with proper testing between additions
- Created backup scripts to preserve work in case of build failures

## Next Steps
1. Complete implementation of new monster types
2. Add proper sprite mappings for all new monsters
3. Balance monster stats and abilities
4. Implement unique monster variants
5. Add proper drop tables for new monsters

## Build Instructions
For this project, we recommend using the Visual Studio-based build approach:
```
.\build-vs-relwithdebinfo.bat
```

If encountering build issues, try:
1. Restore core files: `git restore Source/`
2. Use the VS-based build instead of Ninja
3. Make smaller, incremental changes rather than large batches
