@echo off

call vcvarsall.bat x64

IF NOT EXIST build mkdir build
pushd build

IF NOT EXIST SDL2.dll copy ..\lib\x64\SDL2.dll SDL2.dll
IF NOT EXIST SDL2_image.dll copy ..\lib\x64\SDL2_image.dll SDL2_image.dll

set compiler_flags=-FC -DBUILD_DEBUG=1 -I ../include

cl %compiler_flags% ../code/main.c -link ..\lib\x64\SDL2.lib ..\lib\x64\SDL2_image.lib

popd
