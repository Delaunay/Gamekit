// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Gamekit.h"

// Unreal Engine
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerInput.h"

// Generated
#include "GKPlayerController.generated.h"

/** Base class for PlayerController, should be blueprinted */
UCLASS(Blueprintable)
class GAMEKIT_API AGKPlayerController: public APlayerController
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
};
