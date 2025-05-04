@echo off
echo Copying fixed monster files to the build directory...

REM Create the necessary directories if they don't exist
mkdir "build\x64-RelWithDebInfo\txtdata\monsters" 2>nul
mkdir "build\x64-RelWithDebInfo\assets\txtdata\monsters" 2>nul
mkdir "build\x64-RelWithDebInfo\txtdata\monsters\falspear" 2>nul
mkdir "build\x64-RelWithDebInfo\assets\txtdata\monsters\falspear" 2>nul

REM Copy the fixed test_unique_monsters.tsv file to both possible locations
echo Copying fixed test_unique_monsters.tsv file...
copy /Y "txtdata\monsters\test_unique_monsters.tsv" "build\x64-RelWithDebInfo\txtdata\monsters\" 2>nul
copy /Y "txtdata\monsters\test_unique_monsters.tsv" "build\x64-RelWithDebInfo\assets\txtdata\monsters\" 2>nul

REM Copy translation files for monsters
echo Copying monster translation files...
copy /Y "txtdata\monsters\falspear\carver.trn" "build\x64-RelWithDebInfo\txtdata\monsters\falspear\" 2>nul
copy /Y "txtdata\monsters\falspear\carver.trn" "build\x64-RelWithDebInfo\assets\txtdata\monsters\falspear\" 2>nul

REM Copy other monster files to ensure they're all available
echo Copying other monster files...
copy /Y "txtdata\monsters\fixed_monsters.tsv" "build\x64-RelWithDebInfo\txtdata\monsters\" 2>nul
copy /Y "txtdata\monsters\new_monsters.tsv" "build\x64-RelWithDebInfo\txtdata\monsters\" 2>nul
copy /Y "txtdata\monsters\new_monsters_fixed.tsv" "build\x64-RelWithDebInfo\txtdata\monsters\" 2>nul
copy /Y "txtdata\monsters\test_monsters.tsv" "build\x64-RelWithDebInfo\txtdata\monsters\" 2>nul
copy /Y "txtdata\monsters\new_unique_monsters.tsv" "build\x64-RelWithDebInfo\txtdata\monsters\" 2>nul

echo Files copied successfully!
echo You can now run the game to test if the error is fixed.
pause
