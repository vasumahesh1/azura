.. role:: raw-html-m2r(raw)
   :format: html


Flamenco
========


* 
  Vasu Mahesh
  
  * `LinkedIn <http://linkedin.com/in/vasumahesh>`_

  * `Code Blog <http://www.codeplaysleep.com>`_

* 
  Zach Corse

  * `LinkedIn <https://www.linkedin.com/in/wzcorse/>`_

  * `Personal Website <https://wzcorse.com>`_

Description
^^^^^^^^^^^

Flamenco is a GPU position-based dynamics (PBD) cloth simulation sufficiently fast and robust for use in games. Our take on this well-studied problem is an amalgam of some of the industry's best PBD cloth methods, some dating back as far as 2003, some recently communicated in GDC 2018, all mixed and ported to the GPU [1]_ [2]_. By parallelizing these methods on the GPU, we achieve frame rates far higher than their CPU-based counterparts, and easily satisfy the game industry's 60 fps standard.


.. image:: ../../Source/Samples/3_ClothSim/Images/default_cloth_30x30_(0.8-0.7-0.3).gif
   :alt: Main GIF

----


Methodology
^^^^^^^^^^^

PBD Algorithm
~~~~~~~~~~~~~

We write the core PBD algorithm here for convenience [4]_:

.. code-block:: cpp

   ALGORITHM Position Based Dyanmics:
     for all vertices i do
       initialize x_i, vi, wi = 1/mi 
     end for
     loop
       for all vertices i do vi = vi + wi * dt * f_ext
       for all vertices i do pi = xi + dt * vi
       for all vertices i do genCollConstraints(xi, pi)
       loop solverIterations times
         projectConstraints()
       end loop
       for all vertices i do
         vi = (pi - xi) / dt
         xi = pi
       end for
     end loop
   END

GPU-Based PBD Solver
~~~~~~~~~~~~~~~~~~~~


.. image:: ../../Source/Samples/3_ClothSim/Images/SolverPasses.PNG
   :target: ../_images/SolverPasses.PNG
   :alt: Solver Passes


The PBD algorithm is typically evaluated on the CPU using a Gauss-Seidel type solver, which works exclusively in a serial fashion. Porting the PBD algorithm therefore requires a different approach. Researchers typically choose one of two methods - A Jacobi iterative solver or a graph coloring algorithm [3]_. The graph coloring method identifies independent sets of vertices (those not linked by constraint functions) then solves the constraints associated with each of these sets in a serial fashion using the Gauss-Seidel method. While this method guarantees convergence, it limits vertex/constraint throughput on the GPU and is still inherently serial. The Jacobi method, on the other hand, maximizes parallelism but does not guarantee convergence without the following adjustment to the PBD algorithm: the change in position is computed for each vertex i for all constraints that apply to i in parallel. The final correction applied to i, however, is the average of these adjustments.

Geometric Constraints
~~~~~~~~~~~~~~~~~~~~~

Our cloth model includes four distinct geometric constraints intended to approximate real cloth behavior: distance, isometric bending, long-range attachments, and anchor constraints. We briefly describe each of these here.

Distance
""""""""

The most primitive of geometric constraints, distance-based constraints restore mesh edges that extend or compress relative to their rest lengths.

Isometric Bending
"""""""""""""""""

The isometric bending constraint restores winged edges to their rest configurations, and is best used with inextensible cloth materials.

Anchors
"""""""

We set the mass of vertices we wish to use as anchor points to infinity. Hence, anchor point weights w = 1 / m are zero, implying that anchor positions are unaffected by constraints.

Long-Range Attachments
""""""""""""""""""""""

We include this additional distance-based constraint because it helps enforce inextensibility when the cloth model includes anchors.


.. image:: ../../Source/Samples/3_ClothSim/Images/default_curtain_70x40_(0.8-0.7-0.3).gif
   :alt: Curtain Effect

*A Shower Curtain Using Anchors and Long-Range Attachments*

Environment Collisions
~~~~~~~~~~~~~~~~~~~~~~

Environment collisions are currently restricted to SDF-based models for the sphere and plane. The self-collision method described below naturally extends to all point-triangle collisions that might occur in the scene, but SDFs simplify the evaluation of core cloth physics.

Sphere
""""""

Every vertex position is checked to make sure it is outside the radius of the sphere centered at some origin. If not, the vertex is pushed out in the direction parallel to the vector connecting the vertex's pre-projected position and the sphere's center.

Plane
"""""

Every vertex position is checked to make sure it remains on the same side of the plane it was on in the previous time step. If not, the vertex is pushed back in the direction normal to the plane.


.. image:: ../../Source/Samples/3_ClothSim/Images/cloth_collision.gif
   :alt: Cloth Environment Collisions

*Plane and Sphere SDF Cloth Collisions*

Mesh Definition
~~~~~~~~~~~~~~~

Mesh behavior is a function of the constraints applied to the mesh, and these in turn are a function of the mesh's topology. We observe behavioral differences depending on the choice of mesh discretization. We demonstrate two such discretization's below:

.. list-table::
   :header-rows: 0

   * - .. image:: ../../Source/Samples/3_ClothSim/Images/meshCenterTopology.JPG
        :alt: Center Mesh Topology
     
     - .. image:: ../../Source/Samples/3_ClothSim/Images/meshDefaultTopology.JPG
        :alt: Mesh Default Topology

On the left, we've specified a topology that divides the mesh such that every interior vertex is defined by the joining of four or eight identical triangles that meet isotropically in x and y. This topology guarantees more symmetric behavior. On the other hand, most default meshes will be topologized as on the right. Here, we can see that vertices are defined by the joining of six triangles, but these are biased along a particular diagonal direction. This mesh topology satisfies distance-based constraints more easily, but is biased in its evaluation of bending constraints.

Spatial Hashing with Predictive Constraints for Self-Collisions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To implement naive self-collision constraint generation, every vertex must be checked against every triangle in the mesh. We accelerate this process by using an adaptive spatial hash grid to bin mesh vertices before constraint projection. We then compute the axis-aligned bounding box encapsulating each triangle and its projected displacement. Then, following Chris Lewin's [2] prescription for predictive constraints (introduced at GDC 2018, see links below), we generate all self-collision constraints for the cloth. Predictive constraints are computationally expedient and guarantee that cloth vertices never pass through the mesh.

Modified Spatial Hash Grid for Predictive Constraints [5]_

.. image:: ../../Source/Samples/3_ClothSim/Images/spatial_grid.JPG
   :alt: Modified Spatial Hash Grid

.. image:: ../../Source/Samples/3_ClothSim/Images/default_twist_30x30_(0.8-0.7-0.3).gif
   :alt: Twist

*Predictive Constraints Resolve Self-Collisions*

----

Implementation
^^^^^^^^^^^^^^

This project was written for an engine being developed by one of the authors (see links above). This engine builds to D3D12 and Vulkan for rendering, but for this particular project we restrict ourselves to the D3D12 build. To implement the above methodologies, we include the following shader passes:

Compute Vertex Projected Positions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After updating particle velocites with any external forces, we update vertex positions: pi = xi + dt * vi in parallel.

Binning Initialization
~~~~~~~~~~~~~~~~~~~~~~

This pass empties each hash grid cell of all previously stored vertices.

Vertex Binning
~~~~~~~~~~~~~~

Here, all cloth vertices are binned into their appropriate hash grid cells.

Generate Self-Collision Constraints
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We calculate the axis-aligned bounding box encapsulating each triangle and its projected displaced location and check all hash grid cells this box intersects. We then compare the projected positions of vertices binned in these cells with the barycentric-interpolated time-projected position of this vertex's spatially-projected position on the triangle in consideration. If these predictive measurements indicate a collision is imminent, a predictive constraint is generated using a normal triangle-point collision constraint that is evaluated in the triangle-point pair's pre-projected frame.

Apply Vertex Constraints
~~~~~~~~~~~~~~~~~~~~~~~~

Here, all constraints are evaluated in parallel, including the self-collision constraints generated in the previous step, distance constraints, long-range attachment constraints, and bending constraints.

Apply Deltas
~~~~~~~~~~~~

We keep track of the total displacement of each vertex as well as the number of constraints that have affected each vertex. We then average these displacements by dividing by the number of constraints affecting each vertex. This is the essential process in a Jacobi-style solver. Finally, we resolve all SDF environment collisions here.

Compute Vertex Positions
~~~~~~~~~~~~~~~~~~~~~~~~

Vertex positions and velocities are updated in parallel here as per the PBD algorithm.

NOTE: Apply Vertex Constraints, Apply Deltas, and Compute Vertex Positions are repeated over multiple passes in sequence as per the iterative Jacobi-style parallel solver algorithm.

Mesh Normals
~~~~~~~~~~~~

This shader computes mesh normals for lighting and normal maps.

Shading Pass
~~~~~~~~~~~~

Finally, the mesh is rendered as per the D3D12 API.

Frame Timing Breakdown
~~~~~~~~~~~~~~~~~~~~~~

On the left we break down each frame's total compute time into its constituent components for various square cloth mesh resolutions. On the right we show the percentage of total time (in ms) each compute shader takes for various mesh resolutions in a given frame. We see here that total compute time increases exponentially with mesh resolution, and that the bulk of this time regardless of mesh resolution is spent generating self-collision constraints and applying all constraints to the cloth mesh. The graph on the right demonstrates that self-collision time is minimized for a mesh resolution of 100 x 100. In this case, the time spent generating self-collision constraints is equal to the time spent applying all constraints. As expected, for higher mesh resolutions, self-collision compute time increases faster than all other compute passes. This fact highlights the importance of our spatial hashing acceleration step, as well as the utility of predictive constraints.

.. image:: ../../Source/Samples/3_ClothSim/Images/compute_time_bar.png
    :target: ../_images/compute_time_bar.png
    :alt: Compute Time Bar
     
.. image:: ../../Source/Samples/3_ClothSim/Images/percent_breakdown.png
    :target: ../_images/percent_breakdown.png
    :alt: Percentage Breakdown


----

Performance
^^^^^^^^^^^

All timing studies were perfomed using the following software/hardware:

OS: Windows 10\ :raw-html-m2r:`<br>`
CPU: i7 4790K\ :raw-html-m2r:`<br>`
GPU: NVIDIA RTX 2080

All compute passes launch with 512 threads per block. We do not use shared memory in any of our compute shaders.


.. image:: ../../Source/Samples/3_ClothSim/Images/Data.PNG
   :target: ../_images/Data.PNG
   :alt: Data


CPU vs GPU
~~~~~~~~~~

Including basic distance and bending constraints, we managed CPU frame rates upwards of 20 FPS. For a cloth model of equal complexity, we observe frame rates upwards of 400 FPS. We observed a compute-only framerate of 480 FPS for a 200x200 resolution cloth.

Total Compute Time vs Total Constraints Solved
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

As expected, total compute time increases with the total number of constraints solved, which increase with higher mesh resolutions.


.. image:: ../../Source/Samples/3_ClothSim/Images/total_constraints_time.png
   :alt: Total Constraints Time


Mesh Resolution vs Compute Time
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

As noted previously, compute time increases exponentially with increased mesh resolutions (lower is better).


.. image:: ../../Source/Samples/3_ClothSim/Images/compute_time_line.png
   :alt: Compute Timeline


Spatial Hash Grid Acceleration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Here we show compute time acceleration as a function of hash grid resolution. As noted above, self-collisions are the most computationally expensive pass in the compute pipeline. A tuned (optimized resolution) hash grid can speedup the total compute time by as much as 4x the total compute time without one.


.. image:: ../../Source/Samples/3_ClothSim/Images/grid_resolutions.png
   :alt: Grid Resolution Graph

*All timing values computed for a 200x200 cloth*

----

Additional Features
^^^^^^^^^^^^^^^^^^^

GLTF 2.0 Mesh Support
~~~~~~~~~~~~~~~~~~~~~

Flamenco currently supports GLTF 2.0 mesh loading. However, GLTF meshes do not currently work with cloth self-collisions turned on. We are currently working to enable this feature.


.. image:: ../../Source/Samples/3_ClothSim/Images/custom_cloth_mesh_1.gif
   :alt: Custom GLTF Meshes

*Flamenco Supports GLTF 2.0 Mesh Loading*

Our Constraints also work on custom meshes. We also built a vertex aliasing technique that prevents duplicate vertices from being simulated by the core PBD algorithm.

.. list-table::
   :header-rows: 1

   * - .. image:: ../../Source/Samples/3_ClothSim/Images/custom_cloth_mesh_0_9-0_6.gif
        :alt: GLTF Meshes Stiff
     
     - .. image:: ../../Source/Samples/3_ClothSim/Images/custom_cloth_mesh_0_6-0_1.gif
        :alt: GLTF Meshes Relaxed
     
   * - Distance Constraint = 0.9
     - Distance Constraint = 0.6

   * - Bending Constraint = 0.6
     - Bending Constraint = 0.1


Rendering Pipeline using PBR Shading
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1

   * - Albedo
     - Normals
     - Roughness
     - AO

   * - .. image:: ../../Source/Samples/3_ClothSim/Images/ClothDiffuse.PNG
        :target: ../_images/ClothDiffuse.PNG
        :alt: Diffuse
     
     - .. image:: ../../Source/Samples/3_ClothSim/Images/ClothNormal.PNG
        :target: ../_images/ClothNormal.PNG
        :alt: Normal
     
     - .. image:: ../../Source/Samples/3_ClothSim/Images/ClothRoughness.PNG
        :target: ../_images/ClothRoughness.PNG
        :alt: Roughness
     
     - .. image:: ../../Source/Samples/3_ClothSim/Images/ClothAO.PNG
        :target: ../_images/ClothAO.PNG
        :alt: AO
     


.. list-table::
   :header-rows: 1

   * - Final Frame

   * - .. image:: ../../Source/Samples/3_ClothSim/Images/ClothAll.PNG
        :alt: Final Frame Image
     


Meme Generator
~~~~~~~~~~~~~~

We've added a toggleable meme generator. Results are shown below.


.. image:: ../../Source/Samples/3_ClothSim/Images/default_shehzan_35x40_(0.8-0.7-0.3).gif
   :alt: Shehzan Boi


----

References
^^^^^^^^^^


.. [1] Jan Bender, Matthias Müller, and Miles Macklin, `Position-Based Simulation Methods in Computer Graphics <http://mmacklin.com/EG2015PBD.pdf>`_
.. [2] Chris Lewin, `Cloth Self-Collision with Predictive Contacts <https://media.contentapi.ea.com/content/dam/eacom/frostbite/files/gdc2018-chrislewin-clothselfcollisionwithpredictivecontacts.pdf>`_
.. [3] Marco Fratarcangeli and Fabio Pellacini, `A GPU-Based Implementation of Position Based Dynamics for Interactive Deformable Bodies <http://publications.lib.chalmers.se/records/fulltext/219708/local_219708.pdf>`_
.. [4] Matthias Müller, Bruno Heidelberger, Marcus Hennix, and John Ratcliff, `Position Based Dynamics <http://matthias-mueller-fischer.ch/publications/posBasedDyn.pdf>`_
.. [5] Matthias Teschner, Bruno Heidelberger, Matthias Müller, Danat Pomeranets, and Markus Gross, `Optimized Spatial Hashing for Collision Detection of Deformable Objects <http://matthias-mueller-fischer.ch/publications/tetraederCollision.pdf>`_
