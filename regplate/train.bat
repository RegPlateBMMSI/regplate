@echo off

del regplate.net 2> NUL

Debug\train.exe %1 %2 %3 %4 %5 %6 %7 %8 %9

echo OK