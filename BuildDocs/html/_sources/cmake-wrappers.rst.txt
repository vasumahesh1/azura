
CMake Wrappers
==============

Custom CMake Wrappers
---------------------

If you are modifying the CMake system for Azura or building an application inside Azura, then take a look at these awesome functions to make your life easier.

We use modern CMake, look that up. In short, we define CMake targets and use them. Every project, even 3rd party ones are CMake targets.

Azura has a deploy phase and a build phase. During deploy phase we "install" the application to a deploy specific folder (Example: ``Deploy/WIN64_RELEASE/<CMake Target>/``\ ). When an application is deployed, it basically means that we can zip it and ship it to anyone. The application **must** run as standalone once deployed. This includes having the necessary config, shaders, textures and shared libraries (DLLs).

Azura has CMake functions for both Deploy and Build phase.

Application Build
-----------------

Cross Platform Shader Compilation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

We use a slang shader for compilation. Unfortunately, they don't have a CMake wrapper. Azura provides one with a simple interface.

**API:**

.. code-block:: bash

   AzuraAddSlangShaders(<CMake Target> <HLSL|GLSL|DXBC|VULKAN> <SHADERS>)

**Example:**

.. code-block:: bash

   # Format for shaders: "<Path>|<Shader Type: PS VS CS>"
   # Path must NOT have subfolders. Currently the code just takes the filename directly during export
   set(
     PROJECT_SHADERS
       "${PROJECT_SOURCE_DIR}/Shaders/ShadingPass_Plane.ps.slang|PS"
       "${PROJECT_SOURCE_DIR}/Shaders/ShadingPass_Plane.vs.slang|VS"
   )

   AzuraAddSlangShaders(ClothSim HLSL ${PROJECT_SHADERS})
   AzuraAddSlangShaders(ClothSim DXBC ${PROJECT_SHADERS})

 
^

Copying files that don't compile
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Use this to copy over meshes & textures or any file that is not built using C++ compilers.

**API:**

.. code-block:: bash

   AzuraAddFilesAsDependency(<CMake Target> <Group Name (no space)> <List of Files>)

**Example:**

.. code-block:: bash

   AzuraAddFilesAsDependency(
       ClothSim Meshes
       "Meshes/CustomCloth/buffer.bin"
       "Meshes/CustomCloth/CustomCloth.gltf"
       "Meshes/CustomCloth/ClothDiffuse.jpg"

       "Meshes/Plane/Plane.bin"
       "Meshes/Plane/Plane.gltf"
       "Meshes/Plane/Color plane map.png"

       "Meshes/CustomCloth2/CustomCloth2.gltf"
       "Meshes/CustomCloth2/ClothCustom2_col.jpg"
       "Meshes/CustomCloth2/ClothCustom2_nrm.jpg"
       "Meshes/CustomCloth2/ClothCustom2_rgh.jpg"
       "Meshes/CustomCloth2/ClothCustom2_AO.jpg"
   )

 
^

Adding log support
^^^^^^^^^^^^^^^^^^

Azura has a custom logger. There are 0 cout and 0 printf statements. The log can be piped into the console if needed. To add logging support, call this function with the appropriate level of logging. 50 is the general default.

**API:**

.. code-block:: bash

   AzuraAddLoggingSupport(<CMake Target> <Log Level>)

Importance of log level: Log Level allows certain modules to log at a higher priority (0-100). Currently, all projects are set to a default 50. This allows us to quiet logs that are lower priority.

**Example:**

.. code-block:: bash

   AzuraAddLoggingSupport(ClothSim 50)

Application Deployment
----------------------

Azura is an "out of source" build. We deploy after building the project.

.. code-block:: text

   Flow of code:
   Source/ > Build/ > Deploy/

Deployment happens on top of the build folder.

Here is a quick example on how to deploy an executable:

.. code-block:: text

   AzuraDeployTarget(ClothSim)

   # Copy build folder things to desired deploy directory.
   # Notice how we copy the Shaders which are compiled by the build process.
   AzuraDeployTargetFolder(ClothSim ${PROJECT_BINARY_DIR}/Shaders Shaders/)
   AzuraDeployTargetFolder(ClothSim ${PROJECT_BINARY_DIR}/Textures Textures/)
   AzuraDeployTargetFolder(ClothSim ${PROJECT_BINARY_DIR}/Meshes Meshes/)
