// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Unreal Engine
#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "GenericTeamAgentInterface.h"

// Generated
#include "GKFogOfWarTeam.generated.h"

/**
 * Team Fog information, this needs to be an actor for replication purposes.
 * It is only replicated to teammates
 */
UCLASS()
class GAMEKIT_API AGKFogOfWarTeam: public AInfo
{
    GENERATED_BODY()

    public:
    AGKFogOfWarTeam();

    bool IsNetRelevantFor(const AActor  *RealViewer,
                          const AActor  *ViewTarget,
                          const FVector &SrcLocation) const override;

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

    void *Buffer;

    //! Exploration Texture
    class UTexture *Exploration;

    //! Vision Texture
    class UTexture *Vision;

    //! Previous Frame Vision
    class UTexture *PreviousFrameVision;

    //! Upscaled TExture
    class UTexture *UpScaledVision;

    //! Using discrete algorithm
    bool bDiscrete;

    //! Team Name
    UPROPERTY(Replicated)
    FName Name;

    //! TeamId
    UPROPERTY(Replicated)
    FGenericTeamId TeamId;

    //! Used to avoid double insert inside Visible Enenmies
    //! not need to replicate this
    TSet<class UGKFogOfWarComponent *> Visible;

    UPROPERTY(Replicated)
    TArray<class UGKFogOfWarComponent *> Allies;

    TArray<class UGKFogOfWarComponent *> const &GetBlocking();

    TArray<class UGKFogOfWarComponent *> Blocking;
};
