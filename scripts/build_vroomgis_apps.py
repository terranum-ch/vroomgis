#!/usr/bin/env python

import os
import subprocess


class FileCache(object):
    """Manage the cache for variables when building projects"""
    def __init__(self, cachefilename="buildcache.txt"):
        self.m_cachefilename = os.path.join(os.path.dirname(os.path.abspath(__file__)), cachefilename)
        self.m_questions = []

    def _cacheQuestion(self, message, value):
        data = raw_input("{} (cmake value : {}) ".format(message, value))
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
            cmakeparams = cmakeparams + " -D" + param

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


if __name__ == '__main__':
    # create the cmake cache if needed
    mycmakecache = FileCache("cache_cmake.txt")
    mycmakecache.appendQuestion("Use VroomGIS Libs 0 or 1", "SEARCH_VROOMGIS_LIBS")
    mycmakecache.appendQuestion("GIS lib path", "SEARCH_GIS_LIB_PATH")
    mycmakecache.appendQuestion("wxWidgets wx-config (Unix ONLY, keep empty for windows)", "wxWidgets_CONFIG_EXECUTABLE")
    # TODO: This has to be tested under Windows. If it's not working, we could define the 'WXWIN' environement variable !
    mycmakecache.appendQuestion("wxWidgets root directory (Windows ONLY, keep empty for Unix)", "wxWidgets_ROOT_DIR")
    mycmakecache.hasCacheFile(createifneeded=True)

    # create the build cache if needed
    mybuildcache = FileCache("cache_build.txt")
    mybuildcache.appendQuestion("Binary directory : ", "BINARY_DIR")
    mybuildcache.appendQuestion("CMake Generators (MSYS Makefiles, Xcode, Visual Studio XX XXXX, etc) : ", "CMAKE_GENERATOR")
    mybuildcache.appendQuestion("Build vroomLoader ? (Y/N)", "BUILD_VROOMLOADER")
    mybuildcache.appendQuestion("Build vroomTwin ? (Y/N)", "BUILD_VROOMTWIN")
    mybuildcache.appendQuestion("Build vroomDrawer ? (Y/N)", "BUILD_VROOMDRAWER")
    mybuildcache.hasCacheFile(createifneeded=True)

    # configure voomLoader
    buildvroomloader = mybuildcache.readCacheValue("BUILD_VROOMLOADER")
    if str(buildvroomloader).upper() == "Y":
        print ("Building vroomLoader !")
        myBuild = BuildProject(mybuildcache.readCacheValue("CMAKE_GENERATOR"),
                               mycmakecache.readCacheLines(),
                               "../app/vroomloader/build",
                               mybuildcache.readCacheValue("BINARY_DIR"))
        print (myBuild.getCmakeCommand(True))









