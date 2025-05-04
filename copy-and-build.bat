@echo off
echo Setting up build environment...

echo Initializing Visual Studio environment...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"

echo Setting VCPKG_ROOT environment variable...
set VCPKG_ROOT=C:\vcpkg

echo Step 1: Configure CMake with Visual Studio generator...
cmake -S . -B build-vs -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake

echo Step 2: Building DevilutionX with RelWithDebInfo configuration...
cmake --build build-vs --config RelWithDebInfo

echo Build complete.
pause
