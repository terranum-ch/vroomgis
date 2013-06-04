# BUILD SCRIPT FOR CREATING VROOMGIS BINARY ON EVERY PLATEFORM
# Python IS REQUIRED
# (c) Lucien Schreiber 2011

# Variables to define:
# gwxWidgets = wxconfig binary
# gwxWidgetsVersion = trunk of wxwidgets when using SVN version
# gDirTrunk = Path to the vroomgis trunk
# gDirBin = Path to the vroomgis binary dir (will contain : vroomloader, vroomtwin, vroomtests)
# 
# gDirGis = Path to the GIS directory (GDAL, GEOS, PROJ), may be empty if installed in standard directory
# gDirTestData = Path to the test data, only needed for vroomgistests
# gDirCxx = Path to the cxxtest library
# gDirSQLite = Path to the sqlite library, may be empty if using standard library.


import os.path
import shutil
import subprocess

# script info
print ("\n----------------------------------------------------------")
print ("BUILD SCRIPT FOR CREATING VROOMGIS BINARY ON EVERY PLATEFORM")
print ("----------------------------------------------------------\n")

#
# FUNCTIONS
#
def askUserWithCheck (msg, allowedval=['Y', 'y', 'N', 'n'], errormsg = "Unsupported value"):
  "Ask user for entering a value with validy check, looping until correct value is entered"
  while(1):
    myValue = input(msg)
    if (myValue.isdigit()):
      myValue = int(myValue)
    if (myValue in allowedval):
      if(type(myValue) != type(int())):
        return myValue.upper()
      return myValue
    print (errormsg)



def buildCommand (name, version):
  print ("Building {} -- {} ".format(name, version))
  myProcess = subprocess.Popen(gBuildCommand(version, os.path.join(gDirBin, name)), cwd=os.path.join(gDirBin, name), shell=False)
  myRetcode = myProcess.wait()
  if(myRetcode != 0):
    print ("Building {} -- {} failed, Error code is: ".format(name, version), myRetcode)
    exit()
  print ("Building {} -- {} DONE".format(name, version))
  print ("----------------------------------------------------------\n")


def createEmptyDirs(bindir):
  """Creating vroomtwin, vroomloader, vroomgistests if not existing"""
  try:
    os.makedirs(bindir)
  except:
    print ("Error creating '{}', dir exists!".format(bindir))
  
  try:
    os.makedirs(bindir + os.sep + "vroomloader")
  except:
    print ("Error creating '{}', dir exists!".format(bindir + os.sep + "vroomloader"))
  
  try:
    os.makedirs(bindir + os.sep + "vroomtwin")
  except:
    print ("Error creating '{}', dir exists!".format(bindir + os.sep + "vroomtwin"))
  
  try:
    os.makedirs(bindir + os.sep + "vroomgistests")
  except:
    print ("Error creating '{}', dir exists!".format(bindir + os.sep + "vroomgistests"))
  
  try:
    os.makedirs(bindir + os.sep + "vroomdrawer")
  except:
    print ("Error creating '{}', dir exists!".format(bindir + os.sep + "vroomdrawer"))


   
def runTests (msg, directory):
  "Run ctest and display error log if an error occur"
  print ("Running " + msg + " TEST in " + directory)
  myRetcode = 0
  try:
    myProcess = subprocess.Popen(['ctest'], 0, None, None, None,  None, None, False, False, directory)
    myRetcode = myProcess.wait()
  except:
    print ("Running "+ msg +" TEST  FAILED")
    return False
  if (myRetcode != 0):
    print ("Running "+ msg +" TEST  FAILED")
    print ("----------------------------------------------------------\n")
    myErrorFile = directory + os.sep + "Testing" + os.sep + "Temporary" + os.sep + "LastTest.log"
    try:
      readErrorFile = open(myErrorFile, 'r')
    except:
      print("Error reading log : " + myErrorFile)
      return False
    icount = 1
    for line in readErrorFile:
      print (str(icount) + " " + line, end='')
      icount = icount + 1
    print (readErrorFile.readlines())
    return False
  
  print ("Running "+ msg +" TEST DONE")
  #print ("retcode: " + str(myRetcode))
  print ("----------------------------------------------------------\n")
  return True

def CountProcessor():
  "Return the number of processor"
  import multiprocessing
  myNumberofProc = multiprocessing.cpu_count()
  return myNumberofProc
  

global NumberOfProc
NumberOfProc = CountProcessor()


#
# load variables possible variable files
#
myScriptPathName = os.path.split(__file__)
myVarsFile = os.listdir(myScriptPathName[0])

# copy the list before iterating... otherwise side effects.
myIterateList = myVarsFile[:]
for myFile in myIterateList:
  if (myFile.find(".py") <= 0 or myFile == myScriptPathName[1]):
    myVarsFile.remove (myFile)
icount = 1
for myFile in myVarsFile:
  print ("(",icount, ") -- ", myFile[:-3])
  icount = icount + 1

if (len(myVarsFile) == 0):
  print("no variable found, exiting")
  exit()
  
myvarAllowedValues = list(range(1,len(myVarsFile) + 2))
varChoose = askUserWithCheck("Select variable file number to use for build: ", myvarAllowedValues)
mySelectedVar = myVarsFile[varChoose-1]


try:
  exec(open(myScriptPathName[0] + os.sep + mySelectedVar).read())
except:
  print ("Using variables from ", mySelectedVar, "isn't possible!")
  print (myScriptPathName[0] + os.sep + mySelectedVar)
  exit(); 

#
# generic questions
#
doClean = askUserWithCheck("Clean all before building ? (Y / N): ").upper()
doBuildVroomloader = askUserWithCheck("Build Vroomloader ? (Y / N): ").upper()
doBuildVroomTwin = askUserWithCheck("Build VroomTwin ? (Y / N): ").upper()
doBuildVroomDrawer = askUserWithCheck("Build VroomDrawer ? (Y / N): ").upper()
doBuildVroomgistests = askUserWithCheck("Build Vroomgistests ? (Y / N): ").upper()
doRunTests = askUserWithCheck("Run tests after build ? (Y/N): ").upper()
doBuildRelease = askUserWithCheck("Build release ? (Y/N): ").upper()
print ("----------------------------------------------------------\n")

#
# display variables
#
print ("Using variables from :", mySelectedVar)
print ("wxWidgets :", gwxWidgets)
print ("Bin     :", gDirBin)
print ("Trunk   :", gDirTrunk)
print ("GIS     :", gDirGis)
print ("Unit test :", gDirTestData)
print ("CXX     :", gDirCxx)
print ("----------------------------------------------------------\n")
print ("Using ", NumberOfProc, " Processor(s)")
print ("----------------------------------------------------------\n")

doGo = askUserWithCheck ("Do the build ? (Y / N): ")


#
# get latest version
#
print ("Updating with latest version from Mercurial")
try:
  myProcess = subprocess.Popen("hg pull", shell=True, cwd=gDirTrunk) 
  myProcess.wait()
except:
  print ("Error pulling from Mercurial!", )
  exit()
try:
  myProcess = subprocess.Popen("hg update", shell=True, cwd=gDirTrunk)
  myProcess.wait()
except:
  print("Error updating from Mercurial!")
  exit()
print ("Updating DONE!")
print ("----------------------------------------------------------\n")


createEmptyDirs(gDirBin)

#
# Clean directory
#

if (doClean == 'Y' and gDirBin != ''):
  print ("Cleaning directory")
  try:
    shutil.rmtree(gDirBin)
    createEmptyDirs(gDirBin)
    print ("Cleaning directory DONE")
    print ("----------------------------------------------------------\n")
  except:
    print("Unable to remove directory :", gDirBin)

#
# before configure
#
gBeforeConfig()


#
# Configure vroomloader using CMAKE
#
if (doBuildVroomloader == 'Y'):
  mycmakeCommandLine = ['cmake']
  mycmakeCommandLine.append("-G" + gCmakeEnv)
  mycmakeCommandLine.append(gDirTrunk + os.sep + "app" + os.sep + "vroomloader" + os.sep + "build")
  mycmakeCommandLine = mycmakeCommandLine + gCmakeSpecific
  mycmakeCommandLine.append("-DSEARCH_GDAL:BOOL=1")
  mycmakeCommandLine.append("-DSEARCH_GEOS:BOOL=1")
  mycmakeCommandLine.append("-DSEARCH_VROOMGIS_LIBS:BOOL=1")
  mycmakeCommandLine.append("-DSEARCH_GIS_LIB_PATH:PATH=" + gDirGis)
  if(gwxWidgetsVersion != ""):
    mycmakeCommandLine.append("-DwxWIDGETS_USING_SVN:BOOL=1")
    mycmakeCommandLine.append("-DwxWIDGETS_PATH_SVN:PATH=" + gwxWidgetsVersion)
  
  if (gDirSQLite != ''):
    mycmakeCommandLine.append("-DSQLITE_PATH:PATH=" + gDirSQLite)
  print (" ".join(mycmakeCommandLine))
  try:
    myProcess = subprocess.Popen(mycmakeCommandLine, 0, None, None, None,  None, None, False, False, gDirBin + os.sep + "vroomloader")
    myProcess.wait()
    if (doClean == 'Y'):
      print ("\n****Configuring again to be sure after a clean****")
      myProcess2 = subprocess.Popen(mycmakeCommandLine, 0, None, None, None,  None, None, False, False, gDirBin + os.sep + "vroomloader")
      myProcess2.wait()
  except:
    print("Configuring VROOMLOADER project FAILED!")
    exit()
 
  print ("Configuration VROOMLOADER DONE")
  print ("----------------------------------------------------------\n")


#
# Configure vroomtwin using CMAKE
#
if (doBuildVroomTwin == 'Y'):
  mycmakeCommandLine = ['cmake']
  mycmakeCommandLine.append("-G" + gCmakeEnv)
  mycmakeCommandLine.append(gDirTrunk + os.sep + "app" + os.sep + "vroomtwin" + os.sep + "build")
  mycmakeCommandLine = mycmakeCommandLine + gCmakeSpecific
  mycmakeCommandLine.append("-DSEARCH_GDAL:BOOL=1")
  mycmakeCommandLine.append("-DSEARCH_GEOS:BOOL=1")
  mycmakeCommandLine.append("-DSEARCH_VROOMGIS_LIBS:BOOL=1")
  mycmakeCommandLine.append("-DSEARCH_GIS_LIB_PATH:PATH=" + gDirGis)
  if(gwxWidgetsVersion != ""):
    mycmakeCommandLine.append("-DwxWIDGETS_USING_SVN:BOOL=1")
    mycmakeCommandLine.append("-DwxWIDGETS_PATH_SVN:PATH=" + gwxWidgetsVersion)

  if (gDirSQLite != ''):
    mycmakeCommandLine.append("-DSQLITE_PATH:PATH=" + gDirSQLite)
  print (" ".join(mycmakeCommandLine))
  try:
    myProcess = subprocess.Popen(mycmakeCommandLine, 0, None, None, None,  None, None, False, False, gDirBin + os.sep + "vroomtwin")
    myProcess.wait()
    if (doClean == 'Y'):
      print ("\n****Configuring again to be sure after a clean****")
      myProcess2 = subprocess.Popen(mycmakeCommandLine, 0, None, None, None,  None, None, False, False, gDirBin + os.sep + "vroomtwin")
      myProcess2.wait()
  except:
    print("Configuring VROOMTWIN project FAILED!")
    exit()
  print ("Configuration VROOMTWIN DONE")
  print ("----------------------------------------------------------\n")



#
# Configure vroomDrawer using CMAKE
#
if (doBuildVroomDrawer == 'Y'):
  mycmakeCommandLine = ['cmake']
  mycmakeCommandLine.append("-G" + gCmakeEnv)
  mycmakeCommandLine.append(gDirTrunk + os.sep + "app" + os.sep + "vroomdrawer" + os.sep + "build")
  mycmakeCommandLine = mycmakeCommandLine + gCmakeSpecific
  mycmakeCommandLine.append("-DSEARCH_GDAL:BOOL=1")
  mycmakeCommandLine.append("-DSEARCH_GEOS:BOOL=1")
  mycmakeCommandLine.append("-DSEARCH_VROOMGIS_LIBS:BOOL=1")
  mycmakeCommandLine.append("-DSEARCH_GIS_LIB_PATH:PATH=" + gDirGis)
  mycmakeCommandLine.append("-DSEARCH_CURL_LIB_PATH:PATH=" + gDirCurl)
  if(gwxWidgetsVersion != ""):
    mycmakeCommandLine.append("-DwxWIDGETS_USING_SVN:BOOL=1")
    mycmakeCommandLine.append("-DwxWIDGETS_PATH_SVN:PATH=" + gwxWidgetsVersion)

  if (gDirSQLite != ''):
    mycmakeCommandLine.append("-DSQLITE_PATH:PATH=" + gDirSQLite)
  print (" ".join(mycmakeCommandLine))
  try:
    myProcess = subprocess.Popen(mycmakeCommandLine, 0, None, None, None,  None, None, False, False, gDirBin + os.sep + "vroomdrawer")
    myProcess.wait()
    if (doClean == 'Y'):
      print ("\n****Configuring again to be sure after a clean****")
      myProcess2 = subprocess.Popen(mycmakeCommandLine, 0, None, None, None,  None, None, False, False, gDirBin + os.sep + "vroomdrawer")
      myProcess2.wait()
  except:
    print("Configuring VROOMDRAWER project FAILED!")
    exit()
  print ("Configuration VROOMDRAWER DONE")
  print ("----------------------------------------------------------\n")


#
# Configure vroomgistests using CMAKE
#
if (doBuildVroomgistests == 'Y'):
  if (not gDirTestData):
    print("gDirTestData: Is not specified!")
    exit()
  mycmakeCommandLine = ['cmake']
  mycmakeCommandLine.append("-G" + gCmakeEnv)
  mycmakeCommandLine.append(gDirTrunk + os.sep + "test" + os.sep + "build")
  mycmakeCommandLine = mycmakeCommandLine + gCmakeSpecific
  mycmakeCommandLine.append("-DSEARCH_GDAL:BOOL=1")
  mycmakeCommandLine.append("-DSEARCH_GEOS:BOOL=1")
  mycmakeCommandLine.append("-DSEARCH_GIS_LIB_PATH:PATH=" + gDirGis)
  mycmakeCommandLine.append("-DCXXTEST_DIRECTORY:PATH=" + gDirCxx)
  mycmakeCommandLine.append("-DUNIT_TESTING_PATH:PATH=" + gDirTestData)

  if (gDirSQLite != ''):
    mycmakeCommandLine.append("-DSQLITE_PATH:PATH=" + gDirSQLite)
  print (" ".join(mycmakeCommandLine))
  try:
    myProcess = subprocess.Popen(mycmakeCommandLine, 0, None, None, None,  None, None, False, False, gDirBin + os.sep + "vroomgistests")
    myProcess.wait()
    if (doClean == 'Y'):
      print ("\n****Configuring again to be sure after a clean****")
      myProcess2 = subprocess.Popen(mycmakeCommandLine, 0, None, None, None,  None, None, False, False, gDirBin + os.sep + "vroomgistests")
      myProcess2.wait()
  except:
    print("Configuring VROOMGISTESTS project FAILED!")
    exit()
  print ("Configuration VROOMGISTESTS DONE")
  print ("----------------------------------------------------------\n")



if (doGo == 'N'):
  exit()


#
# BUILDING VROOMLOADER, VROOMTWIN, VROOMGISTESTS
#
if (doBuildVroomloader == 'Y'):
  buildCommand("vroomloader", "Debug")
  if (doBuildRelease == 'Y'):
    buildCommand("vroomloader", "RelWithDebInfo")
  try:
    DoPostBuildCommand(os.path.join(gDirBin, "vroomloader"))
  except:
    print ("Nothing to do after build!")


if (doBuildVroomTwin == 'Y'):
  buildCommand("vroomtwin", "Debug")
  if (doBuildRelease == 'Y'):
    buildCommand("vroomtwin", "RelWithDebInfo")
  try:
    DoPostBuildCommand(os.path.join(gDirBin, "vroomtwin"))
  except:
    print ("Nothing to do after build!")


if (doBuildVroomDrawer == 'Y'):
  buildCommand("vroomdrawer", "Debug")
  if (doBuildRelease == 'Y'):
    buildCommand("vroomdrawer", "RelWithDebInfo")
  try:
    DoPostBuildCommand(os.path.join(gDirBin, "vroomdrawer"))
  except:
    print ("Nothing to do after build")


if (doBuildVroomgistests == 'Y'):
  print ("Building VroomGISTests DEBUG ")
  myProcess = subprocess.Popen(gBuildCommand("Debug",gDirBin + os.sep + "vroomgistests"), 0, None, None, None,  None, None, False, False, gDirBin + os.sep + "vroomgistests")
  myRetcode = myProcess.wait()
  if(myRetcode != 0):
    print ("Building VroomGISTests failed, Error code is: ", myRetcode)
    exit()
  print ("Building VroomGISTests DONE")
  print ("----------------------------------------------------------\n")


#
# running tests
#
if(doRunTests == "Y"):
  myDirectory = gDirBin + os.sep + "vroomgistests"
  if (not runTests("vroomgis", myDirectory)):
    exit()


if(askUserWithCheck("Build Finished, exit ? (Y / N): ").upper() == 'Y'):
  exit()
    

    
  
  









