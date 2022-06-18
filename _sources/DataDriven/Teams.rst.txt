Teams
=====

Gamekit comes with a friend or foe system which leverages the IGenericTeamAgentInterface.
You can simply define the teams per level.

Teams are defined inside a DataTable (see :cpp:`FGKTeamInfo`), the datatable reference is held by the ``GKWorldSettings``.
The fog of war and the gameplay ability system leverage fully the team system.

