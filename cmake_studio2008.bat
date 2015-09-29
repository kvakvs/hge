@echo off
mkdir _build
cd _build
"../cmake/bin/cmake.exe" -G "Visual Studio 9 2008" ..
