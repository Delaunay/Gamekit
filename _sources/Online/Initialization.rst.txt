

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