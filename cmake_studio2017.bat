@echo off
mkdir _build
cd _build
"../cmake/bin/cmake.exe" -A Win32 -G "Visual Studio 15 2017" ..
