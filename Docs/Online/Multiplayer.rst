Multiplayer
===========

Common Actor Configurations
~~~~~~~~~~~~~~~~~~~~~~~~~~~

#. Local Play: No server, one player
#. Hosting Client: Host is both a server & and a client
#. Dedicated Server: no player on the instance, all are connected remotely to the server
#. Remote Client: player connected to a server (Hosting Client or dedicated server)
#. Predicted Client: Remote player simulated on another client

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


About Servers
~~~~~~~~~~~~~

The games are truely only running on the server. Clients forwards their inputs to the server
and render the state of the game. The state of the game between the clients and the server is
kept in sync through replication.

To limit bandwidth usage so actions will get simulated on clients, for example when moving
a character from A to B, there is no need to replicated the position of the character on each frame.
Instead, we can interpolate its location given its current location, destination and speed.

Bad network condition will hinder the synchronisaton which can cause jittering.
Character will move beyond their target, or the actions of other players will not be reflected.


Code Snippet
~~~~~~~~~~~~

Server Only code blocks
-----------------------

.. code-block:: cpp

   #if WITH_SERVER_CODE

   // code to load up your mananger and stuff here

   #endif


Client only code blocks
-----------------------

.. code-block:: cpp

   #if !UE_SERVER

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


