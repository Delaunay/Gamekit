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


/** Base class for PlayerController, should be blueprinted 
 * The controller is the one with the Team Agent interface because it is the top
 * level object we see when `IsNetRelevantFor` is called, which makes it easy for us
 * to replicate based on the Team attitude.
 * 
 * We can also change pawn teamid on possession
 */
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

    // ----------------

    protected:
    /** Called when a global save game as been loaded */
    void HandleSaveGameLoaded(class UGKSaveGame *NewSaveGame);

    public: 
    // Sets up Client info for GAS
    void AcknowledgePossession(APawn * NewPawn) override;

    // Set Generic Team ID on possession
    void ServerAcknowledgePossession_Implementation(APawn * NewPawn) override;

    UFUNCTION(BlueprintImplementableEvent, Category = "Pawn", meta = (DisplayName = "On Acknowledge Possession"))
    void ReceiveAcknowledgePossession(APawn* NewPawn);

    // Replication
    // -----------

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const;
     
    // IGenericTeamAgentInterface
    // --------------------------
    public:
    UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing=OnRep_TeamChange)
    FGenericTeamId TeamId;
    
    UFUNCTION()
    void OnRep_TeamChange();

    // Called when the controller team changed
    // Called on the server and client
    virtual void OnTeamChange();
    
public:
    /** Assigns Team Agent to given TeamID */
    UFUNCTION(BlueprintCallable, Category = "Team")
    void SetGenericTeamId(const FGenericTeamId& TID) override;
    
    /** Retrieve team identifier in form of FGenericTeamId */
    FGenericTeamId GetGenericTeamId() const override { return TeamId; }
};
