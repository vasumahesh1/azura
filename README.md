Azura
=================

A repository containing tools for real-time rendering, general C++, memory allocators, path tracers and more!

Note: This is still a WIP and very much an active project.

| Windows        | Linux        |
| ------------- |:-------------:|
| [![Build Status: Windows](https://ci.appveyor.com/api/projects/status/github/vasumahesh1/azura)](https://ci.appveyor.com/project/vasumahesh1/azura) | Soon(tm) |


Table of Contents
=================
  * [Dependencies](#dependencies)
  * [Building](#building)
  * [Projects](#projects)
    * [Render System](#render-system)
    * [Common](#common)
    * [Log](#Log)

Dependencies
=================

Build time dependencies:
  * [CMake (3.11 or above)](https://cmake.org/)
  * [Ninja](https://ninja-build.org/)

Test / Code Sanity dependencies:
  * [Google Test](https://github.com/google/googletest)
  * [Google Benchmark](https://github.com/google/benchmark)
  * [Clang-tidy (LLVM)](https://llvm.org/)

Code dependencies:
  * [yaml-cpp](https://github.com/jbeder/yaml-cpp)
  * [boost](https://www.boost.org/)
  * [mathfu](https://github.com/google/mathfu)
  * [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/)
  * [GLFW](https://www.glfw.org/)

Most of the dependencies are located at Source/Imports/ and are pulled automatically using git submodule. If you cloned this repository, you can run:

```
git submodule update --init --recursive
```

This will download most of the code dependencies.

Building
=================

Building Azura may seem complicated but it is essentially 1 python script doing all the magic. Here is a list of options available to the user:

```py
usage: build.py [-h] [--project PROJECT] --target TARGET
                [--generator GENERATOR] [--configFile CONFIGFILE]
                [--cmakeConfigFile CMAKECONFIGFILE]
                [--projectGenerator PROJECTGENERATOR] [--buildPath BUILDPATH]
                [--projectPath PROJECTPATH] [--verbose] [--debug] [--clean]
                [--build BUILD] [--projectFiles] [--includeTests] [-v]

optional arguments:
  -h, --help            show this help message and exit

  --project PROJECT     Project to build

  --target TARGET       Project to build on

  --generator GENERATOR
                        CMake Generator

  --configFile CONFIGFILE
                        Config File Override

  --cmakeConfigFile CMAKECONFIGFILE
                        CMake Config File Override

  --projectGenerator PROJECTGENERATOR
                        CMake Project Files Generator
  
  --buildPath BUILDPATH
                        Build Path
  
  --projectPath PROJECTPATH
                        Projects Path
  
  --verbose             Verbose Mode
  
  --debug               Debug Commands Mode
  
  --clean               Clean the project
  
  --build BUILD         Build Release or Debug, defaults to Debug Mode
  
  --projectFiles        Flag to tell the build system to generate project
                        files
  
  --includeTests        Flag to run tests as well
  
  -v                    Version

```

It is highly customizable and uses `.ini` files to change configs. For example, the difference between Appveyor and my current local repository is 1 .ini file.

if you want to just do a simple build:

```
python build.py --project all --target Win64
```