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

Setup
-----

GameTags
^^^^^^^^

Gamekit Tags:

* AnimationCastPointName: Tag sent when animation reach the cast point
* DeathTag: Tag set on death
* DeathDispelTag: Speciall dispel flags set on death (removes all debuffs)
* ActivateFailNotYetLearnedName: Ability has not been learn yet

* Buff
	* Immunity
	* Level
		* Minor

* Debuff
	* Level
	* Stun
	* Silence
	* Mute
	* Root
	* Break

* Dispel
	* Level

* Failure


Unreal Engine GA tags

* ActivateFailIsDeadName: Character is dead
* ActivateFailCooldownName: Ability is till in cooldown
* ActivateFailCostName: Resource level does not meet the ability cost
* ActivateFailTagsBlockedTag: Ability is blocked by a debuff
* ActivateFailTagsMissingName: Ability has missing requirement
* ActivateFailNetworkingName: Internal error due to networking

You can set those tags in `DefaultGame.ini`.

.. code-block:: ini

	[/Script/GameplayAbilities.AbilitySystemGlobals]
	ActivateFailIsDeadName=State.Dead
	ActivateFailCooldownName=Failure.Cooldown
	ActivateFailCostName=Failure.Cost
	ActivateFailTagsBlockedName=Failure.Blocked
	ActivateFailTagsMissingName=Failure.Missing
	ActivateFailNetworkingName=Failure.Network
	ActivateFailNotYetLearnedName=Failure.NotLearned
	DeathDispelName=Dispel.Death
	DeathName=State.Dead

   GlobalCurveTableName=CompositeCurveTable'/Game/Data/CurveDatabaseCache.CurveDatabaseCache'


Define Abilities
----------------

Abilities are define inside a json file.
The list of customizable properties can be found here :class:`FGKAbilityStatic`

.. code-block:: json

	{
		"Name": "Fireball",
		"AbilityKind": "Skill",
		"LocalName": "NSLOCTEXT(\"[0A85C43C484A243EF7B6B7B642AD2ACB]\", \"EC9BA0DD417F0C07C07E3DBB00F88785\", \"Fire Ball\")",
		"LocalDescription": "NSLOCTEXT(\"[0A85C43C484A243EF7B6B7B642AD2ACB]\", \"9CB2744E447BF19F751B929142E6F484\", \"Launch a roaring ball of fire in a straight line\")",
		"Icon": "Texture2D'/Gamekit/Textures/IconsSkills/fireball-red-1.fireball-red-1'",
		"MaxLevel": 4,
		"Duration": 0,
		"AbilityEffects":
		{
		},
		"Cost":
		{
			"Attribute":
			{
				"AttributeName": "Mana",
				"Attribute": "/Script/Gamekit.GKAttributeSet:Mana",
				"AttributeOwner": "Class'/Script/Gamekit.GKAttributeSet'"
			},
			"Value": [ 10, 9, 8, 7 ]
		},
		"Cooldown": [ 0.5, 9, 8, 5 ],
		"Price": 0,
		"MaxStack": 1,
		"AreaOfEffect": 50,
		"CastMaxRange": 500,
		"CastMinRange": 0,
		"AbilityBehavior": "PointTarget",
		"AbilityTargetActorClass": "BlueprintGeneratedClass'/Game/Abilities/TargetActors/ControllerTrace.ControllerTrace_C'",
		"TargetObjectTypes": [],
		"CastTime": 0.20000000298023224,
		"ChannelTime": 0,
		"AbilityAnimation": "Attack",
		"ProjectileActorClass": "BlueprintGeneratedClass'/Game/Abilities/Projectiles/BP_GA_Projectile.BP_GA_Projectile_C'",
		"ProjectileSpeed": 1000,
		"ProjectileBehavior": "Directional",
		"ProjectileRange": 1600,
		"AOEActorClass": "None"
	}


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

#. Ability cannot be cast because of debuff

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

.. [1] Gameplay Ability System `GAS <https://github.com/tranek/GASDocumentation>`_.
