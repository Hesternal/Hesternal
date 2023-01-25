# Hesternal

- C++ 23 with modules
- Hesternal.BuildTool for project/solution generation from C# scripts
  - Project Generators
    - Visual Studio
    - CMake \[WIP\]
  - Compilers
    - MSVC
    - Clang \[WIP\]
- Hesternal.HeaderTool for C++ reflection
  - Should've used llvm/clang from the start for c++ parser instead of writing my own
  - Generated sources are added through custom MSBuild task
    - Should've made it as a console tool not as a MSBuild C# dll
      - adding support for CMake and Clang AST analyzer would've been much easier

## Build

Requirements:
- Visual Studio 2022 17.5.0 Preview 4.0
- [.Net 7.0.100](https://dotnet.microsoft.com/en-us/download/dotnet/7.0)
- [Python 3.11.1](https://www.python.org/downloads/)
  - Conan 1.55.0

Commands:
```
conan-init.ps1
generate-vs.ps1
```
