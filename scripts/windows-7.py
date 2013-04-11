# VARIABLES FOR BUILDING ON WINDOWS 

# Variables to define, see update_vroomgis.py:
gwxWidgets = "D:\\LIB\\wxWIDGETS-SVN" #wxWIDGETS config
gwxWidgetsVersion = "D:\\LIB\\wxWIDGETS-SVN"
gDirTrunk = "D:\\PRJ\\COLTOPGIS\\trunk-vroomgis"
gDirBin = "D:\\PRJ\\COLTOPGIS\\bin\\vroomgis"
gDirGis = "D:\\LIB\\LIB_GDAL"
gDirTestData = "D:\\PRJ\\COLTOPGIS\\test-data"
gDirCxx = "D:\\LIB\\cxxtest"
gDirSQLite = "D:\\LIB\\LIB_SQLITE"
gDirCurl = "D:\\LIB\\LIB_CURL"
gDirGeos = "D:\\LIB\\geos-3.3.7"

# CMAKE SPECIFIC
gCmakeEnv = "Visual Studio 10"
gCmakeSpecific = ['-DUSE_MT_LIBRARY:BOOL=1']
gCmakeSpecific.append("-DSEARCH_GEOS_LIB_PATH:PATH=" + gDirGeos)


# PLATEFORM SPECIFIC FUNCTION
#CONFIG SPECIFIC
def gBeforeConfig():
	"Initing plateform specific variables"
	print (os.environ['WXWIN'])
	try:
		os.environ['WXWIN']=gwxWidgets
		print (os.environ['WXWIN'])
	except:
		print("Error running pre-configure commands :", gwxWidgets)
		exit()


# BUILD SPECIFIC
def gBuildCommand(buildtype="Debug", directory = ""):
	myVarsFile = os.listdir(directory)
	myTarget = ""
	for file in myVarsFile:
		if (file.find(".sln") > 0):
			myTarget = file
			break
	
	myConfiguration = "/property:Configuration="+ buildtype
	return ['msbuild', myTarget, myConfiguration]



def DoPostBuildCommand(binpath):
	print ("BinPath", binpath)
	myDll = (os.path.join(gDirGis,"bin","gdal19.dll"), #,os.path.join(gDirGeos,"bin","Debug", "geos.dll"),
			 os.path.join(gDirGeos,"src","geos_c.dll"),
			 os.path.join(gDirCurl, "lib", "DLL-Debug","libcurld.dll"),
			 os.path.join(gDirCurl, "lib", "DLL-Release","libcurl.dll"),
			 os.path.join(gDirSQLite, "lib", "sqlite3.dll")
		)

	for dll in myDll:
		shutil.copyfile(dll, os.path.join(binpath, "Debug", os.path.basename(dll)))
		if (os.path.exists(os.path.join(binpath, "RelWithDebInfo"))):
			shutil.copyfile(dll, os.path.join(binpath, "RelWithDebInfo", os.path.basename(dll)))

		print ("copying: ", dll)
	return True

