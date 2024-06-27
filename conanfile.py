from conans import ConanFile, CMake
import os


class vroomgis(ConanFile):
    settings = "os", "compiler", "build_type", "arch"

    requires = [
        "wxwidgets/3.2.5@terranum-conan+wxwidgets/stable",
        "gdal/3.8.3@terranum-conan+gdal/stable",
        "libdeflate/1.19",
        "proj/9.3.1",
        "libtiff/4.6.0",
        "sqlite3/3.44.2",
        "libiconv/1.17",
        "libjpeg/9e",
        #"geos/3.12.0",
        #"libspatialite/5.0.1",
        #"libcurl/7.87.0",
        #"libpng/1.6.40",
        #"zlib/1.2.12"
    ]

    options = {
        "build_tests": [True, False],
        "build_apps": [True, False],
    }
    default_options = {
        "build_tests": True,
        "build_apps": True,
    }

    generators = "cmake", "gcc"

    def requirements(self):
        if self.options.build_tests:
            self.requires("gtest/1.11.0")

    def configure(self):
        self.options["gdal"].with_curl = True
        self.options["gdal"].shared = True
        if self.settings.os == "Linux":
            self.options["wxwidgets"].png = "system" # use png sys lib on linux, otherwise leads to a crash.
            self.options['gdal'].with_png = False # to avoid static linking of libpng

    def imports(self):
        # Copy libraries
        if self.settings.os == "Windows":
            self.copy("*.dll", dst="bin", src="@bindirs")
        if self.settings.os == "Macos":
            self.copy("*.dylib", dst="bin", src="@libdirs")
        if self.settings.os == "Linux":
            self.copy("*.so*", dst="bin", src="@libdirs")

        # Copy proj library data
        if self.settings.os == "Windows" or self.settings.os == "Linux":
            self.copy("*", dst="share/proj", src="res", root_package="proj")

        # Copy gdal library data
        if self.settings.os == "Windows" or self.settings.os == "Linux":
            self.copy("*", dst="share", src="res", root_package="gdal")

    def build(self):
        cmake = CMake(self)

        cmake.definitions["BUILD_TESTS"] = self.options.build_tests
        cmake.definitions["BUILD_APPS"] = self.options.build_apps

        cmake.configure()
        cmake.build()
        if self.settings.os == "Macos":
            cmake.install()
