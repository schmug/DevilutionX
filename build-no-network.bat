@echo off
echo Building DevilutionX with networking disabled...

echo Initializing Visual Studio environment...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"

echo Setting VCPKG_ROOT environment variable...
set VCPKG_ROOT=C:\vcpkg

echo Creating a clean build directory...
if exist "build-no-network" rmdir /s /q "build-no-network"
mkdir build-no-network
cd build-no-network

echo Configuring CMake with networking disabled...
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=RelWithDebInfo -DENABLE_NETWORKING=OFF -DCMAKE_POLICY_VERSION_MINIMUM=3.5

echo Building the project in RelWithDebInfo configuration...
cmake --build . --config RelWithDebInfo

if %ERRORLEVEL% NEQ 0 (
  echo Build failed with error code %ERRORLEVEL%
  echo Please check the output above for specific errors.
) else (
  echo Build successful! The DevilutionX game executable has been compiled successfully.
  echo.
  echo You can now run the game to verify that it works without crashing.
  echo The executable should be located at: build-no-network\bin\RelWithDebInfo\devilutionx.exe
)

pause
