@echo off

call vcvarsall.bat x64

IF NOT EXIST build mkdir build
pushd build

IF NOT EXIST SDL2.dll copy ..\lib\x64\SDL2.dll SDL2.dll
IF NOT EXIST SDL2_image.dll copy ..\lib\x64\SDL2_image.dll SDL2_image.dll
IF NOT EXIST SDL2_ttf.dll copy ..\lib\x64\SDL2_ttf.dll SDL2_ttf.dll
IF NOT EXIST zlib1.dll copy ..\lib\x64\zlib1.dll zlib1.dll
IF NOT EXIST libfreetype-6.dll copy ..\lib\x64\libfreetype-6.dll libfreetype-6.dll

set compiler_flags=-WL -Od -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4
set compiler_flags=-wd4201 -wd4100 -wd4189 -wd4505 -wd4127 -FC -Zi -GS- -Gs9999999 %compiler_flags%
set compiler_flags=-Od -DBUILD_DEBUG=1 -I ../include %compiler_flags%

cl %compiler_flags% ../code/main.c -link ..\lib\x64\SDL2.lib ..\lib\x64\SDL2_image.lib ..\lib\x64\SDL2_ttf.lib

popd
