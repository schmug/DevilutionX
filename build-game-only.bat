@echo off
echo Building DevilutionX game executable (without networking)...

echo Initializing Visual Studio environment...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"

echo Setting VCPKG_ROOT environment variable...
set VCPKG_ROOT=C:\vcpkg

echo Configuring CMake with networking disabled...
cd build
cmake .. -DENABLE_NETWORKING=OFF -DCMAKE_BUILD_TYPE=RelWithDebInfo

echo Building the project in RelWithDebInfo configuration...
cmake --build . --config RelWithDebInfo

if %ERRORLEVEL% NEQ 0 (
  echo Build failed with error code %ERRORLEVEL%
  echo Please check the output above for specific errors.
) else (
  echo Build successful! The DevilutionX game executable has been compiled successfully.
  echo.
  echo You can now run the game to verify that the visual artifacts have been fixed.
  echo The executable should be located at: build\bin\devilutionx.exe
)

pause
