@ECHO OFF 
REM script for updating vroomGIS app under windows
REM works only for windows
REM Was updated for Windows 7 and msbuild (17. March 2011)
REM 1) Update the svn repositories
REM 2) Run cmake to update the Visual studio solution
REM 3) Run (manually) the vroomGIS compilation
REM 4) Launch the app


@SET TRUNKDIR=D:\PRJ\COLTOPGIS\trunk-vroomgis
@SET BINDIR=D:\PRJ\COLTOPGIS\bin\vroomloader
@SET BINDIR2=D:\PRJ\COLTOPGIS\bin\vroomtwin
@SET VCVARS="C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"


@SET PARAMCXX=D:/LIB/cxxtest
@SET PARAMGIS=D:/LIB/LIB_GDAL
@SET PARAMGEOS=D:/LIB/geos-3.2.2
@SET PARAMWXWIN=D:/LIB/wxWIDGETS-SVN
@SET PARAMSQLITE=D:/LIB/LIB_SQLITE


ECHO ----------------------------------------
ECHO         UPDATE VROOMGIS APP
ECHO 			VROOMLOADER
ECHO     (c) Lucien Schreiber CREALP
ECHO ----------------------------------------
ECHO 1) Updating repositories ...

REM RUN VCVARS SCRIPT
C:
start %VCVARS% 


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
@SET MYCMAKECMD1=%trunkdir%\app\vroomloader\build -G "Visual Studio 10" -DCXXTEST_DIRECTORY:PATH=%PARAMCXX% -DSEARCH_GDAL:BOOL=1 -DSEARCH_GEOS:BOOL=1 -DSEARCH_GIS_LIB_PATH:PATH=%PARAMGIS% -DSEARCH_GEOS_LIB_PATH:PATH=%PARAMGEOS% -DUSE_MT_LIBRARY:BOOL=1 -DUSE_VERSION:BOOL=1 -DwxWIDGETS_USING_SVN:BOOL=1 -DwxWIDGETS_PATH_SVN=%PARAMWXWIN% -DSQLITE_PATH:PATH=%PARAMSQLITE%
cmake %MYCMAKECMD1%
cmake %MYCMAKECMD1%
ECHO 2) Making Visual studio solution... DONE



ECHO -----------------------------------------------
ECHO 3) BUILDING VROOMLOADER APP (MAY TAKE SOME TIMES)-----
ECHO -----------------------------------------------

cd %BINDIR%
msbuild vroomLoader.sln /property:Configuration=Debug
echo %ERRORLEVEL%
IF ERRORLEVEL 1 goto QuitErrorBuildScript


msbuild vroomLoader.sln /property:Configuration=Release
echo %ERRORLEVEL%
IF ERRORLEVEL 1 goto QuitErrorBuildScript

ECHO 3) BUILDING VROOMLOADER APP DONE


cd %bindir%
start %bindir%\Debug\vroomLoader.exe




ECHO ----------------------------------------
ECHO         UPDATE VROOMGIS APP
ECHO 			VROOMTWIN
ECHO     (c) Lucien Schreiber CREALP
ECHO ----------------------------------------

SET WXWIN=%PARAMWXWIN%
echo %WXWIN%


ECHO 2) Making Visual studio solution...
cd %bindir2%
@SET MYCMAKECMD2=%trunkdir%\app\vroomtwin\build -G "Visual Studio 10" -DCXXTEST_DIRECTORY:PATH=%PARAMCXX% -DSEARCH_GDAL:BOOL=1 -DSEARCH_GEOS:BOOL=1 -DSEARCH_GIS_LIB_PATH:PATH=%PARAMGIS% -DSEARCH_GEOS_LIB_PATH:PATH=%PARAMGEOS% -DUSE_MT_LIBRARY:BOOL=1 -DUSE_VERSION:BOOL=1 -DwxWIDGETS_USING_SVN:BOOL=1 -DwxWIDGETS_PATH_SVN=%PARAMWXWIN% -DSQLITE_PATH:PATH=%PARAMSQLITE%
cmake %MYCMAKECMD2%
cmake %MYCMAKECMD2%
cmake 
ECHO 2) Making Visual studio solution... DONE



ECHO -----------------------------------------------
ECHO 3) BUILDING VROOMTWIN APP (MAY TAKE SOME TIMES)-----
ECHO -----------------------------------------------

cd %BINDIR2%
msbuild vroomTwin.sln /property:Configuration=Debug
echo %ERRORLEVEL%
IF ERRORLEVEL 1 goto QuitErrorBuildScript

msbuild vroomTwin.sln /property:Configuration=Release
echo %ERRORLEVEL%
IF ERRORLEVEL 1 goto QuitErrorBuildScript

ECHO 3) BUILDING VROOMLOADER APP DONE


cd %bindir%
start %bindir2%\Debug\vroomTwin.exe
goto :WaitForEnter



:WaitForEnter
  set wait=
  set /p wait=Please press ENTER to continue 
  IF NOT DEFINED wait goto :eof 
  echo Please press only the ENTER key and nothing else
  goto :WaitForEnter


:QuitErrorBuildScript
	"C:\Program Files\Notepad++\notepad++.exe" %bindir%\solution.log
	ECHO Error detected see solution.log
	goto :WaitForEnter