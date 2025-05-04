@echo off
echo Opening DevilutionX project in Visual Studio...

:: Check if the build directory exists
if not exist "build\x64-RelWithDebInfo" (
  echo The build\x64-RelWithDebInfo directory does not exist.
  echo Please make sure you have configured the project with CMake first.
  pause
  exit /b 1
)

:: Open the CMake project in Visual Studio
echo Opening the project in Visual Studio...
start "" "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe" "build\x64-RelWithDebInfo"

echo Visual Studio should now be opening the project.
echo Please build the project with the RelWithDebInfo configuration.
echo.
echo Note: Our changes to clx_render.cpp should resolve the template parameter deduction issues.
echo If you encounter any further build errors, please let me know.
