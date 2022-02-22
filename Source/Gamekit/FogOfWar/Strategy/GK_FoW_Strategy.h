// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once 

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Gamekit/Container/Matrix.h"

#include "GK_FoW_Strategy.generated.h"


UCLASS(BlueprintType)
class GAMEKIT_API UGKFogOfWarStrategy: public UActorComponent
{
	GENERATED_BODY()

public:

	UGKFogOfWarStrategy();

    virtual void Initialize();

	virtual void Stop() {}

	//! Register a new actor to the fog of war volume
	virtual void RegisterActorComponent(class UGKFogOfWarComponent *c) {}

	//! Unregister the actor to the fog of war volume
	virtual void UnregisterActorComponent(class UGKFogOfWarComponent *c) {}

	//! Draw the fog of war for each factions
    virtual void DrawFactionFog();

	//! Draw the line of sight using the right method
    virtual void DrawLineOfSight(class UGKFogOfWarComponent *c) {}

	virtual TMatrix3D<uint8> const *GetFactionTextureCPU(FName name) const { 
		return nullptr;
	}

	//! Retrieve the Texture used to draw the fog of war for a given faction
    virtual class UTexture *GetFactionTexture(FName name, bool CreateRenderTarget = true) {
		return nullptr;
	}

    protected:
	class AGKFogOfWarVolume* FogOfWarVolume;
};
