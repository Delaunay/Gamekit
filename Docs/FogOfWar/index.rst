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

* Multi Team (uses :cpp:class:`IGenericTeamAgentInterface`)
* Multi Actors
* Many customization & performance parameters to try
* Simple PostProcessingMaterial setup on the controlled pawn's camera
* Upscaler
* Post process materials examples
* Customizable components
* Support spectator fog (multi fog view)
* Replication friendly

.. image :: /_static/FogOFWarOverview.PNG


Get Started
-----------

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

* ``AActor::IsReplicationPausedForConnection`` needs to be updated to only enable
  replication for actors that are visible to the player controller's team.
  :cpp:class:`AGKCharacterBase` provide a default implementation.
* The set of visible units is produced by the server and replicated to the clients.
  Clients can only access the visible set of their faction.
* Spectators receive all the visible sets

.. image :: /_static/FogOfWar/FogReplication.png


Memory Usage
------------

The map is split in tiles, there are 2 permanent layers

* Terrain
* Blocking

By Default 3 teams are defined (Neutral, GoodGuys, BadGuys).

* Memory Usage
  * CPU: (MapSize / TileSize) * (#Team + 2)
  * GPU: (MapSize / TileSize) * #Team * UpscalingMutiplier

.. note::

   ``NoTeam`` is its own separate concept, they will not be registered
   to the fog of war (but they can be registered as obstacle).

Examples
~~~~~~~~

* Map Size 400'000 x 400'000 (400x400 meters)
* Grid Size: 16 x 16 x 32
* RAM usage per team = (400000/16) ^ 2 / 1024 / 1024 ~= 596 Mio

* Map Size 400'000 x 400'000 (400x400 meters)
* Grid Size: 32 x 32 x 32
* RAM usage per team = (400000/32) ^ 2 / 1024 / 1024 ~= 149 Mio

.. note::

   Image is gray scale, using colors would multuple the memory usage by 3 or 4 with an alpha channel.


