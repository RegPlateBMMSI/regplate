@echo off

del report.txt 2> NUL

for /F "tokens=*" %%A in (testdata.txt) do Debug\test.exe photos\%%A.jpg >> report.txt 2> NUL && echo|set /p=* 

echo OK
