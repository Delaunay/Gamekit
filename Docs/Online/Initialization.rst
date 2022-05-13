

The order in which the client receive events from the server is random.
In the example below,  ``AGKPlayerController::ServerAcknowledgePossession_Implementation``,
sets the generic team id, which will trigger the `TeamAssigned` inside the possed pawn.
which gets replicated down.

* [HNSPlayerController_C_0] Server: BeginPlay     <-+
* [HNSPlayerController_C_0] Server: Possessed   <-+ |
* [HNSPlayerController_C_0] Client 1: Possessed <-+ |
* [HNSPlayerController_C_1] Server: BeginPlay       | <---+
* [HNSPlayerController_C_1] Server: Possessed       | <-+ |
* [HNSPlayerController_C_0] Client 1: BeginPlay   <-+   | |
* [HNSPlayerController_C_0] Server: TeamAssigned  <--+  | |
* [HNSPlayerController_C_0] Client 1: TeamAssigned<--+  | |
* [HNSPlayerController_C_0] Client 2: Possessed       <-+ |
* [HNSPlayerController_C_0] Client 2: BeginPlay       <---+
* [HNSPlayerController_C_1] Server: TeamAssigned      <-+
* [HNSPlayerController_C_0] Client 2: TeamAssigned    <-+


* Team assignment could happen in ``InitNewPlayer`` for matchmaking
  Get the team from the options,

.. code-block::

    FString InName = UGameplayStatics::ParseOption(Options, TEXT("Name")).Left(20);
    if (InName.IsEmpty())
    {
        InName = FString::Printf(TEXT("%s%i"), *DefaultPlayerName.ToString(), NewPlayerController->PlayerState->GetPlayerId());
    }


* You can initialize actor very early using `OnActorPreSpawnInitialization`
  problem is we lack the Start info

.. code-block::

	// Broadcast delegate before the actor and its contained components are initialized
	OnActorPreSpawnInitialization.Broadcast(Actor);

	Actor->PostSpawnInitialize(UserTransform, SpawnParameters.Owner, SpawnParameters.Instigator, SpawnParameters.IsRemoteOwned(), SpawnParameters.bNoFail, SpawnParameters.bDeferConstruction);


Match Quality:

    Q = p * (1 - p) / var(player ratings)

Quality is highest when p = 0.5 and player ratings close together
