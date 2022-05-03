Modding & DLC
=============

Modding with official support
-----------------------------

Mods as community made DLCs.
A compiled version of the editor can be distributed to enable the modding community.

See for now `UE4Mod`_

.. _UE4Mod: https://github.com/EpicGames/UGCExample


Modding without official support
--------------------------------

Even without the editor modding is still possible.

Use `funchook <https://github.com/kubo/funchook>`_,
because base types are known from the engine source, it is quite easy for someone
to hook their mod at runtime using such a library.

Usually, you will implement your mod as a game module implementing the ``FDefaultGameModuleImpl``
interface and force the engine to load the module as if it was an offical module.


.. code-block:: cpp

   // Hook yourself to the ``BeginPlay`` method
   SUBSCRIBE_METHOD(ACharacterPlayer::BeginPlay, [](auto& Scope, ACharacterPlayer* Player) {
       UCharacterPlayerExtension* Component = NewObject<UCharacterPlayerExtension>(Player, TEXT("RPG_PlayerComponent"));

       Component->RegisterComponent();
       Component->SetNetAddressable();
       Component->SetIsReplicated(true);
   });

.. note::

   Thanks for Satisfactory modding community for their amazing work


References
----------

.. [1] `Hooking utility <https://github.com/Delaunay/SML/blob/208d9e8021f4d662f08cf21f5d0d1e1c8f14dfea/Source/mod/hooking.h>`_
