# VARIABLES FOR BUILDING ON OSX 

# Variables to define, see update_vroomgis.py:
gwxWidgets = "D:\\LIB\\wxWIDGETS-SVN" #wxWIDGETS config
gwxWidgetsVersion = "D:\\LIB\\wxWIDGETS-SVN"
gDirTrunk = "D:\\PRJ\\COLTOPGIS\\trunk-vroomgis"
gDirBin = "D:\\PRJ\\COLTOPGIS\\bin\\vroomgis"
gDirGis = "D:\\LIB\\LIB_GDAL"
gDirTestData = "D:\\PRJ\\COLTOPGIS\\test-data"
gDirCxx = "D:\\LIB\\cxxtest"
gDirSQLite = "D:\\LIB\\LIB_SQLITE"


# CMAKE SPECIFIC
gCmakeEnv = "Visual Studio 10"
gCmakeSpecific = ['-DUSE_MT_LIBRARY:BOOL=1']
gCmakeSpecific.append("-DSEARCH_GEOS_LIB_PATH:PATH=D:\\LIB\\geos-3.3.0")


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
