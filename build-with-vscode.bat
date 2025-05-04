@echo off
echo Building DevilutionX using VS Code tasks...

echo Step 1: Running VS Code task to configure CMake
cd %~dp0
code --task "Configure – Visual Studio"
timeout /t 5

echo Step 2: Running VS Code task to build with Visual Studio
code --task "Build – Visual Studio"

echo Build process initiated through VS Code.
echo.
echo Note: This will use the build configuration defined in your .vscode/tasks.json file.
echo If you encounter any issues, please check the VS Code output panel for details.
