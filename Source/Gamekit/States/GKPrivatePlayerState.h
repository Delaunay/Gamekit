// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once


// Unreal Engine
#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerState.h"

// Generated
#include "GKPrivatePlayerState.generated.h"



#define GK_PRIVATEPLAYERSTATE_SANITY 1

/** Private state that only replicated to player that are friendly to
 *  the player controller owning the player state
 */
UCLASS()
class GAMEKIT_API AGKPrivatePlayerState : public AInfo
{
    GENERATED_BODY()

public:

    AGKPrivatePlayerState();

    bool IsNetRelevantFor(const AActor * RealViewer,  
                          const AActor * ViewTarget, 
                          const FVector & SrcLocation) const;

    FGenericTeamId GetGenericTeamId() const;

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const;

//*
    void Tick(float Delat) override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, ReplicatedUsing=OnRep_Version)
    int Version;

    UFUNCTION()
    void OnRep_Version();
//*/
};
