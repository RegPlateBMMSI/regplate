@echo off
del tmp.train

for /F "tokens=*" %%A in (traindata.txt) do ..\Debug\regplate.exe zdjecia\%%A.jpg >> tmp.train

setlocal EnableDelayedExpansion
set "cmd=findstr /R /N "^^" tmp.train | find /C ":""

for /f %%a in ('!cmd!') do set number=%%a
SET /a lineNumbers=%number%/2
echo %lineNumbers% 96 36 > result.train
more tmp.train >> result.train
del tmp.train
echo OK
