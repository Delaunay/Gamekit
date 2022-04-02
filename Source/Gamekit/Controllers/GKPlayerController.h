// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Gamekit.h"

// Unreal Engine
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerInput.h"
#include "GenericTeamAgentInterface.h"

// Generated
#include "GKPlayerController.generated.h"

/** Base class for PlayerController, should be blueprinted */
UCLASS(Blueprintable)
class GAMEKIT_API AGKPlayerController: public APlayerController, 
                                       public IGenericTeamAgentInterface
{
    GENERATED_BODY()

    public:
    // Constructor and overrides
    AGKPlayerController();

    virtual void BeginPlay() override;

    /** Get the input key binded to a given action; it is used to display the expected key press to trigger the action
     */
    UFUNCTION(BlueprintCallable, Category = InputKeys)
    const TArray<FInputActionKeyMapping> &GetKeysForAction(const FName ActionName);

    void GetNetworkMetrics();

    protected:
    /** Called when a global save game as been loaded */
    void HandleSaveGameLoaded(class UGKSaveGame *NewSaveGame);

    public:
    // Sets up Client info for GAS
    void AcknowledgePossession(APawn *P) override;

private:
    // CheatDetection, this cannot change after spawn
    // unless you are a spectator
    // This is a bit set of all the faction that can be made visible
    // i.e each faction will need to be drawn
    // in a standard game only one faction can be drawn
    // that means the fog of war will discard all the other faction
    // during component registration
    // 
    // Can i make component registration happen on the server only
    // and replicate partially on each clients ?
    //UPROPERTY(replicated)
    //uint32_t FogOfWarMode;
};
