Multiplayer
===========

* How classes are used inside a multiplayer game

+------------------------------+----------+------------+
| Class                        | Location | Replicated |
+------------------------------+----------+------------+
| GameMode (Match State API)   | Server   | No         |
+------------------------------+----------+------------+
| GameModeBase (No Match)      | Server   | No         |
+------------------------------+----------+------------+
| GameSession                  | Server   | No         |
+------------------------------+----------+------------+
| GameNetwork Manager          | Server   | No         |
+------------------------------+----------+------------+
| GameStateBase                | Server   | All        |
+------------------------------+----------+------------+
| GameState                    | Server   | All        |
+------------------------------+----------+------------+
| PlayerState                  | Server   | All        |
+------------------------------+----------+------------+
| PrivatePlayerState (Gamekit) | Server   | Teammates  |
+------------------------------+----------+------------+
| TeamVision (Fog Of War)      | Server   | Teammates  |
+------------------------------+----------+------------+

GameInstance

* ACharacter are setup for replications

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

To limit bandwidth usage some actions will get simulated on clients, for example when moving
a character from A to B, there is no need to replicate the position of the character on each frame.
Instead, we can interpolate its location given its current location, destination and speed.

Bad network condition will hinder the synchronisaton which can cause jittering.
Character will move beyond their target, or the actions of other players will not be reflected.


Code Snippet
~~~~~~~~~~~~

.. code-block:: cpp

   HasAuthority           = GetNetMode() != ENetMode::NM_Client
   ShouldShowUI           = GetNetMode() != ENetMode::NM_DedicatedServer
   AcceptRemoteConnection = GetNetMode() == NM_DedicatedServer || GetNetMode() == NM_ListenServer


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


Network Conditions
------------------

Netowrk conditions metrics are stored inside the ``UNetDriver`` stored inside the world.
In addition you can hook yourself to the ``FOnNetworkProcessingCausingSlowFrame`` delegate inside ``IpNetDriver``
to display warning to users when network is being slow.

* ``UFindSessionsCallbackProxy::GetPingInMs``
* The ping is store in ``PlayerController->PlayerState->ExactPing``
* Checkout :cpp:func:`UGKUtilityLibrary::GetNetworkMetrics` and :cpp:class:`FGKNetworkMetrics`

+--------+--------------------------------+
| double | LastTickDispatchRealtime;      |
+--------+--------------------------------+
| bool   | bIsPeer;                       |
+--------+--------------------------------+
| bool   | ProfileStats;                  |
+--------+--------------------------------+
| bool   | bSkipLocalStats;               |
+--------+--------------------------------+
| int32  | SendCycles;                    |
+--------+--------------------------------+
| uint32 | InBytesPerSecond;              |
+--------+--------------------------------+
| uint32 | OutBytesPerSecond;             |
+--------+--------------------------------+
| uint32 | InBytes;                       |
+--------+--------------------------------+
| uint32 | InTotalBytes;                  |
+--------+--------------------------------+
| uint32 | OutBytes;                      |
+--------+--------------------------------+
| uint32 | OutTotalBytes;                 |
+--------+--------------------------------+
| uint32 | NetGUIDOutBytes;               |
+--------+--------------------------------+
| uint32 | NetGUIDInBytes;                |
+--------+--------------------------------+
| uint32 | InPackets;                     |
+--------+--------------------------------+
| uint32 | InTotalPackets;                |
+--------+--------------------------------+
| uint32 | OutPackets;                    |
+--------+--------------------------------+
| uint32 | OutTotalPackets;               |
+--------+--------------------------------+
| uint32 | InBunches;                     |
+--------+--------------------------------+
| uint32 | OutBunches;                    |
+--------+--------------------------------+
| uint32 | InTotalBunches;                |
+--------+--------------------------------+
| uint32 | OutTotalBunches;               |
+--------+--------------------------------+
| uint32 | InPacketsLost;                 |
+--------+--------------------------------+
| uint32 | InTotalPacketsLost;            |
+--------+--------------------------------+
| uint32 | OutPacketsLost;                |
+--------+--------------------------------+
| uint32 | OutTotalPacketsLost;           |
+--------+--------------------------------+
| uint32 | InOutOfOrderPackets;           |
+--------+--------------------------------+
| uint32 | OutOutOfOrderPackets;          |
+--------+--------------------------------+
| uint32 | VoicePacketsSent;              |
+--------+--------------------------------+
| uint32 | VoiceBytesSent;                |
+--------+--------------------------------+
| uint32 | VoicePacketsRecv;              |
+--------+--------------------------------+
| uint32 | VoiceBytesRecv;                |
+--------+--------------------------------+
| uint32 | VoiceInPercent;                |
+--------+--------------------------------+
| uint32 | VoiceOutPercent;               |
+--------+--------------------------------+
| double | StatUpdateTime;                |
+--------+--------------------------------+
| float  | StatPeriod;                    |
+--------+--------------------------------+
| uint32 | TotalRPCsCalled;               |
+--------+--------------------------------+
| uint32 | OutTotalAcks;                  |
+--------+--------------------------------+
| bool   | bCollectNetStats;              |
+--------+--------------------------------+
| double | LastCleanupTime;               |
+--------+--------------------------------+
| bool   | bIsStandbyCheckingEnabled;     |
+--------+--------------------------------+
| bool   | bHasStandbyCheatTriggered;     |
+--------+--------------------------------+
| float  | StandbyRxCheatTime;            |
+--------+--------------------------------+
| float  | StandbyTxCheatTime;            |
+--------+--------------------------------+
| int32  | BadPingThreshold;              |
+--------+--------------------------------+
| float  | PercentMissingForRxStandby;    |
+--------+--------------------------------+
| float  | PercentMissingForTxStandby;    |
+--------+--------------------------------+
| float  | PercentForBadPing;             |
+--------+--------------------------------+
| float  | JoinInProgressStandbyWaitTime; |
+--------+--------------------------------+
| int32  | NetTag;                        |
+--------+--------------------------------+
| bool   | DebugRelevantActors;           |
+--------+--------------------------------+


Replication
-----------

.. code-block:: cpp

   #include "Net/Core/PushModel/PushModel.h"

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerState, Score, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerState, bIsSpectator, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerState, bOnlySpectator, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerState, bFromPreviousLevel, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerState, StartTime, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerState, PlayerNamePrivate, SharedParams);

   MARK_PROPERTY_DIRTY_FROM_NAME(APlayerState, Score, this);

Testing
--------

.. code-block:: ini

   # DefaultEngine.ini
   [PacketSimulationSettings]
   PktLag=10
   PktLagVariance=10
   PktLoss=0
   PktOrder=0
   PktDup=0



References
----------

.. [1] `Simulate Network Conditions <https://www.unrealengine.com/en-US/blog/finding-network-based-exploits?sessionInvalidated=true>`_