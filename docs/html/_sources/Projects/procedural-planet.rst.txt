
Procedural Planet
==================

.. image:: https://github.com/vasumahesh1/azura/raw/dev/Source/Samples/1_ProceduralPlanet/Images/planet_low.gif
   :target: https://github.com/vasumahesh1/azura/raw/dev/Source/Samples/1_ProceduralPlanet/Images/planet_low.gif
   :alt: 


Supported APIs
---------------

.. list-table::
   :header-rows: 1

   * - D3D12
     - Vulkan
   * - Supported
     - Supported


Features
---------------
* Traditional Deferred Pipeline
* 2 G-Buffers
  * UV & Terrain Elevation
  * Normals
* Reconstruct view space data back in shading phase from buffers
* Multiple Passes - Noise Pass & Sky Pass --> Terrain Pass --> Water Pass
* Blending for Water Pass to be rendered on top of Terrain    


G-Buffer Layout
---------------

.. list-table:: G-Buffer Ouputs
    :header-rows: 1

    * - Depth Buffer
      - G-Buffer 1
      - G-Buffer 2

    * - .. image:: ../../Source/Samples/1_ProceduralPlanet/Images/frame_depth.PNG
          :target: ../_images/frame_depth.PNG
          :alt: Depth Frame

      - .. image:: ../../Source/Samples/1_ProceduralPlanet/Images/frame_normal.PNG
          :target: ../_images/frame_normal.PNG
          :alt: Normals Frame

      - .. image:: ../../Source/Samples/1_ProceduralPlanet/Images/frame_uv_elevation.PNG
          :target: ../_images/frame_uv_elevation.PNG
          :alt: UV & Elevation Frame

    * - float: Depth
      - float4: Normals
      - float4: UV + Elevation


.. list-table:: Final Frame
    :header-rows: 0

    * - .. image:: ../../Source/Samples/1_ProceduralPlanet/Images/frame_main.PNG
          :target: ../_images/frame_main.PNG
          :alt: Main Frame


Project Notes
---------------

This was the first project sample to be made on Azura. It was first built on Vulkan and then later ported to D3D12. You can find the source code for this sample `here <https://github.com/vasumahesh1/azura/tree/master/Source/Samples/1_ProceduralPlanet>`_.