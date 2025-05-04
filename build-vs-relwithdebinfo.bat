@echo off
echo Setting up Visual Studio environment...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"

echo Configuring CMake for RelWithDebInfo build with Visual Studio...
cmake -S . -B build-vs -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=RelWithDebInfo

echo Building RelWithDebInfo configuration...
cmake --build build-vs --config RelWithDebInfo

echo Build complete.
pause
