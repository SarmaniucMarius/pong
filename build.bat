@echo off

pushd ..\build

cl -nologo -Zi ..\code\win32_platform.cpp user32.lib gdi32.lib

popd