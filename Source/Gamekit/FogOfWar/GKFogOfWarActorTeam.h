// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Gamekit.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "GenericTeamAgentInterface.h"

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
    class UTexture *Exploration;
    class UTexture *Vision;
    class UTexture *PreviousFrameVision;
    class UTexture *UpScaledVision;
    bool            bDiscrete;
	void		   *Buffer;

	// Replicated only when relevant i.e when client PC is an ally
	//
	// UPROPERTY(replicated)
    TSet<class UGKFogOfWarComponent *>   VisibleEnemies;

	UPROPERTY(replicated)
    TArray<class UGKFogOfWarComponent *> Allies;

	// Team Interface
	// --------------
	
	/** Assigns Team Agent to given TeamID */
	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) {
		TeamId = TeamID;
	}
	
	/** Retrieve team identifier in form of FGenericTeamId */
	virtual FGenericTeamId GetGenericTeamId() const { return TeamId; }

	/** Retrieved owner attitude toward given Other object */
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const
	{ 
		const IGenericTeamAgentInterface* OtherTeamAgent = Cast<const IGenericTeamAgentInterface>(&Other);
		return OtherTeamAgent ? FGenericTeamId::GetAttitude(GetGenericTeamId(), OtherTeamAgent->GetGenericTeamId())
			: ETeamAttitude::Neutral;
	}
}; 