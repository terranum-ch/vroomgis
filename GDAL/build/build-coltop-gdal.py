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
	command = "./configure --prefix={} --with-geos={} --with-python=no --with-sqlite3=yes --with-static-proj4={} --with-pg=no"
	
	if (doSysLib == 'S'): #SYSTEM
		geos_path = "/usr/bin"
		proj_path = "/usr"
	elif (doSysLib == 'F'): #FRAMEWORK 
		geos_path = libprefix + os.sep + "GEOS.framework"
		proj_path = libprefix + os.sep + "PROJ.framework"
	else: #USER DEFINED
		geos_path = libprefix + os.sep + "bin"	
		proj_path = libprefix
	
	command = command.format(libprefix, geos_path+os.sep+"geos-config", proj_path)
	print (command)
	isSucess = lsutilities.runProcess(command, gdalpath, "Configuring GDAL with C2D support")
	if (isSucess == True):
		if (lsutilities.askUserWithCheck("Build GDAL now ?: ") == 'Y'):
			lsutilities.runProcess("make --jobs={}; make install".format(lsutilities.CountProcessor()), gdalpath, "Building GDAL with C2D support")
	
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
	else:
		print ("Unix system detected!")
		buildUnix(gdalpath)
	
	

