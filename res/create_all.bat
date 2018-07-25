@echo off

set ORGICON=%1
set CURRENT_DIR=%NEOLINKS_HOME%\cmake_modules\windows
set NEW_DIR=%CURRENT_DIR%\windows_icon

cd "C:\Program Files\Inkscape"

c:
mkdir %NEW_DIR%

call:convert 16
call:convert 22
call:convert 24
call:convert 32
call:convert 48
call:convert 64
call:convert 128
call:convert 256
call:convert 512
call:convert 1024

cd %NEW_DIR%
d:

echo Windows icon

magick icon16x16.png icon32x32.png icon48x48.png icon256x256.png icon.ico

echo Linux icons

magick icon32x32.png icon.xpm

echo OS X icon

png2icns icon.icns icon16x16.png icon32x32.png icon64x64.png icon128x128.png icon256x256.png icon512x512.png icon1024x1024.png

echo NSIS header

magick header.png -type truecolor bmp3:header.bmp

echo Done
pause

goto:eof

:convert
set SIZE=%~1
rem  2> nul
inkscape -z -f %ORGICON% -e %NEW_DIR%\icon%SIZE%x%SIZE%.png -h %SIZE% -y 0
goto:eof
