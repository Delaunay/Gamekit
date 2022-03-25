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

   Because UE4 clients try to reduce bandwidth & latency some currently invisible actors could get
   replicated to the client, so a cheater could know the location of adversaries (World-hacking).

.. note::

   For network game, encrypting the traffic will not prevent people from reverse engineering.
   The encryption key can be fetched in memory and used to decrypt the traffic.
   It is still recommended to encrypt the traffic as it will prevent outsider from listenning in.


Protection Methods
~~~~~~~~~~~~~~~~~~

Anti Cheats
-----------

The main goal of anticheats is to detect players reading/writing values inside the game's RAM.
Additional goals could be detecting scripting/input automation (i.e player-bot)


Overwatchs
----------

Allow players to review suspicious games.


Anomaly detection
-----------------

.. todo

Attack Vectors
~~~~~~~~~~~~~~

* DLL injection, use DLL loading rules to make the game load yours instead of the official DLL
* Function Hooking; override original functions at runtime to execute arbitrary code
* CheatEngine
* Packet Editing


References
^^^^^^^^^^

.. [1] `Function Hooking <https://en.wikipedia.org/wiki/Hooking>`_
.. [2] `Open Source Anti Cheat <https://github.com/mq1n/NoMercy>`_
.. [3] `CheatEngine <https://github.com/cheat-engine/cheat-engine>`_

