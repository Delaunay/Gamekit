Landscape Creation
==================

* `terrain.party`_ gives a square height map of 60x60 to 8x8 km2
* `tangrams`_ gives a heightmap that fills your screen

.. image:: /_static/height_map.png

.. note::

   terrain.party often timeout


Tangrams
~~~~~~~~

1. Download a height map that fills your browser screen, save the heightmap range ``range=(lowest - heighest)``
2. If your heightmap is big, split it into multiple quadrants, the names must follow the convention ``<name>_x0_y0`` with ``0 x 0`` is the center of your image.
3. Exports your heightmap as 16bit gray scale (PNG or RAW)
4. Create a new level
5. Enable landscape mode
6. Import from file
7. Compute the z-Scale


.. code-block:: python

   # increase the range if you want to account for sea level
   range (cm) = range (m) * 100
   z_scale = range (cm) * 0.001953125        # (0.001953125=1/512)

.. note::

   To get a square heightmap you may use your
   browser developer tools to mock the desired screen size


.. note::

   One pixel of your height map is equal to 1 meter in Unreal Engine


.. note::

   The URL contains the lat/long of the pixel at the center.
   Using your browser developer tools you can query the size in meters ``scene.view.size.meters``

   .. code-block:: javascript

      scene.view.size.meters
      Object { x: 5243.083422156026, y: 5243.083422156026 }


References
----------

* UE4 Technical `guide`_ to Landscapes


.. _tangrams: https://tangrams.github.io/heightmapper/
.. _terrain.party: https://terrain.party/
.. _guide: https://docs.unrealengine.com/4.27/en-US/BuildingWorlds/Landscape/TechnicalGuide/
