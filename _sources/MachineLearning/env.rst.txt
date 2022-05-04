UnrealEnv
=========


.. warning::

   This documentation was moved to `GKML <https://delaunay.github.io/GKMachineLearning/>`_


.. note::

   A lot of the work is delegated to the UE instance, the environment simply
   query it using remote function calls.


Setup
~~~~~

#. Launch client ``Client``
#. Launch UE  ``UERunner.run``
#. Connect to UE ``UnrealEnv.connect``
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