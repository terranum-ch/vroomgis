from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout


class vroomgis(ConanFile):
    name = "anote"
    version = "1.2"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    # requires = [
    #     "wxwidgets/3.3.3",
    #     "gdal/3.10.3@terranum-conan+gdal/stable",
    #     "libdeflate/1.19",
    #     "proj/9.3.1",
    #     "libtiff/4.7.0",
    #     "sqlite3/3.45.0",
    # ]

    options = {
        "build_tests": [True, False],
        "build_apps": [True, False],
    }
    default_options = {
        "build_tests": True,
        "build_apps": True,
    }


    def requirements(self):
        self.requires("wxwidgets/3.3.3")
        self.requires("gdal/3.13.0")

        # Alignement sur le binaire GDAL de ConanCenter.
        self.requires("arrow/19.0.1", override=True)
        self.requires("boost/1.90.0", override=True)
        self.requires("libcurl/8.20.0", override=True)
        self.requires("expat/2.8.1", override=True)

        if self.options.build_tests:
            self.requires("gtest/1.18.0")

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
