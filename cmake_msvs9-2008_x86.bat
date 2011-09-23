@echo off
mkdir .build-msvc9-2008
cd .build-msvc9-2008
"../cmake/bin/cmake.exe" -G "Visual Studio 9 2008" ..
