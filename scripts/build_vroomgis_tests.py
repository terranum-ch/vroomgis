#!/usr/bin/env python
import os
import cmbui

if __name__ == '__main__':
    # use build_vroomgis_apps.py first in order to create caches!
    if os.path.exists("cache_cmake.txt") is False:
        print ("cache_cmake.txt didn't exist... run build_vroomgis_apps.py first!")
        exit()

    if os.path.exists("cache_build.txt") is False:
        print ("cache_build.txt didn't exist... run build_vroomgis_apps.py first!")
        exit()

    # create the test cache if needed
    mytestcache = cmbui.FileCache("cache_test.txt")
    mytestcache.appendQuestion("cxxtest library path" , "CXXTEST_DIRECTORY")
    mytestcache.appendQuestion("Unit testing path" , "UNIT_TESTING_PATH")
    mytestcache.hasCacheFile(createifneeded=True)

    mybuildcache = cmbui.FileCache("cache_build.txt")
    mycmakecache = cmbui.FileCache("cache_cmake.txt")
    myBuild = cmbui.BuildProject(mybuildcache.readCacheValue("CMAKE_GENERATOR"),
                                 mycmakecache.readCacheLines() + mytestcache.readCacheLines(),
                                 "../test/build",
                                 os.path.join(mybuildcache.readCacheValue("BINARY_DIR"), "unit_test"))
    print (myBuild.getCmakeCommand(True))

    myBuild.createPath(myBuild.getBinaryDir(), erase_path=False)
    myBuild.doConfigure()
    myBuild.doBuild()













