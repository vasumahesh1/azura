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
  * [Renders](#renders)
  * [Benchmarking](#benchmarking)
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


Benchmarking
=================

Time is `(New - Old) / Old`, basically how much times it is slower or faster. Negative values means they are x times faster than standard. We always compare standard -> azura implementation.

#### std::vector<T> vs Containers::Vector<T>

A point to note here, Azura vectors don't allocate memory on heap directly. It is forwarded to an Allocator. The following benchmark is using a `MemoryBuffer` on the Heap and using a `MonotonicAllocator`. MonotonicAllocators are sequential allocators that are really cheap and fast. It was no suprise here that my vector was able to perform better (as it had no allocation overhead). Later on, Tests for other allocators will also be present here.

```
Comparing .\StdBenchmark.exe to .\CommonBenchmark.exe
Benchmark                                                                Time             CPU      Time Old      Time New       CPU Old       CPU New
-----------------------------------------------------------------------------------------------------------------------------------------------------
VectorBenchmarkFixture/BM_EmptyCreation                               +0.0120         +0.0625             0             0             0             0
VectorBenchmarkFixture/BM_EmptyCreation_ComplexType                   -0.0013         +0.0000             0             0             0             0
VectorBenchmarkFixture/BM_InsertWithReserve/8                         -0.5690         -0.5750            61            26            61            26
VectorBenchmarkFixture/BM_InsertWithReserve/16                        -0.4392         -0.4367            69            38            68            39
VectorBenchmarkFixture/BM_InsertWithReserve/32                        -0.3772         -0.3889            99            62           100            61
VectorBenchmarkFixture/BM_InsertWithReserve/64                        -0.2421         -0.2546           149           113           151           112
VectorBenchmarkFixture/BM_InsertWithReserve/128                       -0.1731         -0.1833           248           205           251           205
VectorBenchmarkFixture/BM_InsertWithReserve/256                       -0.0783         -0.0873           437           403           439           401
VectorBenchmarkFixture/BM_InsertWithReserve/512                       -0.0486         -0.0426           823           783           820           785
VectorBenchmarkFixture/BM_InsertWithReserve/1024                      -0.0267         -0.0217          1595          1552          1604          1569
VectorBenchmarkFixture/BM_InsertWithReserve/2048                      -0.0155         +0.0000          3128          3080          3069          3069
VectorBenchmarkFixture/BM_InsertWithReserve/4096                      -0.0182         +0.0000          6299          6184          6278          6278
VectorBenchmarkFixture/BM_InsertWithReserve/8192                      -0.0270         -0.0222         12602         12262         12556         12277
VectorBenchmarkFixture/BM_InsertWithReserve_Heavy/8                   -0.5525         -0.5481           399           179           399           180
VectorBenchmarkFixture/BM_InsertWithReserve_Heavy/16                  -0.5283         -0.5351           909           429           924           430
VectorBenchmarkFixture/BM_InsertWithReserve_Heavy/32                  -0.4713         -0.4788          2276          1204          2302          1200
VectorBenchmarkFixture/BM_InsertWithReserve_Heavy/64                  -0.3668         -0.3750          6182          3915          6278          3924
VectorBenchmarkFixture/BM_InsertWithReserve_Heavy/128                 -0.2642         -0.2667         18919         13921         18834         13811
VectorBenchmarkFixture/BM_InsertWithReserve_Heavy/256                 -0.3805         -0.3783         84486         52342         85449         53125
VectorBenchmarkFixture/BM_InsertWithReserve_Heavy/512                 -0.1939         -0.1998        252027        203167        254981        204041
VectorBenchmarkFixture/BM_InsertWithReserve_Heavy/1024                -0.1473         -0.1663        933825        796244        941265        784738
VectorBenchmarkFixture/BM_InsertWithReserve_Heavy/2048                -0.0679         -0.0893       3391269       3161138       3446691       3138951
VectorBenchmarkFixture/BM_InsertWithReserve_Heavy/4096                -0.0401         -0.0426      13093986      12568524      13113839      12555804
VectorBenchmarkFixture/BM_InsertWithReserve_Heavy/8192                -0.0194         +0.0000      51507481      50508109      51562500      51562500
VectorBenchmarkFixture/BM_SetWithResize/8                             -0.5162         -0.5135            52            25            52            25
VectorBenchmarkFixture/BM_SetWithResize/16                            -0.4719         -0.4816            55            29            56            29
VectorBenchmarkFixture/BM_SetWithResize/32                            -0.4102         -0.4133            63            37            63            37
VectorBenchmarkFixture/BM_SetWithResize/64                            -0.3541         -0.3704            93            60            94            59
VectorBenchmarkFixture/BM_SetWithResize/128                           -0.2424         -0.2326           122            92           120            92
VectorBenchmarkFixture/BM_SetWithResize/256                           -0.1081         -0.1107           175           156           176           157
VectorBenchmarkFixture/BM_SetWithResize/512                           -0.0396         -0.0444           299           287           298           285
VectorBenchmarkFixture/BM_SetWithResize/1024                          +0.0718         +0.0889           505           541           502           547
VectorBenchmarkFixture/BM_SetWithResize/2048                          +0.0713         +0.0851           995          1066           984          1067
VectorBenchmarkFixture/BM_SetWithResize/4096                          +0.1307         +0.1407          1898          2146          1883          2148
VectorBenchmarkFixture/BM_SetWithResize/8192                          +0.1156         +0.1005          3807          4248          3850          4238
```

License
=================
Copyright 2018 Vasu Mahesh

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.