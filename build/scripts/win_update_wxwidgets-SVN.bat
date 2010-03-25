REM Update and build wxWidgets

@SET TRUNKDIR=D:\LS\PROGRAMATION\LIB\wxMSW-SVN
@SET BUILDDIR=%TRUNKDIR%\build\msw
@SET VSPARAMDIR="C:\Program Files\Microsoft Visual Studio 9.0\VC\"


echo This script update the wxWidgets
echo library and build the lib.



ECHO 0) Loading Visual Studio parameters
c:
cd %VSPARAMDIR%
call vcvarsall.bat x86


ECHO ----------------------------------------
ECHO         UPDATE WXWIDGETS 
ECHO     (c) Lucien Schreiber CREALP
ECHO ----------------------------------------
ECHO 1) Updating SVN ...

D:
cd %trunkdir%
svn update

FOR /F "usebackq" %%s IN (`svnversion .`) DO @SET REV=%%s
REM ECHO  #define SVN_VERSION _T(^"%REV%^")

ECHO 1) Updating SVN ... DONE (version is : %REV%)

REM REMOVING EXISTING FILES
ECHO 2) Removing existing files
del /F /S /Q %trunkdir%\build\msw\vc_mswu
del /F /S /Q %trunkdir%\build\msw\vc_mswud
del /F /S /Q %trunkdir%\lib\vc_lib
ECHO 2) Removing existing files ... DONE

ECHO -----------------------------------------------
ECHO 3) BUILDING TOOLMAP2 (MAY TAKE SOME TIMES)-----
ECHO -----------------------------------------------

ECHO COPYING setup.h
copy  /Y %trunkdir%\include\wx\msw\setup0.h %trunkdir%\include\wx\setup.h 
copy  /Y %trunkdir%\include\wx\msw\setup0.h %trunkdir%\include\wx\msw\setup.h

ECHO 3a) BUILDING DEBUG VERSION
cd %builddir%
nmake -f makefile.vc BUILD=debug CPPFLAGS="/MTd" UNICODE=1 MONOLITHIC=1

ECHO 3b) BUILDING RELEASE VERSION
cd %builddir%
nmake -f makefile.vc BUILD=release CPPFLAGS="/MT" UNICODE=1 MONOLITHIC=1



:WaitForEnter
  set wait=
  set /p wait=Please press ENTER when wxWidgets is build successfully: 
  IF NOT DEFINED wait goto :eof 
  echo Please press only the ENTER key and nothing else
  goto :WaitForEnter