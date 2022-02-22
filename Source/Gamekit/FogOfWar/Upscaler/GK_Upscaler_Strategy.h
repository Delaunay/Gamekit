// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once 



#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Gamekit/Container/Matrix.h"

#include "GK_Upscaler_Strategy.generated.h"

#define UpscaledTextureType UTexture2D
// #define UpscaledTextureType UCanvasRenderTarget2D

UCLASS(BlueprintType)
class GAMEKIT_API UGKUpscalerStrategy: public UActorComponent
{
	GENERATED_BODY()
		 
public:
	UGKUpscalerStrategy();

    virtual void Initialize();

    virtual void Stop() {}

	//! Draw the fog of war for each factions
    virtual void Upscale(FName Name, TMatrix3D<uint8> const *Original, class UTexture2D* Tex) {}

	//! Retrieve the Texture used to draw the fog of war for a given faction
    virtual class UTexture *GetFactionTexture(FName name, bool CreateRenderTarget = true);

protected:
	class UpscaledTextureType *GetFactionUpscaleTarget(FName name, bool bCreateRenderTarget = true);

    class UpscaledTextureType *CreateUpscaleTarget();

    UPROPERTY(Transient)
    TMap<FName, class UTexture2D *> UpscaledFogFactions;

    FIntVector               TextureSize;
    uint8                    Multiplier;
    bool                     bInitialized;
    class AGKFogOfWarVolume *FogOfWarVolume;
    FUpdateTextureRegion2D   UpdateRegion;
};
