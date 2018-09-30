@echo off
mkdir _build
cd _build
"../cmake/bin/cmake.exe" -G "Visual Studio 15 2017" ..
