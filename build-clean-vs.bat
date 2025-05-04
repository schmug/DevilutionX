@echo off
echo Building DevilutionX with Visual Studio (clean build)...

echo Initializing Visual Studio environment...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"

echo Setting VCPKG_ROOT environment variable...
set VCPKG_ROOT=C:\vcpkg

echo Creating clean build directory...
if exist "build-vs-clean" rmdir /s /q "build-vs-clean"
mkdir build-vs-clean
cd build-vs-clean

echo Configuring CMake with Visual Studio generator...
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=RelWithDebInfo -DENABLE_NETWORKING=OFF -DCMAKE_POLICY_VERSION_MINIMUM=3.5

echo Building the project in RelWithDebInfo configuration...
cmake --build . --config RelWithDebInfo

if %ERRORLEVEL% NEQ 0 (
  echo Build failed with error code %ERRORLEVEL%
  echo Please check the output above for specific errors.
) else (
  echo Build successful! The DevilutionX game executable has been compiled successfully.
  echo.
  echo You can now run the game to verify that the visual artifacts have been fixed.
  echo The executable should be located at: build-vs-clean\bin\RelWithDebInfo\devilutionx.exe
)

pause
