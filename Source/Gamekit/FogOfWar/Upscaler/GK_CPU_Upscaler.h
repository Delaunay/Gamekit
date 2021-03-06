// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Container/Matrix.h"
#include "Gamekit/FogOfWar/Upscaler/GK_Upscaler_Strategy.h"

// Unreal Engine
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

// Generated
#include "GK_CPU_Upscaler.generated.h"

// https://en.wikipedia.org/wiki/Pixel-art_scaling_algorithms
enum class EGK_UpscalingMethod
{
};

using Texel2x2 = uint8;

/*
 *  .. note::
 *
 *     this is too slow, we need a GPU kernel to make the upscaling
 *     more efficient, use for debug only
 *
 */
UCLASS(BlueprintType)
class GAMEKIT_API UGKCPUUpscalerStrategy: public UGKTransformerStrategyTexture2D
{
    GENERATED_BODY()

    public:
    void Initialize() override;

    Texel2x2 GetTexel(TMatrix3D<uint8> const &Mat, FIntVector v);

    void Transform(class AGKFogOfWarTeam *FactionFog);

    private:
    TMatrix3D<uint8> UpscaledBuffer;
};
