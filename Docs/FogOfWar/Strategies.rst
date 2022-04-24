Drawing Strategies
==================

:cpp:class:`AGKFogOfWarVolume` implements different strategies that implements the fog of war drawing.
The default implementation is :cpp:class:`UGKShadowCasting`, other strategies are provided for their
educational value but should not be used in production.

Implementations
---------------

All fog implementations inherit from :cpp:class:`UGKFogOfWarStrategy`.
Drawing the fog is half of the problem, has you will need to be able to query
the fog to know if a unit from Team X is visible by Team Y for replication purposes.
If the feature is not implemented, clients will receive replication updates for hidden units
and cheater could use the data to gain an unfair advantage.


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
|             |                     | * No Mutiplayer               |
|             |                     |                               |
+-------------+---------------------+-------------------------------+
| RayCast V2  | * Less Rays than V1 | * 2x Jittery                  |
|             |                     | * Slow                        |
|             |                     | * No Mutiplayer               |
|             |                     |                               |
+-------------+---------------------+-------------------------------+
| RayCast V3  | * Less Rays than V1 | * 2x Jittery                  |
|             | * Better defined    | * Slow                        |
|             |   Shadows that V2   | * No Mutiplayer               |
|             |                     |                               |
|             |                     |                               |
+-------------+---------------------+-------------------------------+
| Discrete V1 | * Fast              | * 1x Jittery                  |
|             | * CPU Texture       |                               |
| Shadow      | * Easy to Query     |                               |
| Casting     | * Multiplayer       |                               |
|             |                     |                               |
+-------------+---------------------+-------------------------------+


Transforms (Exploration & Upscaling)
------------------------------------

:cpp:class:`UGKTransformerStrategy` can be used to implement post processing steps on the fog textures, to
tweak its final look.

Exploration & Upscaling are both implemented using :cpp:class:`UGKTransformerStrategy`.


Idea
----

* Change the drawing backend per actors
  * Make client use a prettier/slower implementation
  * Server use faster implementation


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