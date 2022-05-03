Procedural Level Generation
===========================

Discrete Level Generation
-------------------------

The level is split into discret chunks, the prodecural process will assign each chunk a mesh/entity to be spawned.

A cheap representation of the level is used to speed up the generation process.
Gamekit implements a ``T3DArray<T>`` that can be used as such representation.

From the cheap representation we can generate a ``(position, class)`` pair that can be used to spawn
the required meshes to build the level.

.. note::

   Instead of spawning an ``AActor`` or a ``AStaticMeshActor`` into the level you can add a ``UStaticMeshComponent``
   to the spawning actor (the level actor) this will avoid the extra overhead of spawning a full actor.


.. image:: /_static/StaticProcedural.PNG


.. code-block:: cpp

   T3DArray<int> grid(10, 10);

   for(int i = 0; i < 10; i++) {
       grid(0, i) = WALL;
       grid(i, 0) = WALL;
       grid(0, 9) = WALL;
       grid(9, 0) = WALL;
   }


.. seealso:: :cpp:func:`UGKMazeGeneration::RandomWall` :cpp:func:`UGKMazeGeneration::RandomizedDepthFirstSearch`

Landscape Generation
--------------------

Lanscape can be generated from a height map which can be a regular image.
To make a complex landscape multiple layers of maps can be used for different entity.

For that kind of image generation you will want to use `GP-GPU compute capabilities <https://developer.nvidia.com/gpugems/gpugems3/part-i-geometry/chapter-1-generating-complex-procedural-terrains-using-gpu>`_
to parallelize the generation process as much as possible.


.. note::

   You can get a height map of the real world using `tangrams`_ or `terrain.party`_.


.. image:: /_static/height_map.png


.. _tangrams: https://tangrams.github.io/heightmapper/
.. _terrain.party: https://terrain.party/



References
----------

.. [1] `Procedural Generation <https://www.iquilezles.org/www/articles/morenoise/morenoise.htm>`_
.. [2] `The Book of Shaders <https://thebookofshaders.com/13/>`_
.. [3] `TerraForge3D <https://github.com/Jaysmito101/TerraForge3D>`_