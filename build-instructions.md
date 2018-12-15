---
description: This page documents the python based build system on Azura.
---

# Build Instructions

## Build Structure

Builds are always generated for a `target`. The following set of targets are available:

* Win64
* Win32\*
* Mac64 = Soon\(tm\)

These targets are combined with a Debug or Release specification and generate the following string:

* WIN64\_DEBUG
* WIN64\_RELEASE
* WIN32\_DEBUG\*
* WIN32\_RELEASE\*
* MAC64\_DEBUG - Soon\(tm\)
* MAC64\_RELEASE - Soon\(tm\)

\* = Currently disabled due to CI issues. 

These are basically a string of Platform + Architecture + Debug/Release combined. This allows us to build multiple targets later on.



The following folders are also available:

* `Build/` 

  This folder is the main folder where a build is made. Builds are made without any IDE support and are generated via CMake and Ninja. `--build` controls if you want to build `Debug` or `Release` . And `--target` controls the target \(listed above\)

* `BuildProjects/`

  This folder contains project files \(Currently tuned for Visual Studio 2017\). This folder will contain you visual studio files. You might want to rebuild these files if the CMake files change. `--projectFiles` flag controls if we want to build project files or not.

## The Most Basic Build

The most basic build must include these commands:

```bash
$ python build.py --project all --target Win64
```

{% hint style="info" %}
 Defaults to Debug mode
{% endhint %}

## Building IDE Project Files

You can generate IDE files \(Visual Studio files\) by appending the flag `--projectFiles` This will tell Azura to generate a set of project files along with a regular build.

Note: You don't have to do this every single time. This will waste a lot of time building them. One should only use `--projectFiles` only when CMakeLists.txt changes with additions / removals related to Shaders / Textures / CPP Files or any other files that are linked to CMake targets.

## Different System - Different Configs

Azura only needs one thing in your PATH to be operational. It is a link to your Python27 application. Azura creates a copy of your environment variables and modifies them. It injects paths to MSVC Compilers and other platform specific things and calls CMake.

{% hint style="info" %}
All Configs are located in the `External` folder
{% endhint %}

There are now two ways for you to build:

* Build the way Azura builds by default
  * Involves: Copying your files & extra HDD space
  * Steps:
    * Look at the [default config file](https://github.com/vasumahesh1/azura/blob/master/External/Config.ini) Azura picks up
    * Make sure all of these paths exists - You will need to copy them over to `External/<Platform>/` 
    * 



* Build your own way
  * Involves: Modifying two Config INI files that point to the correct Libraries and Platform SDKs



