// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Gamekit.h"
#include "Gamekit/Team/GKTeamAgentMixin.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "GameFramework/Info.h"

// Generated
#include "GKFogOfWarActorTeam.generated.h"

/** Set of actors belonging to the same team,
 * This is represented as an actor for replication purposes.
 */ 
UCLASS()
class GAMEKIT_API AGKFogOfWarActorTeam: public AInfo,
                                        public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

	virtual bool IsNetRelevantFor(
        const AActor * RealViewer,
        const AActor * ViewTarget,
        const FVector & SrcLocation) const override;

	AGKFogOfWarActorTeam();

	FGenericTeamId  TeamId;
	FName           Name;
	class AGKFogOfWarVolume* FogOfWar;
    class UTexture *Exploration;
    class UTexture *Vision;
    class UTexture *PreviousFrameVision;
    class UTexture *UpScaledVision;
    bool            bDiscrete;
	void		   *Buffer;

	// Replicated only when relevant i.e when client PC is an ally
	//
	UPROPERTY(Replicated)
    TArray<class UGKFogOfWarComponent *> VisibleEnemies;

	UPROPERTY(Replicated, ReplicatedUsing=OnRep_Allies)
    TArray<class UGKFogOfWarComponent *> Allies;

	UFUNCTION()
	void OnRep_Allies();

	DEFINE_TEAM_AGENT()
}; 