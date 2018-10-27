
Azura
=================

A repository containing tools for real-time rendering, general C++, memory allocators, path tracers and more!

Active Platforms on Azura:

| Windows        | Linux        |
| ------------- |:-------------:|
| [![Build Status: Windows](https://ci.appveyor.com/api/projects/status/github/vasumahesh1/azura)](https://ci.appveyor.com/project/vasumahesh1/azura) | Soon(tm) |


Active Draw APIs on Azura:

| D3D12         | Vulkan        |
|:-------------:|:-------------:|
| Supported     | Supported     |


![](https://github.com/vasumahesh1/azura/raw/dev/Source/Samples/1_ProceduralPlanet/Images/planet_low.gif)

## [Main Docs](https://vasumahesh1.github.io/azura_docs/)
## [Render Samples](https://vasumahesh1.github.io/azura_docs/#render-samples)

Table of Contents
=================
  * [Dependencies](#dependencies)
  * [Documentation Site](https://vasumahesh1.github.io/azura_docs/)
  * [Building](#building)
  * [Benchmarking](BENCHMARKS.md)
  * [License](#license)

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
  * [GLTF SDK](https://github.com/Microsoft/glTF-SDK)

Most of the dependencies are located at Source/Imports/ and are pulled automatically using git submodule. If you cloned this repository, you can run:

```
git submodule update --init --recursive
```

This will download most of the code dependencies.

Building
=================

## TLDR

if you want to just do a simple build:

```
python build.py --project all --target Win64
```

## All Options

Building Azura may seem complicated but it is essentially 1 python script doing all the magic. Here is a list of options available to the user:

```bash
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

## Custom CMake Overrides

There are two important config (.ini) files that you can provide to Azura build system.

```
--configFile CONFIGFILE
                        Config File Override

--cmakeConfigFile CMAKECONFIGFILE
                        CMake Config File Override
```

  * **--configFile**

    Config File is your Environment Config.

    ```
    [Windows]
    CMake=Tools/CMake/cmake-3.11.4-win64-x64/bin/
    Ninja=Tools/Ninja/
    LLVM=Tools/LLVM/x64/LLVM/bin/
    VSBuildTools=VisualStudioBuildTools/
    MSVCPath=VisualStudioBuildTools/VC/Tools/MSVC/14.14.26428/
    Windows10SDKLib=Windows Kits/10/Lib/10.0.17134.0/
    Windows10SDKBin=Windows Kits/10/bin/10.0.17134.0/
    Windows10SDKInc=Windows Kits/10/Include/10.0.17134.0/

    [Linux]
    ...

    [MacOS]
    ...
    ```

    If you have custom locations for these above files, you can make a custom ini file and tell Azura to use it. Appveyor basically has its own config located [here](https://github.com/vasumahesh1/azura/blob/master/External/AppveyorConfig.ini).


  * **--cmakeConfigFile**

    These are custom CMake Overrides. Here, you can define custom variables that you want. Appveyor for example already has boost, so we just point boost to the correct place and we are done!

    ```
    [Defines]
    VULKAN_1_1_77_0_ROOT=C:/AppveyorCache/VulkanSDK/1.1.77.0/
    BOOST_ROOT=C:/Libraries/boost_1_67_0
    GLFW_3_2_1_32_ROOT=C:/AppveyorCache/GLFW/glfw-3.2.1.bin.WIN32/
    GLFW_3_2_1_64_ROOT=C:/AppveyorCache/GLFW/glfw-3.2.1.bin.WIN64/
    ```

    You can also build private repositories:

    ```
    FORCE_INCLUDE_SUB_DIRECTORIES=Sandbox;MyCustomPrivateRepo
    ```

Misc
=================

Some pseudo APIs I work on before implementing it on Azura:
[Design Gist Link](https://gist.github.com/vasumahesh1/08fa44f16daba245574794e18ebd47dd)


License
=================
Copyright 2018 Vasu Mahesh

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.