@echo off
echo Setting up Visual Studio environment...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"

echo Creating fresh build directory...
mkdir build-ninja-relwithdebinfo

echo Configuring CMake for RelWithDebInfo build with Ninja...
cmake -S . -B build-ninja-relwithdebinfo -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo

echo Building RelWithDebInfo configuration...
cmake --build build-ninja-relwithdebinfo

echo Build complete.
pause
