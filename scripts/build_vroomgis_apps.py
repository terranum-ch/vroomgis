#!/usr/bin/env python

import os

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
                return line[line.find("=")+1:]
        return ""


if __name__ == '__main__':
    # create the cmake cache if needed
    mycmakecache = FileCache("cache_cmake.txt")
    mycmakecache.appendQuestion("Use VroomGIS Libs 0 or 1", "SEARCH_VROOMGIS_LIBS")
    mycmakecache.appendQuestion("GIS lib path", "SEARCH_GIS_LIB_PATH")
    mycmakecache.appendQuestion("wxWidgets wx-config (Unix ONLY, keep empty for windows)", "wxWidgets_CONFIG_EXECUTABLE")
    # TODO: This has to be tested under Windows. Or we could define the 'WXWIN' environement variable !
    mycmakecache.appendQuestion("wxWidgets root directory (Windows ONLY, keep empty for Unix)", "wxWidgets_ROOT_DIR")
    mycmakecache.hasCacheFile(createifneeded=True)

    #print(mycmakecache.readCacheValue("SEARCH_GIS_LIB_PATH"))

    # create the build cache if needed






