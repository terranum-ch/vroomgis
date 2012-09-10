# VARIABLES FOR BUILDING ON Linux (Home) 

gwxWidgets = "/home/lucien/programmation/LIB/_LIBWX/bin/wx-config" 
gwxWidgetsVersion = "/home/lucien/programmation/LIB/wxWidgets-SVN"
gDirTrunk = "/home/lucien/programmation/ColtopGIS/vroomgis-trunk"

gDirBin = "/home/lucien/programmation/ColtopGIS/bin/vroomgis"
gDirGis = "/home/lucien/programmation/LIB/_LIBGIS"
gDirTestData = "/home/lucien/programmation/ColtopGIS/test_data"
gDirCxx = "/home/lucien/programmation/LIB/cxxtest"
gDirSQLite = ""
gDirCurl = ""

# CMAKE SPECIFIC
gCmakeEnv = "CodeBlocks - Unix Makefiles" #result of cmake --help
gCmakeSpecific = []
gCmakeSpecific.append("-DCMAKE_WXWINDOWS_WXCONFIG_EXECUTABLE:FILE=" + gwxWidgets)
# comment line bellow to build debug version (default is debug)
gCmakeSpecific.append("-DCMAKE_BUILD_TYPE:STRING=Release")

# PLATEFORM SPECIFIC FUNCTION
# CONFIG SPECIFIC
def gBeforeConfig():
    "Initing plateform specific variables"
    print ("Nothing to init")

# BUILD SPECIFIC
def gBuildCommand(buildtype="Debug", directory = ""):
    myCommand = ['make']
    myCommand.append("--jobs=" + str(NumberOfProc))    
    return myCommand

