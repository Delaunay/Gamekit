GameFlow
========

Breakdown
---------

#. EngineLoop

   #. Load Engine Plugins/Modules (Pre Init)

      #. EarliestPossible
      #. PostConfigInit
      #. PostSplashScreen
      #. PreEarlyLoadingScreen
      #. PreEarlyLoadingScreen
      #. PreDefault
      #. Default
      #. PostDeault

         #. Actors CDO are created & setup for replications
         #. StartupModule

   #. Init Engine
   #. Load Engine Plugins/Modules (Post Engine Init)
   #. Engine Start

      #. UGameInstance, UGameViewport, ULocalPlayer, WorldContext

         #. GameInstance Start
         #. Browse/Load Map

            #. PreloadMap Broadcast
            #. Destroy Previous World
            #. PreloadContent
            #. Create a new World

               #. Init World
               #. Create Game Mode (Spawn Gameplay Actors)
               #. Load Map
               #. InitializeActorsForPlay

                  #. Register Components to World

               #. GamaMode::InitGame
               #. PreInitializeComponents

                  #. Create GameState
                  #. Create a GameNetwork Manager
                  #. Init Game State
                  #. InitializeComponent

                     #. Activate Components

               #. Post InitializeComponent (Actors is fully initialized)
               #. GameMode::PreLogin (Remote)
               #. GameMode::Login()

                  #. Spawn Player Controller

                     #. Initialize
                     #. Spawn Player State

                  #. Associate UPlayer with its PlayerController

               #. GameMode::PostLogin

                  #. GameMode::RestartPlayer

                     #. Find PlayerStart
                     #. Spawn Pawn for PlayerController (SetPawn)

            #. World->BeginPlay

               #. GameMode::StartPlay

                  #. GameState::HandleBeginPlay

                     #. WorldSetting::NotifyBeginPlay

                        #. Actors->BeginPlay

                           #. Components->BeginPlay

      # GameInstance::OnStart

Flow Charts
-----------

.. image:: /_static/UE/GameFlowChart.png

.. image:: /_static/UE/ActorLifeCycle.jpg

References
----------

.. [1] `Engine Loop <https://www.youtube.com/watch?v=IaU2Hue-ApI>`_
.. [2] `Game flow <https://docs.unrealengine.com/4.27/en-US/InteractiveExperiences/Framework/GameFlow/>`_
.. [3] `Actor lifecycle <https://docs.unrealengine.com/4.26/en-US/ProgrammingAndScripting/ProgrammingWithCPP/UnrealArchitecture/Actors/ActorLifecycle/>`_
