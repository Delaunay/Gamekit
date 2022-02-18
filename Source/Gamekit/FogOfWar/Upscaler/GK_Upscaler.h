// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once 

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Gamekit/Container/Matrix.h"

#include "GK_Upscaler.generated.h"


// https://en.wikipedia.org/wiki/Pixel-art_scaling_algorithms
enum class EGK_UpscalingMethod
{
	HQX,
};


using Texel2x2 = uint8;


UCLASS(BlueprintType)
class GAMEKIT_API UGKUpscalerStrategy: public UActorComponent
{
	GENERATED_BODY()

public:
	Texel2x2 GetTexel(TMatrix3D<uint8> const &Mat, FIntVector v);

	UGKUpscalerStrategy();

    virtual void Initialize();

	//! Draw the fog of war for each factions
    virtual void Upscale();

	//! Retrieve the Texture used to draw the fog of war for a given faction
    virtual class UTexture *GetFactionTexture(FName name, bool CreateRenderTarget = true) {
		return nullptr;
	}

    protected:
	class AGKFogOfWarVolume* FogOfWarVolume;

	TMatrix3D<uint8> UpscaledBuffer;
};
