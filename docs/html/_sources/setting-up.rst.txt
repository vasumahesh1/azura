Setting Up
==========

0. PATH setup
~~~~~~~~~~~~~

Azura's build system runs on Python 3.

Make sure you have added python 3 added to your PATH environment variable. This makes it easier to invoke the build system.

Currently used: Python 3.7


.. warning:: Qt is known to create issues with Azura. Why? Qt straight up pollutes your PATH variable which confuses CMake in Azura to build for MSVC or MinGW. There is a variable injected by Qt which you can delete from your PATH environment variable.

.. warning:: If you have VulkanSDK installed for Windows, we recommend uninstalling it if you just started using Azura.


1. Clone and Install submodules
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You can start by `cloning azura <https://github.com/vasumahesh1/azura>`_ from github.

Then make sure to install all the submodules.

.. code-block:: sh

   git submodule update --init --recursive

This will download all the code related dependencies.

.. warning:: Make sure you use default command prompt, or Powershell or Cmder for downloading / cloning Azura. Not sure about Git Bash but Ubuntu Subsystem for Windows has issues with cloning the submodules for Azura.

2. Run the Install Script
~~~~~~~~~~~~~~~~~~~~~~~~~

There is a install script provided that auto downloads most non-compiler related dependencies. You need to have 7 zip installed. It can be in your PATH but it is not necessary.

You can execute the script by calling:

.. code-block:: sh

   # Direct Usage (if 7z in PATH)
   python install.py

   # Direct Usage (if 7z not in PATH, give the location)
   python install.py --zipExtractor "C:\Program Files\7-Zip\7z.exe"

Sometimes you may need to install only one dependency. You can then use:

.. code-block:: sh

   python install.py --only <Name>
   python install.py --only Vulkan

   # ---
   # OR (Based on your zip extractor usage)
   # ---

   python install.py --zipExtractor "C:\Program Files\7-Zip\7z.exe" --only <Name>
   python install.py --zipExtractor "C:\Program Files\7-Zip\7z.exe" --only Vulkan

All installations are silent and will not request any UI. However, some installations may ask for Windows UAC access (Windows only thing).

.. warning:: Things like LLVM & Vulkan will actually "install" it inside Azura causing an entry to pop up in system control panel. If you are deleting it be sure to check your Control Panel while uninstalling it.

The install script will download almost all of the dependencies. But, it doesn't download Windows compiler dependencies.


3. Manual Installation
~~~~~~~~~~~~~~~~~~~~~~

There are some platform specific things that need to be done. Sadly, they are to be done manually.

.. note:: All build related configs are located in the ``External`` folder

[REQUIRED] Build Requirements:
""""""""""""""""""""""""""""""

* 
  Windows

  You will need to either specify the path to your Microsoft compilers OR just copy them over to the respective paths that Azura checks for.

  There are two ways to do this. For both ways, We recommend to have a look at `Config.ini <https://github.com/vasumahesh1/azura/blob/master/External/Config.ini>`_ and `Appveyor's Config <https://github.com/vasumahesh1/azura/blob/master/External/AppveyorConfig.ini>`_.


  * 
    Copy Over Path Method (Following Paths must exist and must contain the compiler files):

    .. code-block:: ini

       VisualStudioBuildTools/VC/Tools/MSVC/14.14.26428/
       Windows Kits/10/Lib/10.0.17134.0/
       Windows Kits/10/bin/10.0.17134.0/
       Windows Kits/10/Include/10.0.17134.0/


  * 
    Linking Path Method

    Following Paths must be linked into a new config file and must be provided to the build system when building. (using the --configFile option)

    You can create a copy of Config.ini and update the following path variables based on your system. Appveyor config does this without copying files.

    .. code-block:: ini

       VSBuildTools
       MSVCPath
       Windows10SDKLib
       Windows10SDKBin
       Windows10SDKInc


* Linux
* MacOS

[OPTIONAL] Documentation Requirements:
""""""""""""""""""""""""""""""""""""""

Tools needed for documentation generation.

.. code-block:: sh

   pip3 install sphinx breathe exhale sphinx_rtd_theme
