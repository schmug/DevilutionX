@echo off
echo Setting up Visual Studio environment...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"

echo Configuring CMake for RelWithDebInfo build...
cmake -S . -B build\x64-RelWithDebInfo -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo

echo Building RelWithDebInfo configuration...
cmake --build build\x64-RelWithDebInfo

echo Build complete.
pause
