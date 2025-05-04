@echo off
echo Copying monster files to the correct locations...

REM Create the necessary directories if they don't exist
mkdir "build\x64-RelWithDebInfo\assets\txtdata\monsters" 2>nul
mkdir "build\x64-RelWithDebInfo\txtdata\monsters" 2>nul

REM Copy monster files from txtdata to the build directory
echo Copying from txtdata\monsters to build\x64-RelWithDebInfo\txtdata\monsters
copy /Y "txtdata\monsters\fixed_monsters.tsv" "build\x64-RelWithDebInfo\txtdata\monsters\" 2>nul
copy /Y "txtdata\monsters\new_monsters.tsv" "build\x64-RelWithDebInfo\txtdata\monsters\" 2>nul
copy /Y "txtdata\monsters\new_monsters_fixed.tsv" "build\x64-RelWithDebInfo\txtdata\monsters\" 2>nul
copy /Y "txtdata\monsters\test_monsters.tsv" "build\x64-RelWithDebInfo\txtdata\monsters\" 2>nul
copy /Y "txtdata\monsters\test_unique_monsters.tsv" "build\x64-RelWithDebInfo\txtdata\monsters\" 2>nul
copy /Y "txtdata\monsters\new_unique_monsters.tsv" "build\x64-RelWithDebInfo\txtdata\monsters\" 2>nul

REM Copy monster files from assets to the build directory
echo Copying from assets\txtdata\monsters to build\x64-RelWithDebInfo\assets\txtdata\monsters
copy /Y "assets\txtdata\monsters\fixed_monsters.tsv" "build\x64-RelWithDebInfo\assets\txtdata\monsters\" 2>nul
copy /Y "assets\txtdata\monsters\new_monsters.tsv" "build\x64-RelWithDebInfo\assets\txtdata\monsters\" 2>nul
copy /Y "assets\txtdata\monsters\test_unique_monsters.tsv" "build\x64-RelWithDebInfo\assets\txtdata\monsters\" 2>nul
copy /Y "assets\txtdata\monsters\new_unique_monsters.tsv" "build\x64-RelWithDebInfo\assets\txtdata\monsters\" 2>nul

echo Monster files copied successfully!
echo You can now run the game to test if the "Invalid monster type: 150" error is fixed.
pause
