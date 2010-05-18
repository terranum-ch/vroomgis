@echo off
if [%1] equ [] echo Chemin de visual studio manquant & goto :eof
if [%2] equ [] echo Chemin des sources de GDAL manquant & goto :eof

set mydir=%CD%

echo %1/vcvarsall.bat
call %1/vcvarsall.bat

cd %2
nmake -f makefile.vc
nmake -f makefile.vc install
REM nmake -f makefile.vc devinstall
cd %mydir%
