Multiplayer
===========


.. code-block::

   GetGameInstance()->IsDedicatedServerInstance()



   #if WITH_SERVER_CODE

   // code to load up your mananger and stuff here

   #endif

   AActor::GetNetMode() with NM_DedicatedServer

   AActor::GetLocalRole()