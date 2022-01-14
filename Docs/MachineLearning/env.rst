UnrealEnv
=========

.. note::

   A lot of the work is delegated to the UE4 instance, the environment simply
   query it using remote function calls.


Setup
~~~~~

#. Launch client ``Client``
#. Launch UE4  ``UE4Runner.run``
#. Connect to UE4 ``UnrealEnv.connect``
#. Setup Agents

    #. new_agent / configure agent
    #. get action space
    #. get observation space


Steps
~~~~~

#. act
#. get observation
#. get reward


Reset
~~~~~

#. AGameModeBase::ResetLevel
#. GetSession().ResetWorld()

    #. AGameModeBase::RestartPlayer(AController*)
    #. IsDone() == AGameModeBase::HasMatchEnded()