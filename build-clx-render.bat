@echo off
echo Building clx_render.cpp with Visual Studio...

echo Initializing Visual Studio environment...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"

echo Setting VCPKG_ROOT environment variable...
set VCPKG_ROOT=C:\vcpkg

echo Building the project in RelWithDebInfo configuration...
cd build\x64-RelWithDebInfo
cmake --build . --target libdevilutionx_clx_render --config RelWithDebInfo

if %ERRORLEVEL% NEQ 0 (
  echo Build failed with error code %ERRORLEVEL%
  echo Please check the output above for specific errors.
) else (
  echo Build successful! The clx_render.cpp file has been compiled successfully.
  echo.
  echo You can now build the full project to see if our changes fixed the visual artifacts.
)

pause
