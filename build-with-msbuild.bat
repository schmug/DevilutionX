@echo off
echo Setting up build environment...

echo Initializing Visual Studio environment...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"

echo Building with MSBuild in RelWithDebInfo configuration...
cd build
msbuild DevilutionX.sln /p:Configuration=RelWithDebInfo /p:Platform=x64

echo Build complete.
pause
