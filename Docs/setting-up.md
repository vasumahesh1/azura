# Setting Up

## 1. Different System Configs

Azura only needs one thing in your PATH to be operational. It is a link to your Python27 application. Azura creates a copy of your environment variables and modifies them. It injects paths to MSVC Compilers and other platform specific things and calls CMake.

{% hint style="info" %}
All build related configs are located in the `External` folder
{% endhint %}

There are now two ways for you to build:

* **Build the way Azura builds by default**
  * Involves: Copying your files & extra HDD space
  * Steps:

    * Look at the [default config file](https://github.com/vasumahesh1/azura/blob/master/External/Config.ini) Azura picks up
    * Make sure all of these paths exists - You will need to copy them over to `External/<Platform>/` or download the exact version online.
    * [Appveyor YML file](https://github.com/vasumahesh1/azura/blob/master/appveyor.yml) might be helpful to get the direct download URL
* **Build your own way**
  * Involves: Modifying two Config INI files that point to the correct Libraries and Platform SDKs
  * Not recommended for beginners

{% hint style="danger" %}
Qt is known to create issues with Azura. Why? Qt straight up pollutes your PATH variable which confuses CMake in Azura to build for MSVC or MinGW. There is a variable injected by Qt which you can delete from your PATH environment variable.
{% endhint %}

## 2. Download Dependencies

There are two sub steps for dependencies.

1. **Sub-module clone:**

   Just run `git submodule update --init --recursive` like normal and it should download all the dependencies needed. Currently, it downloads:

   * Source/Imports/Common/benchmark
   * Source/Imports/Common/glTF-SDK
   * Source/Imports/Common/glfw
   * Source/Imports/Common/mathfu
   * Source/Imports/Common/stb
   * Source/Imports/Common/yaml-cpp

2. **Manual download:**

   Sadly, we need to still download some of the stuff manually. This includes some stuff that can't be on github. We recommend checking the [Appveyor YML file](https://github.com/vasumahesh1/azura/blob/master/appveyor.yml) which have a direct download link to **most** of these. They are:

   * Vulkan SDK 1.1.77.0 - Located at: `Source/Imports/<Platform>/Vulkan/1.1.77.0/`
   * Slang 0.11.16 - Located at `Source/Imports/<Platform>/Slang/slang-0.11.16/`
   * Boost 1.67 - Located at `Source/Imports/<Platform>/Boost/boost_1_67_0/`

They are all versioned appropriately. In case for version changes, we just add a new folder.

{% hint style="info" %}
Difference between `External/<Platform>/` and `Source/Imports/<Platform>/`

Both of these folders include Platform specific things. Why the two folders?

There is a semantic difference. For example:

In Windows, we have:

`External/Windows` and `Source/Imports/Windows`

Now a question: Where to place cmake.exe? This is a _tool_ that Windows uses and is unrelated to the source code. Hence, it should stay outside the Source folder. That is why CMake is inside the `External` folder.

Another: Where to place vulkan.h and vulkan win32 headers? These are _C++ files_ that are used by Azura. Hence, they should be inside `Source/Imports/Windows`
{% endhint %}



