from conan import ConanFile
from conan.tools.cmake import CMakeDeps
from conan.tools.microsoft import MSBuildDeps

required_conan_version = '>=1.55.0'

class Hesternal(ConanFile):
    name = 'Hesternal'
    version = '0.1.0'
    settings = {
        'os': ['Windows'],
        'arch': ['x86_64'],
        'compiler': None,
        'build_type': ['Debug', 'Release'],
    }

    # Hack to add Development build configuration, don't know how else
    options = {
        'development': [False, True]
    }

    #generators = 'cmake_find_package_multi'

    requires = [
          'assimp/5.2.2',
          'catch2/3.1.0',
         'cxxopts/3.0.0',
            'entt/3.10.0',
           'imgui/cci.20220621+1.88.docking',
          'spdlog/1.10.0',
             'stb/cci.20210910',
    ]

    default_options = (
        'cxxopts:unicode=False',
         'spdlog:wchar_support=True',
            'stb:with_deprecated=False'
    )

    # def configure(self):

    # def generate(self):
    #     cmake = CMakeDeps(self)
    #     if self.options.final == True and self.settings.build_type == 'Release':
    #         cmake.configuration = 'Final'
    #     cmake.generate()

    def generate(self):
        ms = MSBuildDeps(self)

        if self.settings.build_type == 'Debug':
            ms.configuration = 'msvc-debug'
        elif self.settings.build_type == 'Release':
            if  self.options.development == True:
                ms.configuration = 'msvc-development'
            else:
                ms.configuration = 'msvc-release'

        ms.generate()
