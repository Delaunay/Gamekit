// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Unreal Engine
#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GenericTeamAgentInterface.h"

// Generated
#include "GKPlayerState.generated.h"


#define GK_PLAYERSTATE_SANITY 0

/**
 * 
 */
UCLASS()
class GAMEKIT_API AGKPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    AGKPlayerState();

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const;

    // Is this the earliest we can do it ?
    // hoping to create the Private player state
    // before player state replication
    void PreInitializeComponents() override;

    TSubclassOf<class AGKPrivatePlayerState> PrivatePlayerStateClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
    class AGKPrivatePlayerState* PrivatePlayerState;

    FGenericTeamId GetGenericTeamId() const { 
        // GetOwner Should be a PlayerController
        return FGenericTeamId::GetTeamIdentifier(GetOwner());
    }

/*
    // This is just an example to check different replication setting
    void Tick(float Delat) override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, ReplicatedUsing=OnRep_Version)
    int Version;

    UFUNCTION()
    void OnRep_Version();
//*/
};
 