// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Unreal Engine
#include "CoreMinimal.h"
#include "AIController.h"
#include "GenericTeamAgentInterface.h"

// Generated
#include "GKAIController.generated.h"

/**
 * 
 */
UCLASS()
class GAMEKIT_API AGKAIController: public AAIController
{
    GENERATED_BODY()
    

public:
    // IGenericTeamAgentInterface
    // --------------------------
    public:
    FGenericTeamId TeamId;

    // Called when the controller team changed
    // Called on the server and client
    virtual void OnTeamChange();

    public:
    /** Assigns Team Agent to given TeamID */
    UFUNCTION(BlueprintCallable, Category = "Team")
    void SetGenericTeamId(const FGenericTeamId &TID) override;

    /** Retrieve team identifier in form of FGenericTeamId */
    UFUNCTION(BlueprintCallable, Category = "Team")
    FGenericTeamId GetGenericTeamId() const override { return TeamId; }
};
