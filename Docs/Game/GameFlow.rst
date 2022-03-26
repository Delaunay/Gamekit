GameFlow
========

.. image:: /_static/GameFlowChart.png


Breakdown
~~~~~~~~~

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

References
----------

.. [1] https://www.youtube.com/watch?v=IaU2Hue-ApI
