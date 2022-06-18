Ability Design
==============

.. danger::

   The defaults provided by Gamekit should be considered placeholders.
   They **try** to follow current trend in the industry of what is usually done.
   Nevertheless, to give you game a soul it will need you to put much though into how
   your game is going to feel.

   This document simply discuss the pros of current decision so you can decide on the best way
   to tweak the system to your game idea.


* Skill Abilities are exclusive
    * When a skill is activated, it cannot be canceled implicitly and it prevents more skill abilities from being casted
    * This is a QOL feature, which prevent players from cancelling their own ability by casting another one too quickly.
      Cancelling is still possible by pressing the explicit cancel button and casting the right skill

* Movement abilities are not exclusive
    * They get canceled as soon as the skill ability was able to select a target to walk towards to

* Experience level difficulty follows a (x^3) starting levels are very easy but grow exponentially until
  it plateau for medium levels and grows linearly until higher levels are reached then the difficulty grow exponentially again.
    * First few levels are tutorials, they can pass fairly quickly
    * Medium levels are were most of the time will be spent, slowly but steadly improving
    * high level becomes increasinling hard towards the end to let player time to learn the ropes regarding to competitive gameplay elements

.. note::

   While it is customory to have the experience level to grow exponentially, we have to keep in mind
   that the resources of a high level player should grow significantly as well making them more efficient
   at gathering experience.

   As such the effective difficulty of reaching higher level should not grow exponentially.

   i.e the difficulty increase but players are given the tools to overcome it with ease.
