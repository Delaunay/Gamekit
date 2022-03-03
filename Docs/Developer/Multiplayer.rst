Multiplayer
===========


Server Only code blocks
-----------------------

.. code-block:: cpp

   #if WITH_SERVER_CODE

   // code to load up your mananger and stuff here

   #endif


Actor Network configuration
---------------------------

.. code-block:: cpp

   AActor::GetNetMode()    // Standalone | Dedicated  | Listen | Client
   AActor::GetLocalRole()  // Simulated  | Autonomous | Authority
   AActor::GetRemoteRole() // Simulated  | Autonomous | Authority


Conditionnal execution of some code
-----------------------------------

.. code-block:: cpp

   // HasAuthority in blueprints:
   HasAuthority() = AActor::GetLocalRole() == ROLE_Authority);

   // CanExecute Cosmetic Events
   //    Used when working with HUD/Widget
   !UKismetSystemLibrary::IsDedicatedServer(World)


Common Actor Configurations
---------------------------

+------------------------------+------------+------------+-------------+
| Actor                        | Net Mode   | Local Role | Remote Role |
+------------------------------+------------+------------+-------------+
| Hosting Client               | Listen     | Authority  | Simulated   |
+------------------------------+------------+------------+-------------+
| Remote Clients on Server     | Dedicated  | Authority  | Simulated   |
+------------------------------+------------+------------+-------------+
| Remote Clients (predicted)   | Client     | Simulated  | Authority   |
+------------------------------+------------+------------+-------------+
| Local Client (no prediction) | Client     | Autonomous | Authority   |
+------------------------------+------------+------------+-------------+
| Static Actor                 | Client     | None       | Authority   |
+------------------------------+------------+------------+-------------+
| Local Play                   | Standalone | Authority  | Simulated   |
+------------------------------+------------+------------+-------------+
