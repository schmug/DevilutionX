@echo off
echo Setting up build environment...

echo Initializing Visual Studio environment...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"

echo Compiling clx_render.cpp with RelWithDebInfo configuration...
cd build\x64-RelWithDebInfo
cl.exe /nologo /TP -DASIO_STANDALONE -DDEVILUTIONX_DEFAULT_RESAMPLER=Speex -DDEVILUTIONX_DISPLAY_TEXTURE_FORMAT=SDL_PIXELFORMAT_RGB888 -DDEVILUTIONX_PALETTE_TRANSPARENCY_BLACK_16_LUT -DDEVILUTIONX_RESAMPLER_SDL -DDEVILUTIONX_RESAMPLER_SPEEX -DDEVILUTIONX_SCREENSHOT_FORMAT=DEVILUTIONX_SCREENSHOT_FORMAT_PNG -DFMT_SHARED -DPACKET_ENCRYPTION -DWINVER=0x0601 -D_CRT_SECURE_NO_WARNINGS -D_SILENCE_CXX20_U8PATH_DEPRECATION_WARNING -D_WIN32_WINDOWS=0x0601 -D_WIN32_WINNT=0x0601 -I..\..\Source -I..\..\3rdParty\tl -external:I.\vcpkg_installed\x64-windows\include -external:I.\vcpkg_installed\x64-windows\include\SDL2 -external:W0 /DWIN32 /D_WINDOWS /EHsc /O2 /Ob2 /DNDEBUG -std:c++20 -MD /W3 /Zc:__cplusplus /utf-8 /Fo"Source\CMakeFiles\libdevilutionx_clx_render.dir\engine\render\clx_render.cpp.obj" /Fd"Source\CMakeFiles\libdevilutionx_clx_render.dir\" /FS -c ..\..\Source\engine\render\clx_render.cpp

echo Compilation complete.
pause
