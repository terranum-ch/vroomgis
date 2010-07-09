@ECHO OFF 
REM script for updating vroomGIS app under windows
REM works only for windows
REM 1) Update the svn repositories
REM 2) Run cmake to update the Visual studio solution
REM 3) Run (manually) the vroomGIS compilation
REM 4) Launch the app


@SET TRUNKDIR=D:\LS\PRJ\COLTOPGIS\trunk-vroomgis
@SET BINDIR=D:\LS\PRJ\COLTOPGIS\bin\vroomloader
@SET BINDIR2=D:\LS\PRJ\COLTOPGIS\bin\vroomtwin
@SET VSDIR=C:\Program Files\Microsoft Visual Studio 9.0\Common7\IDE


@SET PARAMCXX=D:/LS/PROGRAMATION/LIB/cxxtest
@SET PARAMGIS=D:/LS/PROGRAMATION/LIB/LIB_GDAL
@SET PARAMGEOS=D:/LS/PROGRAMATION/LIB/geos-3.2.2
@SET PARAMWXWIN=D:/LS/PROGRAMATION/LIB/wxMSW-SVN



ECHO ----------------------------------------
ECHO         UPDATE VROOMGIS APP
ECHO 			VROOMLOADER
ECHO     (c) Lucien Schreiber CREALP
ECHO ----------------------------------------
ECHO 1) Updating repositories ...

D:
cd %trunkdir%
svn update

REM GETTING SUBVERSION NUMBER
FOR /F "usebackq" %%s IN (`svnversion .`) DO @SET REV=%%s
REM ECHO  #define SVN_VERSION _T(^"%REV%^")

ECHO 1) Updating repositories ... DONE (version is : %REV%)

SET WXWIN=%PARAMWXWIN%
echo %WXWIN%


ECHO 2) Making Visual studio solution...
cd %bindir%
cmake %trunkdir%\app\vroomloader\build -G "Visual Studio 9 2008" -DCXXTEST_DIRECTORY:PATH=%PARAMCXX% -DSEARCH_GDAL:BOOL=1 -DSEARCH_GEOS:BOOL=1 -DSEARCH_GIS_LIB_PATH:PATH=%PARAMGIS% -DSEARCH_GEOS_LIB_PATH:PATH=%PARAMGEOS% -DUSE_MT_LIBRARY:BOOL=1 -DUSE_VERSION:BOOL=1 -DwxWIDGETS_USING_SVN:BOOL=1 -DwxWIDGETS_PATH_SVN=%PARAMWXWIN%
ECHO 2) Making Visual studio solution... DONE



ECHO -----------------------------------------------
ECHO 3) BUILDING VROOMLOADER APP (MAY TAKE SOME TIMES)-----
ECHO -----------------------------------------------

cd %BINDIR%
"%vsdir%\VCExpress.exe" vroomLoader.sln /Out solution.log /Build Debug
echo %ERRORLEVEL%
IF ERRORLEVEL 1 goto QuitErrorBuildScript

"%vsdir%\VCExpress.exe" vroomLoader.sln /Out solution.log /Build Release
echo %ERRORLEVEL%
IF ERRORLEVEL 1 goto QuitErrorBuildScript

ECHO 3) BUILDING VROOMLOADER APP DONE


cd %bindir%
start %bindir%\Debug\vroomLoader.exe
REM "C:\Program Files\Notepad++\notepad++.exe" %bindir%\Testing\Temporary\LastTest.log
REM notepad.exe %bindir%\Testing\Temporary\LastTest.log
REM goto :WaitForEnter





ECHO ----------------------------------------
ECHO         UPDATE VROOMGIS APP
ECHO 			VROOMTWIN
ECHO     (c) Lucien Schreiber CREALP
ECHO ----------------------------------------

SET WXWIN=%PARAMWXWIN%
echo %WXWIN%


ECHO 2) Making Visual studio solution...
cd %bindir2%
cmake %trunkdir%\app\vroomtwin\build -G "Visual Studio 9 2008" -DCXXTEST_DIRECTORY:PATH=%PARAMCXX% -DSEARCH_GDAL:BOOL=1 -DSEARCH_GEOS:BOOL=1 -DSEARCH_GIS_LIB_PATH:PATH=%PARAMGIS% -DSEARCH_GEOS_LIB_PATH:PATH=%PARAMGEOS% -DUSE_MT_LIBRARY:BOOL=1 -DUSE_VERSION:BOOL=1 -DwxWIDGETS_USING_SVN:BOOL=1 -DwxWIDGETS_PATH_SVN=%PARAMWXWIN%
ECHO 2) Making Visual studio solution... DONE



ECHO -----------------------------------------------
ECHO 3) BUILDING VROOMTWIN APP (MAY TAKE SOME TIMES)-----
ECHO -----------------------------------------------

cd %BINDIR2%
"%vsdir%\VCExpress.exe" vroomTwin.sln /Out solution.log /Build Debug
echo %ERRORLEVEL%
IF ERRORLEVEL 1 goto QuitErrorBuildScript

"%vsdir%\VCExpress.exe" vroomTwin.sln /Out solution.log /Build Release
echo %ERRORLEVEL%
IF ERRORLEVEL 1 goto QuitErrorBuildScript

ECHO 3) BUILDING VROOMLOADER APP DONE


cd %bindir%
start %bindir2%\Debug\vroomTwin.exe
REM "C:\Program Files\Notepad++\notepad++.exe" %bindir%\Testing\Temporary\LastTest.log
REM notepad.exe %bindir%\Testing\Temporary\LastTest.log
goto :WaitForEnter



:WaitForEnter
  set wait=
  set /p wait=Please press ENTER to continue 
  IF NOT DEFINED wait goto :eof 
  echo Please press only the ENTER key and nothing else
  goto :WaitForEnter

REM :UserPressedENTER

:QuitErrorBuildScript
	"C:\Program Files\Notepad++\notepad++.exe" %bindir%\solution.log
	ECHO Error detected see solution.log
	goto :WaitForEnter