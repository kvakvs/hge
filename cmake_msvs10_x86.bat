@echo off
mkdir .build-msvc10
cd .build-msvc10
"../cmake/bin/cmake.exe" -G "Visual Studio 10" ..
