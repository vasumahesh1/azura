
Build Instructions
==================

Build Structure
---------------

Builds are always generated for a ``target``. The following set of targets are available:


* Win64
* Win32*
* Mac64 = Soon(tm)

These targets are combined with a Debug or Release specification and generate the following string:


* WIN64_DEBUG
* WIN64_RELEASE
* WIN32_DEBUG*
* WIN32_RELEASE*
* MAC64_DEBUG - Soon :sup:`tm`
* MAC64_RELEASE - Soon :sup:`tm`

``*`` = Currently disabled due to CI issues. 

These are basically a string of Platform + Architecture + Debug/Release combined. This allows us to build multiple targets later on.

The following folders are also available:


* 
  ``Build/`` 

  This folder is the main folder where a build is made. Builds are made without any IDE support and are generated via CMake and Ninja. ``--build`` controls if you want to build ``Debug`` or ``Release`` . And ``--target`` controls the target (listed above)

* 
  ``BuildProjects/``

  This folder contains project files (Currently tuned for Visual Studio 2017). This folder will contain you visual studio files. You might want to rebuild these files if the CMake files change. ``--projectFiles`` flag controls if we want to build project files or not.

The Most Basic Build
--------------------

The most basic build must include these commands:

.. code-block:: bash

   $ python build.py --project all --target Win64

.. note:: Defaults to Debug mode, use ``--build Release`` to build Release mode.

Building IDE Project Files
--------------------------

You can generate IDE files (Visual Studio files) by appending the flag ``--projectFiles`` This will tell Azura to generate a set of project files along with a regular build.

Note: You don't have to do this every single time. This will waste a lot of time building them. One should only use ``--projectFiles`` only when CMakeLists.txt changes with additions / removals related to Shaders / Textures / CPP Files or any other files that are linked to CMake targets.

.. code-block:: bash

   $ python build.py --project all --target Win64 --projectFiles

This will create the required project files in the ``BuildProjects/`` folder.

Windows Specific Project Files:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For Visual Studio open `Mundus <http://elderscrolls.wikia.com/wiki/Mundus>`_.sln

This is the one single visual studio solution containing all projects. You can now set any project (that is an executable) as a Start Up Project. You can now build and code as usual.

.. note:: Visual Studio projects have reduced static analysis and compiler checks. In short, if your visual studio project builds then the main build may or may not build. This is due to stricter checks in the main build allowing you to hack through code in VS. But obviously, correcting the hacks once the code works.

