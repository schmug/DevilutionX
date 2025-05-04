@echo off
echo Opening DevilutionX project in Visual Studio...

echo Initializing Visual Studio environment...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"

echo Opening Visual Studio solution...
start "" "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe" "C:\Users\coryr\DevilutionX\build\x64-RelWithDebInfo\build.ninja"

echo Visual Studio should now be opening.
echo.
echo Build steps:
echo 1. In Visual Studio, select the RelWithDebInfo configuration
echo 2. Build the project (F7 or Build menu)
echo.
echo Note: Our changes to clx_render.cpp should resolve the template parameter deduction issues.
echo We've explicitly cast the width and height parameters to uint_fast16_t to avoid ambiguity.
