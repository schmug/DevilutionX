@echo off
echo Setting up environment to compile clx_render.cpp only...

echo Initializing Visual Studio environment...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"

echo Creating output directory if it doesn't exist...
if not exist "build-manual\Source\engine\render" mkdir "build-manual\Source\engine\render"

echo Compiling clx_render.cpp directly...
cl.exe /c /EHsc /std:c++20 /MD /O2 /Ob2 /DNDEBUG /DWIN32 /D_WINDOWS ^
  /I"Source" /I"3rdParty\tl" ^
  /Fo"build-manual\Source\engine\render\clx_render.obj" ^
  "Source\engine\render\clx_render.cpp"

if %ERRORLEVEL% NEQ 0 (
  echo Compilation failed with error code %ERRORLEVEL%
  echo Please check the output above for specific errors.
) else (
  echo Compilation successful!
  echo The object file is available at: build-manual\Source\engine\render\clx_render.obj
  echo.
  echo You can now replace this file in your existing build to test the changes.
)

pause
