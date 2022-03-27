// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once 

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Container/Matrix.h"

#include "GK_FoW_Strategy.generated.h"


UCLASS(BlueprintType)
class GAMEKIT_API UGKFogOfWarStrategy: public UActorComponent
{
	GENERATED_BODY()

public:

	UGKFogOfWarStrategy();

    virtual void Initialize();

	virtual void Stop() {}

	//! Draw the fog of war for each factions
    virtual void DrawFactionFog(struct FGKFactionFog* FactionFog);

	//! Draw the line of sight using the right method
    virtual void DrawLineOfSight(struct FGKFactionFog *FactionFog, class UGKFogOfWarComponent *c) {}

	void DebugDrawComponent(class UGKFogOfWarComponent *c);

	void DebugDrawPoint(FVector Center, FLinearColor Color = FLinearColor::White, float Radius = 26.f);

	virtual TMatrix3D<uint8> const *GetFactionTextureCPU(FName name) const { 
		return nullptr;
	}

	//! Retrieve the Texture used to draw the fog of war for a given faction
	virtual class UTexture *GetFactionTexture(FName name, bool CreateRenderTarget = true) { return nullptr; }

	//! Retrieve the Texture used to draw the fog of war for a given faction
	virtual class UTexture *GetPreviousFrameFactionTexture(FName name, bool CreateRenderTarget = true) { return nullptr; }

	void OnNewFaction(FName Name) { 
		GetFactionTexture(Name, true);
	}

	// Check if actor has a FogOfWar component, is so trigger the OnSighted event
    void AddVisibleActor(struct FGKFactionFog *      FactionFog,
                         class UGKFogOfWarComponent *SourceComp,
                         class AActor *              Actor);

	void AddVisibleComponent(struct FGKFactionFog *      FactionFog,
                             class UGKFogOfWarComponent *SourceComp,
                             class UGKFogOfWarComponent *SightedComp);

    protected:
	class AGKFogOfWarVolume* FogOfWarVolume;
};
