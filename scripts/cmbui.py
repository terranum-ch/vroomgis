#!/usr/bin/env python
import os
import subprocess
import shutil
import multiprocessing


class FileCache(object):
    """Manage the cache for variables when building projects"""
    def __init__(self, cachefilename="buildcache.txt"):
        self.m_cachefilename = os.path.join(os.path.dirname(os.path.abspath(__file__)), cachefilename)
        self.m_questions = []

    def _cacheQuestion(self, message, value):
        data = input("{} (cmake value : {}) ".format(message, value))
        return value + "=" + str(data)

    def _cacheWrite(self, text, hfile):
        """Only write to cache file if value isn't empty"""
        findindex = text.find("=")
        if (findindex == -1):
            return

        if findindex == len(text) -1:
            return

        hfile.write(text + "\n")

    def appendQuestion(self, message, storedvalue):
        self.m_questions.append((message, storedvalue))

    def createCache(self):
        with open(self.m_cachefilename, 'w') as cfile:
            for item in self.m_questions:
                self._cacheWrite(self._cacheQuestion(item[0], item[1]), cfile)

    def hasCacheFile(self, createifneeded=False):
        """check for the existance of the file cache and create one if needed"""
        if(os.path.exists(self.m_cachefilename) is True):
            return True

        if createifneeded is True:
            self.createCache()

    def readCacheLines(self):
        """get cache content as an array"""
        with open(self.m_cachefilename, 'r') as cfile:
            return cfile.readlines()

    def readCacheValue(self, value):
        """get the value of a variable stored in cache"""
        mylines = self.readCacheLines()
        for line in mylines:
            if value in line:
                return line[line.find("=")+1:-1]
        return ""


class BuildProject(object):
    """Class for building project using cmake"""
    def __init__(self, cmake_generator, cmake_params, cmake_dir="", binary_dir="bin"):
        self.cmake_generator = cmake_generator
        self.cmake_params = cmake_params
        self.binary_dir = binary_dir
        self.cmake_dir = cmake_dir

    def _checkParams(self):
        """private : Check class members"""
        if not self.binary_dir:
            print("Binary directory is None!")
            return False
        return True

    def createPath(self, path, erase_path=False):
        """Create paths if they didn't exists. If erase_path is True,
        path will be destroyed before creation."""
        if not os.path.exists(path):
            os.makedirs(path)
            return

        if erase_path is True:
            shutil.rmtree(path)
            self.createPath(path)

    def getBinaryDir(self):
        return self.binary_dir

    def getCmakeCommand(self, use_cmake_filepath=True):
        """Get The cmake command for configuring the project"""
        if self._checkParams() is False:
            return False

        generator = ""
        if self.cmake_generator != "":
            generator = " -G'" + self.cmake_generator + "' "

        cmake_filepath = ""
        if use_cmake_filepath is True:
            cmake_filepath = os.path.join(os.path.dirname(__file__), self.cmake_dir)

        cmakeparams = ""
        for param in self.cmake_params:
            cmakeparams = cmakeparams + " -D" + param[:-1]

        return "cmake " + cmake_filepath  + generator + cmakeparams

    def doConfigure(self):
        """Run cmake and configure the project"""
        if self._checkParams() is False:
            return False

        try:
            myProcess = subprocess.Popen(self.getCmakeCommand(), shell=True, cwd=self.binary_dir)
            myProcess.wait()
        except:
            print("Error configuring project, see configure command bellow")
            print (self.getCmakeCommand())
            return False
        return True

    def doBuild(self):
        """run the cmake --build command from the configure path"""
        cmd = "cmake --build {}".format(self.binary_dir)

        # using multiprocessor
        nbproc = multiprocessing.cpu_count()
        if(nbproc is not None):
            cmd = cmd + " -- -j {}".format(nbproc)

        try:
            myP = subprocess.Popen(cmd, shell=True, cwd=self.binary_dir)
            myP.wait()
        except:
            print("Error building project!")
            print ("command: " + cmd)
            return False
        return True
