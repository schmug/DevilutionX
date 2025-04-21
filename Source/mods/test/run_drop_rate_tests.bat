@echo off
REM Run the drop rate modification system tests

echo Running Item Drop Rate Modification System Tests
echo ===============================================
echo.

REM Set the current directory to the repository root
cd "..\..\..\DevilutionX"

REM Check if the build directory exists
if not exist "build" (
    echo Creating build directory...
    mkdir build
)

cd build

REM Configure the project with CMake
echo Configuring project with CMake...
cmake .. -DBUILD_TESTING=ON

REM Build the test script
echo Building test script...
cmake --build . --target drop_rate_test_script

REM Run the tests
echo.
echo Running tests...
echo.

if exist "bin\drop_rate_test_script.exe" (
    bin\drop_rate_test_script.exe test_output
) else if exist "bin\Debug\drop_rate_test_script.exe" (
    bin\Debug\drop_rate_test_script.exe test_output
) else (
    echo Error: Could not find the test executable.
    exit /b 1
)

echo.
echo Test report is available at: test_output\drop_rate_report.md
echo.

REM Return to the original directory
cd "..\Source\mods\test"

echo Done!
