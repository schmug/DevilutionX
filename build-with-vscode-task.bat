@echo off
echo Building DevilutionX with VS Code tasks...

echo Step 1: Configure with CMake
cd %~dp0
call code --task "Configure – CMake"
timeout /t 5

echo Step 2: Build with CMake
call code --task "Build – CMake"

echo Build process initiated. Check VS Code output for build progress.
echo.
echo Note: If you prefer to build with Visual Studio instead, run:
echo code --task "Configure – Visual Studio"
echo code --task "Build – Visual Studio"
