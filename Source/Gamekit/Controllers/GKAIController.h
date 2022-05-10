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
    // Called when the controller team changed
    // Called on the server and client
    virtual void OnTeamChange();

    virtual void OnPossess(APawn* InPawn) override {
        Super::OnPossess(InPawn);

        if (GetGenericTeamId() == FGenericTeamId::NoTeam){
            return;
        }

        auto Agent = Cast<IGenericTeamAgentInterface>(InPawn);

        if (Agent)
        {
            Agent->SetGenericTeamId(GetGenericTeamId());
        }
    }

    public:
    /** Assigns Team Agent to given TeamID */
    UFUNCTION(BlueprintCallable, Category = "Team")
    void SetGenericTeamId(const FGenericTeamId &TeamID) override;

    /** Retrieve team identifier in form of FGenericTeamId */
    UFUNCTION(BlueprintCallable, Category = "Team")
    FGenericTeamId GetGenericTeamId() const override { return Super::GetGenericTeamId(); }
};
