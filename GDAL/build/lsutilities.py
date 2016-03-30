#!/usr/bin/env python
# encoding: utf-8
"""
lsutilities.py

Created by Lucien Schreiber on 2011-04-13.
Copyright (c) 2011 CREALP. All rights reserved.
"""

import sys
import os
import shutil
import subprocess


def askUserWithCheck(msg, allowedval=['Y', 'y', 'N', 'n'], errormsg="Unsupported value"):
    "Ask user for entering a value with validy check, looping until correct value is entered"
    while (1):
        myValue = input(msg)
        if (myValue.isdigit()):
            myValue = int(myValue)
        if (myValue in allowedval):
            if (type(myValue) != type(int())):
                return myValue.upper()
            return myValue
        print (errormsg)


def createEmptyDirs(bindir):
    """Creating dir if not exists"""
    if not os.path.exists(bindir):
        os.makedirs(bindir)


def CountProcessor():
    "Return the number of processor"
    import multiprocessing
    myNumberofProc = multiprocessing.cpu_count()
    return myNumberofProc


def runProcess(command, directory, name, shell=True):
    """Running command in directory of specified name"""
    myProcess = subprocess.Popen(command, cwd=directory, shell=shell)
    if (myProcess.wait() != 0):
        print("{0} FAILED!".format(name))
        return False
    print ("{0} DONE!".format(name))
    print ("----------------------------------------------------------\n")
    return True
