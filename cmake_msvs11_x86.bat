@echo off
mkdir .build-msvc11
cd .build-msvc11
"../cmake/bin/cmake.exe" -G "Visual Studio 11" ..
