@echo off
echo Setting up build environment for DevilutionX...

echo Initializing Visual Studio environment...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"

echo Setting VCPKG_ROOT environment variable...
set VCPKG_ROOT=C:\vcpkg

echo Creating build directory if it doesn't exist...
if not exist "build-vs" mkdir build-vs

echo Step 1: Configure CMake with Visual Studio generator...
cmake -S . -B build-vs -G "Visual Studio 17 2022" -A x64 ^
  -DCMAKE_BUILD_TYPE=RelWithDebInfo ^
  -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake

if %ERRORLEVEL% NEQ 0 (
  echo CMake configuration failed with error code %ERRORLEVEL%
  echo Please check the output above for specific errors.
  pause
  exit /b %ERRORLEVEL%
)

echo Step 2: Building DevilutionX with RelWithDebInfo configuration...
cmake --build build-vs --config RelWithDebInfo

if %ERRORLEVEL% NEQ 0 (
  echo Build failed with error code %ERRORLEVEL%
  echo Please check the output above for specific errors.
  pause
  exit /b %ERRORLEVEL%
)

echo Build complete successfully!
echo The executable should be available in build-vs\RelWithDebInfo\devilutionx.exe
pause
