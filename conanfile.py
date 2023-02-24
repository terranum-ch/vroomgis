from conans import ConanFile, CMake
import os


class vroomgis(ConanFile):
    settings = "os", "compiler", "build_type", "arch"

    requires = [
        "geos/3.11.0",
        "libspatialite/5.0.1",
        "libdeflate/1.17",
        "libcurl/7.87.0",
        "proj/9.1.1",
        "libtiff/4.4.0",
        "sqlite3/3.40.1",
        "gdal/3.5.2@terranum-conan+gdal/stable",
        "wxwidgets/3.2.1@terranum-conan+wxwidgets/stable",
        "libiconv/1.17",
        "libpng/1.6.38",
        "libjpeg/9e"
    ]

    options = {
        "build_tests": [True, False],
        "build_apps": [True, False],
    }
    default_options = {
        "build_tests": False,
        "build_apps": False,
    }

    generators = "cmake", "gcc"

    def requirements(self):
        if self.options.build_tests:
            self.requires("gtest/1.11.0")

    def configure(self):
        self.options["gdal"].with_curl = True
        self.options["gdal"].shared = True

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
