// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Container/Matrix.h"
#include "Gamekit/FogOfWar/Upscaler/GK_Upscaler_Strategy.h"

// Unreal Engine
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

// Generated
#include "GK_GPU_Upscaler.generated.h"

/*
 *
 *  TODO: batch multi factions upscaling
 */
UCLASS(BlueprintType)
class GAMEKIT_API UGKGPUUpscalerStrategy: public UGKTransformerStrategyTexture2D
{
    GENERATED_BODY()

    public:
    void Initialize() override;

    void Stop() override;

    //! Draw the fog of war for each factions
    void Transform(class AGKFogOfWarActorTeam *FactionFog);

    private:
    class FUpscalingDispatcher *UpscalingDispatcher;
};
