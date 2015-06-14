#!/bin/bash

printf "\x1f\x8b\x08\x00\x00\x00\x00\x00" |cat - $1.mexe |gzip -dc > $1.out
../../../qt_workspaces/build-Morpho2-Desktop_Qt_5_3_GCC_64bit-Debug/Morpho2 -dump $1.out > $1.asm
