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



if __name__ == '__main__':
	ConfigureGDAL("/Users/lucien/Desktop/test")

