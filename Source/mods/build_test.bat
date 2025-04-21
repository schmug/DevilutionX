@echo off
echo Building Drop Rate Modification System Test...

REM Set paths for Visual Studio compiler
set VCVARSALL="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
if not exist %VCVARSALL% set VCVARSALL="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
if not exist %VCVARSALL% (
    echo Visual Studio not found. Please update the script with the correct path to vcvarsall.bat
    exit /b 1
)

REM Initialize Visual Studio environment
call %VCVARSALL% x64

REM Create build directory
if not exist build mkdir build

REM Compile the test program
cl.exe /EHsc /std:c++17 /I.. /I..\.. /Fe:build\test_drop_rates.exe ..\mods\test_drop_rates.cpp ..\mods\drop_rate_modifier.cpp ..\mods\config\drop_rate_config.cpp ..\mods\config\drop_rate_validator.cpp ..\utils\paths.cpp ..\utils\log.cpp ..\utils\file_util.cpp

if %ERRORLEVEL% NEQ 0 (
    echo Compilation failed!
    exit /b 1
)

echo Build completed successfully.
echo Running test program...
cd build
test_drop_rates.exe
cd ..

echo Test completed.
