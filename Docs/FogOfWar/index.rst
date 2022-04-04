Fog Of War
==========

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


Limitations
-----------

* If used in multiplayer the number of teams is limited to 32.
  This is because a `uint32` is used to represent the visibility of a unit.
  Each bit representing the unit visiblity by a given team.
  This is used to compute conditional replication to avoid replicating
  actors that are not visible, this is both to limit bandwidth usage and
  prevent cheating.


The limit could be increased further to support 64 teams by using a uint64 but
it seems unnecessary.

Versions
--------

RayCast V1
~~~~~~~~~~

.. note::

   RayCast is using a custom collision channel, it is expected to be using ``ECC_GameTraceChannel1`` by default,
   the exact collision channel is customizable using the Collision trace property in ``FogOfWarVolume``

   The Collision channel ``FoWObstacle`` is used to control if the actor is blocking vision or not,
   by default all actors block visions.

   .. code-block:: ini

      [/Script/Engine.CollisionProfile]
      +DefaultChannelResponses=(Channel=ECC_GameTraceChannel1,DefaultResponse=ECR_Block,bTraceType=True,bStaticObject=False,Name="FoWObstacle")


Draws ``n`` rays, draw lines of a given width on the Fog of War texture.

.. |V1_36_Render| image:: /_static/FogOfWar_RayCast_V1_36Rays_RenderTexture.PNG

.. |V1_36_View| image:: /_static/FogOfWar_RayCast_V1_36Rays.PNG

.. |V1_720_Render| image:: /_static/FogOfWar_RayCast_V1_720Rays_RenderTexture.PNG

.. |V1_720_View| image:: /_static/FogOfWar_RayCast_V1_720Rays.PNG

.. |V1_360_Render| image:: /_static/FogOfWar_RayCast_V1_360Rays_Tickness1_RenderTexture.PNG

.. |V1_360_View| image:: /_static/FogOfWar_RayCast_V1_360Rays_Thickness1.PNG

+----------+------+---------------+-----------------+
| Tickness | Rays |  Render       | Fog Texture     |
+----------+------+---------------+-----------------+
|        2 |  36  | |V1_36_View|  | |V1_36_Render|  |
+----------+------+---------------+-----------------+
|        1 | 360  | |V1_360_View| | |V1_360_Render| |
+----------+------+---------------+-----------------+
|        2 | 720  | |V1_720_View| | |V1_720_Render| |
+----------+------+---------------+-----------------+

RayCast V2
~~~~~~~~~~

Draws ``n`` rays, draw triangles with a material.

.. |V2_36_Render| image:: /_static/FogOfWar_RayCast_V2_36Rays_RenderTexture.PNG

.. |V2_36_View| image:: /_static/FogOfWar_RayCast_V2_36Rays.PNG

.. |V2_12_Render| image:: /_static/FogOfWar_RayCast_V2_12Rays_RenderTexture.PNG

.. |V2_12_View| image:: /_static/FogOfWar_RayCast_V2_12Rays.PNG

+------+---------------+----------------+
| Rays |  Render       | Fog Texture    |
+------+---------------+----------------+
|   12 | |V2_12_View|  | |V2_12_Render| |
+------+---------------+----------------+
|   36 | |V2_36_View|  | |V2_36_Render| |
+------+---------------+----------------+


RayCast V3
~~~~~~~~~~

For each actors in vision range trace 4 rays; 2 Left/right rays, one that misses and one that hits

.. |V3_12_Render| image:: /_static/FogOfWar_RayCast_V3_12Rays_RenderTexture.PNG

.. |V3_12_View| image:: /_static/FogOfWar_RayCast_V3_12Rays.PNG

+------+---------------+----------------+
| Rays |  Render       | Fog Texture    |
+------+---------------+----------------+
|   12 | |V3_12_View|  | |V3_12_Render| |
+------+---------------+----------------+

Discrete Tiles V1
~~~~~~~~~~~~~~~~~

Split the world in square tiles & draw the line of sight.

.. |V1_Discrete_Render| image:: /_static/FogOfWar_ShadowCasting_V1_RenderTexture.PNG

.. |V1_Discrete_Render_Upscaled| image:: /_static/FogOfWar_ShadowCasting_V1_Upscaled.PNG

+----------+-------------------------------+-------------------------------+
| Upscaled |  Render                       | Fog Texture                   |
+----------+-------------------------------+-------------------------------+
|   Yes    | |V1_Discrete_Render_Upscaled| | |V1_Discrete_Render|          |
+----------+-------------------------------+-------------------------------+


Comparison
~~~~~~~~~~

+-------------+---------------------+-------------------------------+
| Strategy    |  Pros               | Cons                          |
+-------------+---------------------+-------------------------------+
| RayCast V1  |                     | * x 2 Slow                    |
|             |                     |                               |
+-------------+---------------------+-------------------------------+
| RayCast V2  | * Less Rays than V1 | * 2x Jittery                  |
|             |                     | * Slow                        |
|             |                     |                               |
+-------------+---------------------+-------------------------------+
| RayCast V3  | * Less Rays than V1 | * 2x Jittery                  |
|             | * Better defined    | * Slow                        |
|             |   Shadows that V2   |                               |
|             |                     |                               |
+-------------+---------------------+-------------------------------+
| Discrete V1 | * Fast              | * 1x Jittery                  |
|             | * CPU Texture       |                               |
| Shadow      | * Easy to Query     |                               |
| Casting     |                     |                               |
+-------------+---------------------+-------------------------------+


Exploration
-----------

Exploration is managed through another render target which has the current vision added on every FoWVolume tick.


Line of Sight
-------------

The fog of war can be used for stealth games; line of sights are cast to determine which part of the map is visible.

Idea
----

I thought I could improve my fog of war by using a point light for the tracing and draw the point light on a render target but it does not seem possible in UE4.
There are 3 light channels but they cant be rendered separately or I have not found a way to do so


.. note:: Decal Rendering

   A decal Component on the :cpp:class:`AGKFogOfWarVolume` can be used to render the fog of war on screen instead of using a post processing step.
   This method is not advised because it might adversly impact other part of the game that are using decals (cursor, etc..)

   Additionally in the case of a game with factions it is easier to tweak the ``CameraComponent|PostProcessingMaterial`` to use the faction fog of war than to
   modify the globally unique :cpp:class:`AGKFogOfWarVolume`.

.. note::

   UnrealEngine implements ``UAISense_Sight`` (`doc`_) which requires sightable targets to implement ``IAISightTargetInterface``.
   The implementation is different an tries to limit the number of trace done.
   As a result the full line of sight is not drawn. It might be a path worth investigating if you find ``AGKFogOfWarVolume``
   to be too expensive.


.. _doc: https://docs.unrealengine.com/4.26/en-US/API/Runtime/AIModule/Perception/UAISense_Sight/