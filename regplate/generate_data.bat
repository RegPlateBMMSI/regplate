@echo off
del tmp.train 
del tmp.test  

for /F "tokens=*" %%A in (traindata.txt) do Debug\generate_data.exe photos\%%A.jpg >> tmp.train 
for /F "tokens=*" %%A in (testdata.txt)  do Debug\generate_data.exe photos\%%A.jpg >> tmp.test  

setlocal EnableDelayedExpansion

set "cmd=findstr /R /N "^^" tmp.train | find /C ":""
for /f %%a in ('!cmd!') do set number=%%a
SET /a lineNumbers=%number%/2
echo %lineNumbers% 96 36 > regplate.train

set "cmd=findstr /R /N "^^" tmp.test  | find /C ":""
for /f %%a in ('!cmd!') do set number=%%a
SET /a lineNumbers=%number%/2
echo %lineNumbers% 96 36 > regplate.test


more tmp.train >> regplate.train

more tmp.test >> regplate.test
del tmp.test

echo OK
