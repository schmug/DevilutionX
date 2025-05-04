@echo off
echo Moving monster data files to the correct location...

REM Create the destination directory if it doesn't exist
mkdir "assets\txtdata\monsters" 2>nul

REM Copy files from txtdata\monsters to assets\txtdata\monsters
if exist "txtdata\monsters\new_unique_monsters.tsv" (
    copy "txtdata\monsters\new_unique_monsters.tsv" "assets\txtdata\monsters\" /Y
    echo Copied new_unique_monsters.tsv from txtdata\monsters to assets\txtdata\monsters
)

if exist "txtdata\monsters\test_unique_monsters.tsv" (
    copy "txtdata\monsters\test_unique_monsters.tsv" "assets\txtdata\monsters\" /Y
    echo Copied test_unique_monsters.tsv from txtdata\monsters to assets\txtdata\monsters
)

REM Copy files from monsters directory if they exist
if exist "monsters\new_unique_monsters.tsv" (
    copy "monsters\new_unique_monsters.tsv" "assets\txtdata\monsters\" /Y
    echo Copied new_unique_monsters.tsv from monsters to assets\txtdata\monsters
)

if exist "monsters\test_unique_monsters.tsv" (
    copy "monsters\test_unique_monsters.tsv" "assets\txtdata\monsters\" /Y
    echo Copied test_unique_monsters.tsv from monsters to assets\txtdata\monsters
)

if exist "monsters\monstdat.tsv" (
    copy "monsters\monstdat.tsv" "assets\txtdata\monsters\" /Y
    echo Copied monstdat.tsv from monsters to assets\txtdata\monsters
)

if exist "monsters\unique_monstdat.tsv" (
    copy "monsters\unique_monstdat.tsv" "assets\txtdata\monsters\" /Y
    echo Copied unique_monstdat.tsv from monsters to assets\txtdata\monsters
)

echo Monster data file organization complete!
