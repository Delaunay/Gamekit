Experience
==========

Gamekit defines an the experience table to customize the level thresholds and level constraints.
By default the level thresholds follow the formula ``exp(((level - 15) ^ 3) * 0.007 + 7.2194)``

Lower level have a low difficulty, but the difficulty increase exponentially until it reaches
the medium levels, there the difficulty plateau and grows linearly until high levels are reached,
then the difficulty grows again exponentially.

.. image::  /_static/Abilities/ExperienceLevels.png


.. note::

   The experience level and formula are only provided as an example and are not created
   which any game design in need, you will need to come up with one for your game.


.. note::

   While it is customory to have the experience level to grow exponentially, we have to keep in mind
   that the resources of a high level should grow significantly as well making them more efficient
   at gathering experience.

   As such the effective difficulty of reaching higher level should not grow exponentially.

   i.e the difficulty increase but players are given the tools to overcome it with ease.

