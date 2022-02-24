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

	//! Draw the fog of war for each factions
    virtual void DrawFactionFog(struct FGKFactionFog* FactionFog);

	//! Draw the line of sight using the right method
    virtual void DrawLineOfSight(class UGKFogOfWarComponent *c) {}

	void DebugDrawComponent(class UGKFogOfWarComponent *c);

	void DebugDrawPoint(FVector Center, FLinearColor Color = FLinearColor::White);

	virtual TMatrix3D<uint8> const *GetFactionTextureCPU(FName name) const { 
		return nullptr;
	}

	//! Retrieve the Texture used to draw the fog of war for a given faction
    virtual class UTexture *GetFactionTexture(FName name, bool CreateRenderTarget = true) {
		return nullptr;
	}

	void OnNewFaction(FName Name) { 
		GetFactionTexture(Name, true);
	}

    protected:
	class AGKFogOfWarVolume* FogOfWarVolume;
};
