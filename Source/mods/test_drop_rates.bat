@echo off
REM Drop Rate Testing Script for DevilutionX
REM This script helps test drop rate configurations

echo DevilutionX Drop Rate Testing Script
echo ===================================
echo.

REM Check if droprate_tool exists
if not exist "droprate_tool.exe" (
    echo Error: droprate_tool.exe not found.
    echo Please build the tool first using:
    echo cmake -B build -S .
    echo cmake --build build --target droprate_tool
    exit /b 1
)

:menu
echo Choose an option:
echo 1. Generate drop rate report
echo 2. Simulate drops
echo 3. Compare drop rates
echo 4. Reload configuration
echo 5. Exit
echo.

set /p choice="Enter your choice (1-5): "

if "%choice%"=="1" goto generate_report
if "%choice%"=="2" goto simulate_drops
if "%choice%"=="3" goto compare_rates
if "%choice%"=="4" goto reload_config
if "%choice%"=="5" goto end

echo Invalid choice. Please try again.
goto menu

:generate_report
echo.
set /p report_path="Enter report output path (default: drop_rate_report.md): "
if "%report_path%"=="" set report_path=drop_rate_report.md
echo Generating drop rate report to %report_path%...
droprate_tool.exe report "%report_path%"
echo.
goto menu

:simulate_drops
echo.
set /p num_drops="Enter number of drops to simulate (default: 1000): "
if "%num_drops%"=="" set num_drops=1000
set /p monster_level="Enter monster level (default: 30): "
if "%monster_level%"=="" set monster_level=30
set /p dungeon_level="Enter dungeon level (default: 16): "
if "%dungeon_level%"=="" set dungeon_level=16
echo Simulating %num_drops% drops...
droprate_tool.exe simulate %num_drops% %monster_level% %dungeon_level%
echo.
goto menu

:compare_rates
echo.
set /p monster_level="Enter monster level (default: 30): "
if "%monster_level%"=="" set monster_level=30
set /p dungeon_level="Enter dungeon level (default: 16): "
if "%dungeon_level%"=="" set dungeon_level=16
echo Comparing drop rates...
droprate_tool.exe compare %monster_level% %dungeon_level%
echo.
goto menu

:reload_config
echo.
set /p config_path="Enter config path (leave empty for default): "
if "%config_path%"=="" (
    droprate_tool.exe reload
) else (
    droprate_tool.exe reload "%config_path%"
)
echo.
goto menu

:end
echo Thank you for using the Drop Rate Testing Script.
