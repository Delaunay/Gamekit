Fog Of War
==========

Fog of war have an important impact on the overall game in a multiplayer setting.
Indeed, the fog of war brings a new dimension to the gameplay, that new dimensions needs
to be protected from cheater that might reveal hidden part of the map.
In order to do so, replication needs to be tweak for all actors participating in the fog of war.

Relevant classes:

* :cpp:class:`AGKFogOfWarVolume`: Top level Actors that manages all the textures and components
* :cpp:class:`UGKFogOfWarComponent`: Simple actor component that is added to each actors participating in the fog of war (drawing & blocking)
* :cpp:class:`UGKFogOfWarStrategy`: Implements a drawing the line of sight algorithm
* :cpp:class:`UGKUpscalerStrategy`: Implements a upscaling algorithm for the fog of war texture.
  It can be used to implement post processing/filters on the texture itself before it gets display on the scene

.. image :: /_static/FogOFWar_System.png

Features
--------

* Multi Faction
* Multi Actors
* Many customization & performance parameters to try
* Automatic PostProcessingMaterial setup on the controlled pawn's camera
* Upscaler
* Mutliple fog of war drawing trategy
* Sample post process materials
* Customizable Components
* Support spectator fog (multi fog view)
* Replication friendly


.. image :: /_static/FogOFWarOverview.PNG


In a nutshell
-------------

1. Create a ``AGKFogOfWarVolume`` over the area that will be covered by the fog

   1. [Optional] Set the post process material

   2. [Optional] Set the fog of war material parameters

   3. [Optional] Disable/Enable exploration

   4. [Optional] Select Vision drawing strategy

   5. [Optional] Disable/Enable Upscaling strategy

      1. Set the upscaling material

2. Add ``UGKFogOfWarComponent`` to every actors that will participate in the fog of war (including blocking)

   1. Call ``UGKFogOfWarComponent::SetCameraPostprocessMaterial`` to add the post process material to our camera
      to render the fog

   2. Update materials to make sure out of sight units are not rendered.

   3. Configure the actor' sight settings

Multiplayer
-----------

* ``AActor::IsNetRelevantFor`` needs to be update to only flag actor as relevant if visible
* The set of visible units is produced by the server and replicated to the clients.
  Clients can only access the visible set of their faction.
* Spectators receive all the visible sets

.. image :: /_static/FogOfWar/FogReplication.png

Limitations
~~~~~~~~~~~

* If used in multiplayer the number of teams is limited to 32.
  This is because a `uint32` is used to represent the visibility of a unit.
  Each bit representing the unit visiblity by a given team.
  This is used to compute conditional replication to avoid replicating
  actors that are not visible, this is both to limit bandwidth usage and
  prevent cheating.


The limit could be increased further to support 64 teams by using a uint64 but
it seems unnecessary.

