@echo off
echo Setting up build environment...

echo Copying our modified files to the Release build configuration...
copy /Y "Source\engine\render\clx_render.cpp" "build\x64-Release\Source\engine\render\clx_render.cpp"

echo Building from the Release configuration...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
cmake --build build\x64-Release --config Release

echo Build complete.
pause
