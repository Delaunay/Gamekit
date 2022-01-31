Abilities
=========

Relevant classes:

* :class:`UGKGameplayAbility`: basic Gameplay Ability for skills and items, implements a few top down casting options
* :class:`FGKAbilityStatic`: configurable settings for abilities
* :class:`AGKAbilityTarget_Actor`: used to select a target for a given ability
* :class:`UGKAbilitySystemComponent`
* :class:`UGKCastPointAnimNotify`: used to notify the ability when the animation reached a given point
* :class:`UGKAttributeSet`: defines a set of attributes for characters (Health, Mana, Stamina, etc...)
* :class:`UGKAbilityWidget`: defines a basic interface for widget representing an ability state

Features
--------

* **Data-Driven**: Ability can generate their own GameplayEffect at runtime
* **Generic**: Ability does not assume a specific Character/Skeleton
* **Multiplayer ready**
* **Basic UI**: easier debugging

Overview
--------

Ability DataTable Initialization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. image:: /_static/AbilityDataDriven.png


Ability Activation Flow
^^^^^^^^^^^^^^^^^^^^^^^

.. image:: /_static/AbilityActivationFlow.png


Ability Replication Flow
^^^^^^^^^^^^^^^^^^^^^^^^

.. image:: /_static/AbilityReplicationFlow.png

Animations
^^^^^^^^^^

To make abilities generic, the animations montage themselves are not specified.
Instead an animation kind is set (Channelling, Attack, Cast, etc..) which is a single enumerator
:cpp:enumerator:`EGK_AbilityAnimation` representing the kind of annimation an ability can use.

The animation montage are specified by the Character itself through its :cpp:class:`FGKAnimationSet` (inside :cpp:class:`FGKUnitStatic`)
which our ability can sample from.

When an ability is activated the :cpp:class:`FGKAnimationSet` is fectched from the character to the ability
and the right animation is then played. This enable us to use the same ability for different
characters which can have different animation as well.


.. image:: /_static/AbilityAnimations.png

.. note::

   Setup your Gameplay Ability globals inside DefaultGame.ini to get failure tags.

   .. code-block:: ini

      [/Script/GameplayAbilities.AbilitySystemGlobals]
      ActivateFailIsDeadName=State.Dead
      ActivateFailCooldownName=Cooldown
      ActivateFailCostName=Failure.Cost
      ActivateFailTagsBlockedName=Failure.Blocked
      ActivateFailTagsMissingName=Failure.Missing
      ActivateFailNetworkingTag=Failure.Network

   in DefaultGameplayTags.ini

   .. code-block:: ini

      [/Script/GameplayTags.GameplayTagsSettings]
      ImportTagsFromConfig=True
      WarnOnInvalidTags=True
      FastReplication=False
      InvalidTagCharacters="\"\',"
      NumBitsForContainerSize=6
      NetIndexFirstBitSegment=16
      +GameplayTagList=(Tag="AbilityName",DevComment="")
      +GameplayTagList=(Tag="AbilityName.Attack",DevComment="")
      +GameplayTagList=(Tag="Animation.Play",DevComment="")
      +GameplayTagList=(Tag="Cooldown",DevComment="")
      +GameplayTagList=(Tag="Cooldown.Attack",DevComment="")
      +GameplayTagList=(Tag="Cooldown.Skill1",DevComment="")
      +GameplayTagList=(Tag="Debuff.Stun",DevComment="")
      +GameplayTagList=(Tag="Dispel.Death",DevComment="")
      +GameplayTagList=(Tag="Failure.Blocked",DevComment="")
      +GameplayTagList=(Tag="Failure.Cost",DevComment="")
      +GameplayTagList=(Tag="Failure.Missing",DevComment="")
      +GameplayTagList=(Tag="Failure.Network",DevComment="")
      +GameplayTagList=(Tag="State.Dead",DevComment="")


User Interface
--------------

Spell Button States
^^^^^^^^^^^^^^^^^^^

See :class:`UGKAbilityWidget` for a basic set of implementable event

Here is a list of states you might want to consider when implementing
a spell button.

#. Ability is available and can be casted (UMG State Normal)

#. Ability is picking a target

#. Ability is being cast (Animation)

#. Ability is on cooldown

#. Backswing animation

#. Ability requirements/cost are not met

#. Ability cannot be cast because of debuf

.. image:: /_static/btn_cooldown.PNG
   :width: 60

.. image:: /_static/btn_disabled.PNG
   :width: 60

.. image:: /_static/btn_normal.PNG
   :width: 60

.. image:: /_static/btn_targetting.PNG
   :width: 60

.. image:: /_static/btn_oom.PNG
   :width: 60

.. image:: /_static/btn_mute.png
   :width: 60


UI & Ability State
^^^^^^^^^^^^^^^^^^

The easiest way to bind the ability to a UI element is to use async task that will wait on events

* :cpp:class:`UGKAsyncTaskCooldownChanged`: can be used to keep track of the cooldowns.
* :cpp:class:`UGKAsyncTaskAttributeChanged`: can be used to listen to energy level and detect when an ability lacks its resources to be cast.
* :cpp:class:`UGKAsyncTask_GameplayEffectChanged`: can be used to listen to debuff that will disable spell casting.


Resources
~~~~~~~~~

#. Gameplay Ability System `GAS <https://github.com/tranek/GASDocumentation>`_.
