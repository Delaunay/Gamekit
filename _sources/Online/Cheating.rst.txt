Cheating
========

You can limit cheating by forcing server be the authority on all data.
Clients interaction can be seen as making RPC calls to the server for everything where the parameters are derived inside the client itself.
As such, no data from the client is used & altering the memory of the client will have reduced impact if any.


.. note::

   To reduce latency and bandwidth requirement clients will often simulate remote actions or movement.
   In that case cheating will make the simulation inaccurate but it will fixed itself once the parameters of
   the simulation get updated by replication.


.. warning::

   Because UE clients try to reduce bandwidth & latency some currently invisible actors could get
   replicated to the client, so a cheater could know the location of adversaries (World-hacking).

.. note::

   For network game, encrypting the traffic will not prevent people from reverse engineering.
   The encryption key can be fetched in memory and used to decrypt the traffic.
   It is still recommended to encrypt the traffic as it will prevent outsider from listenning in.


Protection Methods
~~~~~~~~~~~~~~~~~~

Server side authority
---------------------

When a client perform an action, the parameters of the action are derived from the server game state,
the client parameters are never consumed. In the case of a cheating client, the server will be unable
to derive the parameters and the cheat will fail.

.. note::

   Cannot be worked around, cheater cannot impact the server game state beyond their own player inputs.


Limiting Information
--------------------

Server side authority is not enough to prevent all the hacks.
Game clients can have information replicated to them yet invisible to the client under normal circumstances.
Cheating clients could fetch those hidden information (enemy position, for example) and display them to the
player giving them an edge. See :cpp:class:`AGKPrivatePlayerState` for a replicating state to a subset of clients (allies).

.. note::

   Cannot be worked around, cheater cannot get access to data that is not there.


Anti Cheats
-----------

The main goal of anticheats is to detect players reading/writing values inside the game's RAM.
Additional goals could be detecting scripting/input automation (i.e player-bot)

.. note::

   Periodically worked around, needs to be updated with the latest exploits


Overwatchs
----------

Allow players to review suspicious games.


.. note::

   Some cheats can be subtle.


Anomaly detection
-----------------

Clients will send input & movement information periodically, those can be checked for anomaly (low variance, variance of variance, etc...)

In addition, clients will have a set of invariant values that cannot change on the client itself,
for the server to verify them the clients would have to send them periodically.


.. note::

   Both system could be worked around by cheater.

   * Movement variance could be added to a bot (enough to not trigger the detection)
   * A function hook could be installed to send back the expected values, instead of
     the current modified values.


Attack Vectors
~~~~~~~~~~~~~~

* DLL injection, use DLL loading rules to make the game load yours instead of the official DLL
* Function Hooking; override original functions at runtime to execute arbitrary code
* RAM inspection
* CheatEngine
* Packet Editing


References
^^^^^^^^^^

.. [1] `Function Hooking <https://en.wikipedia.org/wiki/Hooking>`_
.. [2] `Open Source Anti Cheat <https://github.com/mq1n/NoMercy>`_
.. [3] `CheatEngine <https://github.com/cheat-engine/cheat-engine>`_

