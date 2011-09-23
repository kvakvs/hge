@echo off
mkdir .build-msvc8-2005
cd .build-msvc8-2005
"../cmake/bin/cmake.exe" -G "Visual Studio 8 2005" ..
