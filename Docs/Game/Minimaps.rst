Minimaps
========

Relevant classes:

* :cpp:class:`AGKMinimapVolume`
* :cpp:class:`UGKMinimapComponent`

.. image :: /_static/Minimap/MinimapVolume.png

The minimap volume creates a SceneCaptureComponent that generates an image of the minimap.
The volume is used to set the size of the orthographic view.

In addition, it periodically iterates through all the MinimapComponent and draw them
on a separate texture.

The different layers are combined in a single material giving us the final look.

The example above combines 5 textures

* Fog of war line of sight
* Fog of war exploration
* Minimap Capture
* Minimap Component Drawings
* Controller Field of View


.. note::

   Minimaps are ignored for dedicated server builds.


Radar
----


Minimaps that display around the player