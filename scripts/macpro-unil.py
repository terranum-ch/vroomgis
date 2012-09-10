# VARIABLES FOR BUILDING ON OSX 

# Variables to define, see update_vroomgis.py:
gwxWidgets = "/Users/lucien/Documents/PROGRAMMATION/64/_LIBWXSVN/bin/wx-config" #wxWIDGETS config
gwxWidgetsVersion = "/Users/lucien/Documents/PROGRAMMATION/64/wxWidgets-svn"
gDirTrunk = "/Users/lucien/Documents/PRJ/COLTOPGIS/trunk-vroomgis"
gDirBin = "/Users/lucien/Documents/PRJ/COLTOPGIS/bin/vroomgis"
gDirGis = "/Users/lucien/Documents/PROGRAMMATION/64/_LIBGIS"
gDirTestData = "/Users/lucien/Documents/PRJ/COLTOPGIS/test_data"
gDirCxx = "/Users/lucien/Documents/PROGRAMMATION/cxxtest"
gDirSQLite = ""
gDirCurl = ""


# CMAKE SPECIFIC
gCmakeEnv = "Xcode" #result of cmake --help
gCmakeSpecific = ['-DCMAKE_OSX_ARCHITECTURES:TEXT=x86_64']
gCmakeSpecific.append("-DCMAKE_WXWINDOWS_WXCONFIG_EXECUTABLE:FILE=" + gwxWidgets)


# PLATEFORM SPECIFIC FUNCTION
#CONFIG SPECIFIC
def gBeforeConfig():
	print ("No special configurations")


# BUILD SPECIFIC
def gBuildCommand(buildtype="Debug", directory = ""):
	if (buildtype == "Release"):
		return ['xcodebuild', '-configuration', 'Release']
	return ['xcodebuild', '-configuration', 'Debug']
	


	
