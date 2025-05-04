@echo off
echo Setting up build environment...

echo Initializing Visual Studio environment...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"

echo Rebuilding the clx_render target with RelWithDebInfo configuration...
cd build\x64-RelWithDebInfo
cmake --build . --target libdevilutionx_clx_render --config RelWithDebInfo

echo Build complete.
if %ERRORLEVEL% EQU 0 (
  echo Successfully rebuilt the clx_render target!
) else (
  echo Build failed with error code %ERRORLEVEL%
)
pause
