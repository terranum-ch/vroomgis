#!/usr/bin/env python
# encoding: utf-8
"""
build-coltop-GDAL.py

Created by Lucien Schreiber on 2012-02-01.
Copyright (c) 2012 . All rights reserved.
"""

import sys
import os
import shutil
import lsutilities


def ConfigureGDAL(gdalpath):
	"""Copy needed files to specified GDAL tree"""
	localpaths = os.path.split(__file__)
	localpath = os.path.normpath(localpaths[0] + os.sep + ".." + os.sep)	
	
	# copy files and directory to frmts
	shutil.copytree(localpath + os.sep + "frmts" + os.sep + "c2d", gdalpath + os.sep + "frmts" + os.sep + "c2d")
	shutil.copy(localpath + os.sep + "frmts" + os.sep + "gdalallregister.cpp", gdalpath + os.sep + "frmts" + os.sep)	
	shutil.copy(localpath + os.sep + "frmts" + os.sep + "makefile.vc", gdalpath + os.sep + "frmts" + os.sep)
	
	#copy file to gcore
	shutil.copy(localpath + os.sep + "gcore" + os.sep + "gdal_frmts.h", gdalpath + os.sep + "gcore" + os.sep)	
	
	#copy file to gdal root
	shutil.copy(localpath + os.sep + "GDALmake.opt.in", gdalpath + os.sep)
	print ("C2D Files copied!")
	pass



def buildUnix(gdalpath):
	"""docstring for buildUnix"""
	doSysLib = lsutilities.askUserWithCheck("Use sytem libs (s), framework (f) or user defined path (p) for GEOS and PROJ ? ", ["s", "S", "f", "F", "p", "P"])
	libprefix = input ("GDAL installation directory : ")
	geos_path = ""
	proj_path = ""
	
	# BUILD COMMAND
	if (doSysLib == 'S'): #SYSTEM
		geos_path = "/usr/bin"
		proj_path = "/usr"
	elif (doSysLib == 'F'): #FRAMEWORK 
		geos_path = libprefix + os.sep + "GEOS.framework"
		proj_path = libprefix + os.sep + "PROJ.framework"
	else: #USER DEFINED
		geos_path = libprefix + os.sep + "bin"	
		proj_path = libprefix
	
	command = ""
	if (lsutilities.askUserWithCheck("Support Snow Leopard ? [Y/N] :") == 'Y'):
		cflags="-02 -isysroot /Users/lucien/DATA/PROGRAMATION/_LIB/SDKs/MacOSX10.6.sdk"
		lflags="-isysroot /Users/lucien/DATA/PROGRAMATION/_LIB/SDKs/MacOSX10.6.sdk"
		command = "./configure CFLAGS=\"{}\" CXXFLAGS=\"{}\" LDFLAGS=\"{}\" --prefix={} --with-geos={} --with-python=no --with-sqlite3=yes --with-static-proj4={} --with-pg=no --with-grass=no --with-jasper=no --with-jpeg=internal --with-png=internal"
		command = command.format(cflags, cflags, lflags, libprefix, geos_path+os.sep+"geos-config", proj_path)
	else:
		command = "./configure --prefix={} --with-geos={} --with-python=no --with-sqlite3=yes --with-static-proj4={} --with-pg=no --with-grass=no --with-jasper=no --with-jpeg=internal --with-png=internal"
		command = command.format(libprefix, geos_path+os.sep+"geos-config", proj_path)
	curlpath = input ("CURL path (leave empty to use system lib)  : ")
	if(curlpath != ""):
		command = command + " --with-curl=" + curlpath + os.sep + "bin" + os.sep + "curl-config"
	
	
	print (command)
	isSucess = lsutilities.runProcess(command, gdalpath, "Configuring GDAL with C2D support")
	if (isSucess == True):
		if (lsutilities.askUserWithCheck("Build GDAL now ?: ") == 'Y'):
			lsutilities.runProcess("make --jobs={}; make install".format(lsutilities.CountProcessor()), gdalpath, "Building GDAL with C2D support")
	
	pass

	
def buildWindows(gdalpath):
	if(lsutilities.askUserWithCheck("Is nmake.opt configured ? [Y / N]: ")=="N"):
		exit()
	if(lsutilities.runProcess("nmake -f makefile.vc", gdalpath, "Building GDAL")==True):
		lsutilities.runProcess("nmake -f makefile.vc install", gdalpath, "Installing GDAL")
		lsutilities.runProcess("nmake -f makefile.vc devinstall", gdalpath, "Dev-Installing GDAL")
	pass

	


if __name__ == '__main__':
	print ("-----------------------------------------")
	print ("CONFIGURE AND BUILD GDAL WITH C2D SUPPORT")
	print ("-----------------------------------------")
	
	gdalpath = input ("GDAL path : ")
	if (lsutilities.askUserWithCheck("Add C2D support ? [Y / N] ") == 'Y'):
		ConfigureGDAL(gdalpath)
	
	# PLATEFORM DETECTION
	if("nt" in os.name): #WINDOWS
		print ("Windows System")
		buildWindows(gdalpath)
	else:
		print ("Unix system detected!")
		buildUnix(gdalpath)
	
	

