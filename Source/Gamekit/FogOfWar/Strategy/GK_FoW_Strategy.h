// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once 

#include "Gamekit.h"
#include "CoreMinimal.h"

#include "GK_FoW_Strategy.generated.h"


UCLASS(BlueprintType)
class UGKFogOfWarStrategy: public UObject
{
	GENERATED_BODY()

public:
	//! Register a new actor to the fog of war volume
	virtual void RegisterActorComponent(class UGKFogOfWarComponent *c) {}

	//! Unregister the actor to the fog of war volume
	virtual void UnregisterActorComponent(class UGKFogOfWarComponent *c) {}

	//! Draw the fog of war for each factions
    virtual void DrawFactionFog();

	//! Draw the line of sight using the right method
    virtual void DrawLineOfSight(class UGKFogOfWarComponent *c) {}

	class AGKFogOfWarVolume* FogOfWarVolume;
};
