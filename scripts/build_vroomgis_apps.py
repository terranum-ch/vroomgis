#!/usr/bin/env python
import os
import cmbui

if __name__ == '__main__':
    # create the cmake cache if needed
    mycmakecache = cmbui.FileCache("cache_cmake.txt")
    mycmakecache.appendQuestion("Use VroomGIS Libs 0 or 1", "SEARCH_VROOMGIS_LIBS")
    mycmakecache.appendQuestion("GIS lib path", "SEARCH_GIS_LIB_PATH")
    mycmakecache.appendQuestion("wxWidgets wx-config (Unix ONLY, keep empty for windows)", "wxWidgets_CONFIG_EXECUTABLE")
    # TODO: This has to be tested under Windows. If it's not working, we could define the 'WXWIN' environement variable !
    mycmakecache.appendQuestion("wxWidgets root directory (Windows ONLY, keep empty for Unix)", "wxWidgets_ROOT_DIR")
    mycmakecache.hasCacheFile(createifneeded=True)

    # create the build cache if needed
    mybuildcache = cmbui.FileCache("cache_build.txt")
    mybuildcache.appendQuestion("Binary directory : ", "BINARY_DIR")
    mybuildcache.appendQuestion("CMake Generators (MSYS Makefiles, Xcode, Visual Studio XX XXXX, etc) : ", "CMAKE_GENERATOR")
    mybuildcache.appendQuestion("configure vroomLoader ? (Y/N)", "CONFIGURE_VROOMLOADER")
    mybuildcache.appendQuestion("configure vroomTwin ? (Y/N)", "CONFIGURE_VROOMTWIN")
    mybuildcache.appendQuestion("configure vroomDrawer ? (Y/N)", "CONFIGURE_VROOMDRAWER")
    mybuildcache.appendQuestion("build applications ? (Y/N)", "BUILD_APPS")
    mybuildcache.hasCacheFile(createifneeded=True)

    buildApp = False
    if mybuildcache.readCacheValue("BUILD_APPS").upper() == "Y":
        buildApp = True

    # configure voomLoader
    buildvroomloader = mybuildcache.readCacheValue("CONFIGURE_VROOMLOADER")
    if str(buildvroomloader).upper() == "Y":
        print ("Building vroomLoader !")
        myBuild = cmbui.BuildProject(mybuildcache.readCacheValue("CMAKE_GENERATOR"),
                               mycmakecache.readCacheLines(),
                               "../app/vroomloader/build",
                                os.path.join(mybuildcache.readCacheValue("BINARY_DIR"), "vroomloader"))
        print (myBuild.getCmakeCommand(True))

        # create directory if needed
        myBuild.createPath(myBuild.getBinaryDir(), erase_path=False)
        myBuild.doConfigure()

        if (buildApp):
            myBuild.doBuild()

    # configure voomDrawer
    buildvroomloader = mybuildcache.readCacheValue("CONFIGURE_VROOMDRAWER")
    if str(buildvroomloader).upper() == "Y":
        print ("Building vroomDrawer !")
        myBuild = cmbui.BuildProject(mybuildcache.readCacheValue("CMAKE_GENERATOR"),
                                     mycmakecache.readCacheLines(),
                                     "../app/vroomdrawer/build",
                                     os.path.join(mybuildcache.readCacheValue("BINARY_DIR"), "vroomdrawer"))
        print (myBuild.getCmakeCommand(True))

        # create directory if needed
        myBuild.createPath(myBuild.getBinaryDir(), erase_path=False)
        myBuild.doConfigure()

        if (buildApp):
            myBuild.doBuild()

    # configure voomTwin
    buildvroomloader = mybuildcache.readCacheValue("CONFIGURE_VROOMTWIN")
    if str(buildvroomloader).upper() == "Y":
        print ("Building vroomTwin !")
        myBuild = cmbui.BuildProject(mybuildcache.readCacheValue("CMAKE_GENERATOR"),
                                     mycmakecache.readCacheLines(),
                                     "../app/vroomtwin/build",
                                     os.path.join(mybuildcache.readCacheValue("BINARY_DIR"), "vroomtwin"))
        print (myBuild.getCmakeCommand(True))

        # create directory if needed
        myBuild.createPath(myBuild.getBinaryDir(), erase_path=False)
        myBuild.doConfigure()

        if (buildApp):
            myBuild.doBuild()












