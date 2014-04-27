#!/bin/sh

for d in include src/core src/fontconv src/fonted src/helpers src/libPNG src/particleed src/pngopt src/texasm src/zlib-1.2.5 ; do
    rm $d/*.orig
    astyle --style=stroustrup --add-brackets --max-code-length=100 $d/*.cpp $d/*.h
done