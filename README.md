# README

## Azura

A repository containing tools for real-time rendering, general C++, memory allocators, path tracers and more!

Active Platforms on Azura:

| Windows | Linux |
| :--- | :---: |
| [![Build Status: Windows](https://ci.appveyor.com/api/projects/status/github/vasumahesh1/azura)](https://ci.appveyor.com/project/vasumahesh1/azura) | Soon\(tm\) |

Active Draw APIs on Azura:

| D3D12 | Vulkan |
| :---: | :---: |
| Supported | Supported |

![](https://github.com/vasumahesh1/azura/raw/dev/Source/Samples/1_ProceduralPlanet/Images/planet_low.gif)

### [Main Docs](https://vasumahesh1.github.io/azura_docs/)

### [Render Samples](https://vasumahesh1.github.io/azura_docs/#render-samples)

## Table of Contents

* [Dependencies](./#dependencies)
* [Documentation Site](https://vasumahesh1.github.io/azura_docs/)
* [Benchmarking](benchmarks.md)

## Dependencies

Build time dependencies:

* [CMake \(3.11 or above\)](https://cmake.org/)
* [Ninja](https://ninja-build.org/)

Test / Code Sanity dependencies:

* [Google Test](https://github.com/google/googletest)
* [Google Benchmark](https://github.com/google/benchmark)
* [Clang-tidy \(LLVM\)](https://llvm.org/)

Code dependencies:

* [yaml-cpp](https://github.com/jbeder/yaml-cpp)
* [boost](https://www.boost.org/)
* [mathfu](https://github.com/google/mathfu)
* [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/)
* [GLFW](https://www.glfw.org/)
* [GLTF SDK](https://github.com/Microsoft/glTF-SDK)

Most of the dependencies are located at Source/Imports/ and are pulled automatically using git submodule. If you cloned this repository, you can run:

```text
git submodule update --init --recursive
```

This will download most of the code dependencies.



