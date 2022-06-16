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

Gamekit automatically defines base gametags for its abilities.

* Ability: use to tag abilities
	* Exclusive: Used to prevent activation of multiple abilities witht the `Exclusive` tag
	* Move: Used to tag abilities performing movement operation
	* Passive: tag passive abilities

* Animation: used to tag animation
	* CastPoint: Tag is sent when the animation reach the point where projectile/effect should be
	  spawned.

* Cooldown: used to tag cooldowns; to prevent the same ability from being casted all the time

* Buff: Improves the effectiveness of a character
	* Immunity: protects against debuff and disables
	* Armor increase
	* Damage
	* Health bonus
	* Haste: increase movement speed
	* Level: level of the buff
		* Minor
		* Medium
		* Strong

* Debuff: reduce the effectiveness of a character
	* Level: level of the debuff
		* Minor
		* Medium
		* Strong
	* Armor reduction
	* Health Penalty
	* Damage decrease

* Disable: prevent character for performing certain actions
	* Level: level of the disable
		* Minor
		* Medium
		* Strong
	* Stun: prevents movement, spell casting and item usage
	* Silence: prevents spell casting
	* Mute: prevent item usage
	* Root: prevent all movements
	* Break: disable passive abilities
	* Dead: removes all buffs, debuffs and disables
		* Death is both a disable and a dispel, on depth all active effects are removed
		  but all actions are disabled.
	* Etheral: cannot do physical damage, cannot be damaged by physical attacks [TODO]
	* Disarmed: cannot do physical attacks [TODO]

* Dispel: removes debuff and disables
	* Level: level of the dispel
		* Minor
		* Medium
		* Strong

* Failure: used to report errors back to the users
	* Blocked:  Ability is blocked by a disable
	* Cost: Resource level does not meet the ability cost
	* Missing:  Ability has missing requirement
	* Network: Internal error due to networking
	* NotLearned: Ability was not learned yet
	* Cooldown: Ability is still in cooldown
	* Dead: Character is dead

* GameplayCue: used to spawn effect in reseponse to gameplay effects, all disables will have an effect so players can see it easily
	* Immunity
	* Stun
	* Silence
	* Mute
	* Root
	* Break

* Quest: used to tag objectives for the quest system

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


Ability Kinds
-------------

MaxLevel = HeroLevel / 2
UltimateMaxLevel = HeroLevel / 6

* Standard Ability: 4 Levels, Every 2 level character increase their maximum basic level ability by 1
	*

* Ultimate Ability: 3 Levels, every 6 level

* Unlockable Abilities: Ability is blocked until a tag is granted, unlock tag is granted
* Abilities with charges: Ability consume charges, charges are given by an effect periodically
* Linked Abilities [TODO]
* Sub abilities [TODO]
* Passive abilities
* Autocast abilities [TODO]


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

.. figure:: /_static/btn_cooldown.PNG
   :alt: Ability under cooldown
   :width: 60

   Ability under cooldown

.. figure:: /_static/btn_disabled.PNG
   :width: 60

   Ability disabled

.. figure:: /_static/btn_normal.PNG
   :width: 60

   Ability ready

.. figure:: /_static/btn_targetting.PNG
   :width: 60

   Ability selecting a target

.. figure:: /_static/btn_oom.PNG
   :width: 60

   Ability out of mana

.. figure:: /_static/btn_mute.png
   :width: 60

   Ability disabled by gameplay effect


UI & Ability State
^^^^^^^^^^^^^^^^^^

The easiest way to bind the ability to a UI element is to use async task that will wait on events

* :cpp:class:`UGKAsyncTaskCooldownChanged`: can be used to keep track of the cooldowns.
* :cpp:class:`UGKAsyncTaskAttributeChanged`: can be used to listen to energy level and detect when an ability lacks its resources to be cast.
* :cpp:class:`UGKAsyncTask_GameplayEffectChanged`: can be used to listen to debuff that will disable spell casting.


Builtin Abilities
-----------------

* Base Ability: Basic ability with activation logic
* Cancel Ability: Cancel all the current abilities
* Move Ability: Move to destination or Actor
* Base Skill: Ability that is disabled when silenced
* Base Item: Ability that is disabled when muted
	* with an optional item slot (Gloves, ring, etc...)

Builtin Effects
---------------

* Cooldown
* Death: Remove all active effects and set health to zero
* Dispel: Remove all debuffs and disables
* Heal
* HealOvertime
* Damage
* DamageOvertime
* Immunity: Grant immunity against all debuffs and disables
* IncreaseHealth
* ManaCost
* MoveHaste
* Root
* Silence
* Stun

Builtin Attribute Set
---------------------

* Health
* Mana
* Experience
* Gold



Resources
~~~~~~~~~

.. [1] Gameplay Ability System `GAS <https://github.com/tranek/GASDocumentation>`_.
