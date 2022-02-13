// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once 

#include "Gamekit/FogOfWar/Strategy/GK_FoW_Strategy.h"

#include "GK_FoW_ShadowCasting.generated.h"


UCLASS(BlueprintType)
class UGKShadowCasting: public UGKFogOfWarStrategy
{
	GENERATED_BODY()

public:
	//! Register a new actor to the fog of war volume
	virtual void RegisterActorComponent(class UGKFogOfWarComponent *c) {}

	//! Unregister the actor to the fog of war volume
	virtual void UnregisterActorComponent(class UGKFogOfWarComponent *c) {}

	//! Draw the line of sight using the right method
    virtual void DrawLineOfSight(class UGKFogOfWarComponent *c) {}

	class AGKFogOfWarVolume* FogOfWarVolume;
};
