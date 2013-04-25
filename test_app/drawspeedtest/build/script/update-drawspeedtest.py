#!/usr/bin/env python
# encoding: utf-8
"""
update-drawspeedtest.py

Created by Lucien Schreiber on 2013-04-11.
Copyright (c) 2013 . All rights reserved.
"""

import os
import shutil
try:
  from Tkinter import *
except :
  from tkinter import *
from subprocess import *
from ttk import *



def GetCmakeDirName():
  pathname = os.path.dirname(sys.argv[0])        
  return os.path.join(os.path.abspath(pathname), "..")



def updateSVN():
  myPath = os.path.normpath(os.path.join(GetCmakeDirName(), ".."))
  print ("Updating subversion in: {}".format(myPath))
  try:
    p = Popen("svn update", shell=True, cwd = myPath)
    p.wait()
  except:
    print ("Error updating subversion in: {}".format(myPath))

  # print version number
  try:
    p = Popen("svnversion", shell=True, cwd=myPath, stdout=PIPE, stderr=PIPE)
    information = p.communicate()
    myVersion = str(information[0])
    #myVersionAlone = myVersion[myVersion.find("'")+1:-3] 
    print ("Updated version is: {}".format(myVersion))
  except:
    print("Error getting version for {}".format(GetCmakeDirName()))



def createAndClearDirectory(directory):
  if (os.path.exists(directory)):
    try:
        shutil.rmtree(directory)
    except:
      print ("Removing directory failed!", directory)
  
  print ("creating directory:", directory)
  os.mkdir(directory)


    


def BuildMacPro():
  print ("Configuring Mac Pro (UNIL)")
  builddir = "/Users/lucien/Documents/PRJ/COLTOPGIS/bin/vroomgis/drawspeedtest"
  createAndClearDirectory(builddir)

  try:
    p = Popen("cmake -GXcode " + GetCmakeDirName() + "  -DCMAKE_OSX_ARCHITECTURES:TEXT=x86_64 -DCMAKE_OSX_DEPLOYMENT_TARGET:TEXT=10.6 -DCMAKE_OSX_SYSROOT:PATH=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk -DCMAKE_WXWINDOWS_WXCONFIG_EXECUTABLE:FILE=/Users/lucien/Documents/PROGRAMMATION/64/_LIBWXSVN/bin/wx-config", shell=True, cwd=builddir)
    p.wait()
  except:
    print("Error creating makefile")



def BuildMacBook():
  print ("Configuring MacBook")
  builddir = "/Users/lucien/DATA/PRJ/COLTOP-GIS/bin/vroomgis/drawspeedtest"
  createAndClearDirectory(builddir)

  try:
    p = Popen("cmake -GXcode " + GetCmakeDirName() + "  -DCMAKE_OSX_ARCHITECTURES:TEXT=x86_64 -DCMAKE_OSX_DEPLOYMENT_TARGET:TEXT=10.6 -DCMAKE_OSX_SYSROOT:PATH=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk -DCMAKE_WXWINDOWS_WXCONFIG_EXECUTABLE:FILE=/Users/lucien/DATA/PROGRAMATION/_LIB/64/_LIBWXSVN/bin/wx-config ", shell=True, cwd=builddir)
  except:
    print("Error creating makefile")



def BuildLinux():
  print("Configuring Linux")
  builddir="/home/lucien/programmation/ColtopGIS/bin/vroomgis/drawspeedtest"
  try:
    p = Popen("cmake -G'CodeBlocks - Unix Makefiles' " + GetCmakeDirName() + " -DCMAKE_WXWINDOWS_WXCONFIG_EXECUTABLE:FILE=/home/lucien/programmation/LIB/_LIBWX/bin/wx-config", shell=True, cwd=builddir)
    p.wait()
  except:
    print("Error creating makefile")




def BuildWindows7():
  print ("Configuring Windows")
  builddir = "D:\\PRJ\\COLTOPGIS\\bin\\vroomgis\\drawspeedtest"
  createAndClearDirectory(builddir)

  try:
    p = Popen("cmake -G\"Visual Studio 10\" " + GetCmakeDirName() + " -DwxWIDGETS_USING_SVN:BOOL=1  -DwxWIDGETS_PATH_SVN:PATH=D:\\LIB\\wxWIDGETS-SVN -DwxWIDGETS_USING_SVN:BOOL=1 -DUSE_MT_LIBRARY:BOOL=1", shell=True, cwd=builddir)
    p.wait()
  except:
    print("Error creating makefile")



def RunBuildMac():
  builddir =  myPath = os.path.normpath(os.path.join(GetCmakeDirName(),"..", "..", "..", "..",  "bin", "vroomgis", "drawspeedtest" ))
  print (builddir)
  
  try:
    p = Popen("xcodebuild -configuration Debug", shell=True, cwd=builddir)
    p.wait()
  except:
    print ("Error building in", builddir)
    return

  try:
    p = Popen("xcodebuild -configuration Release", shell=True, cwd=builddir)
    p.wait()
  except:
    print ("Error building in", builddir)
    return

def RunBuildWindows(solutionname):
  builddir =  myPath = os.path.normpath(os.path.join(GetCmakeDirName(), "..", "..",  "bin"))
  print (builddir)
  mycommand = "msbuild {}.sln /p:Configuration={}" 
  
  try:
    p = Popen(mycommand.format(solutionname, "Debug"), shell=True, cwd=builddir)
    p.wait()
  except:
    print ("Error building in", builddir)
    return

  try:
    p = Popen(mycommand.format(solutionname, "RelWithDebInfo"), shell=True, cwd=builddir)
    p.wait()
  except:
    print ("Error building in", builddir)
    return  




if __name__ == '__main__':
  root = Tk()
  myContainer1 = Frame(root)
  myContainer1.pack()

  label = Label(myContainer1, text="drawSpeedTest")
  label.pack()

  button3 = Button(myContainer1, text=("Update subversion"), command=updateSVN)
  button3.pack()

  orient1 = Separator(myContainer1, orient='horizontal')
  orient1.pack(fill = BOTH, expand = True, pady=5, padx=5)

  button2 = Button(myContainer1, text="Configure MacBook", command=BuildMacBook)
  button2.pack()

  button5 = Button(myContainer1, text="Configure Linux", command=BuildLinux)
  button5.pack()
  
  button1 = Button(myContainer1, text="Configure Mac Pro (UNIL)", command=BuildMacPro)
  button1.pack()

  button3 = Button(myContainer1, text= "Configure Windows 7", command=BuildWindows7)
  button3.pack()
 
  orient = Separator(myContainer1, orient='horizontal')
  orient.pack(fill = BOTH, expand = True, pady=5, padx=5)
  
  button4 = Button(myContainer1, text= "Build Mac (bin)", command=RunBuildMac)
  button4.pack()

  solutionname = label.cget("text")
  button5 = Button(myContainer1, text= "Build Windows (bin)", command=lambda:RunBuildWindows(solutionname))
  button5.pack()
  
  root.mainloop()


