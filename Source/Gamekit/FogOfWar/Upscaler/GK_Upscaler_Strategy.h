// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once 



#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Container/Matrix.h"

#include "GK_Upscaler_Strategy.generated.h"

#define UpscaledTextureType UTexture2D
// #define UpscaledTextureType UCanvasRenderTarget2D

UCLASS(BlueprintType)
class GAMEKIT_API UGKTransformerStrategy: public UActorComponent
{
    GENERATED_BODY()

public:
    UGKTransformerStrategy();

    virtual void Initialize();

    virtual void Stop() {}

    virtual void Transform(struct FGKFactionFog *FactionFog) {}

    //! Retrieve the Texture used to draw the fog of war for a given faction
    virtual class UTexture *GetFactionTexture(FName name, bool CreateRenderTarget = true) { 
        return nullptr;
    }

    FORCEINLINE FIntVector UpscaledTextureSize() const { 
        if (bFixedSize)
            return FixedSize;
        return TextureSize * Multiplier;
    }

    void OnNewFaction(FName Name) { GetFactionTexture(Name, true); }

protected:
    bool                     bUseUpscaledVision;
    FIntVector               FixedSize;
    bool                     bFixedSize;  
    FIntVector               TextureSize;
    uint8                    Multiplier;
    bool                     bInitialized;
    class AGKFogOfWarVolume *FogOfWarVolume;
    FUpdateTextureRegion2D   UpdateRegion;
};


UCLASS(BlueprintType)
class GAMEKIT_API UGKTransformerStrategyTexture2D: public UGKTransformerStrategy
{
    GENERATED_BODY()

    public:
    virtual void Initialize();
    //! Retrieve the Texture used to draw the fog of war for a given faction
    virtual class UTexture *GetFactionTexture(FName name, bool CreateRenderTarget = true);

    protected:
    class UTexture2D *GetFactionTransformTarget(FName name, bool bCreateRenderTarget = true);

    class UTexture2D *CreateTransformTarget();

    UPROPERTY(Transient)
    TMap<FName, class UTexture2D *> TransformedTarget;
};


UCLASS(BlueprintType)
class GAMEKIT_API UGKTransformerStrategyCanvas: public UGKTransformerStrategy
{
    GENERATED_BODY()

    public:
    virtual void Initialize();

    //! Retrieve the Texture used to draw the fog of war for a given faction
    virtual class UTexture *GetFactionTexture(FName name, bool CreateRenderTarget = true);

    protected:
    class UCanvasRenderTarget2D *GetFactionTransformTarget(FName name, bool bCreateRenderTarget = true);

    class UCanvasRenderTarget2D *CreateTransformTarget();

    UPROPERTY(Transient)
    TMap<FName, class UCanvasRenderTarget2D *> TransformedTarget;
};
