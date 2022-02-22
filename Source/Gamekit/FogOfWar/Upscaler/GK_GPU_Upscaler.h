// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once 

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Gamekit/Container/Matrix.h"

#include "GK_Upscaler_Strategy.h"

#include "GK_GPU_Upscaler.generated.h"


/*
 * Does not work yet
 */
UCLASS(BlueprintType)
class GAMEKIT_API UGKGPUUpscalerStrategy: public UGKUpscalerStrategy
{
	GENERATED_BODY()
		 
public:

    void Initialize() override;

    void Stop() override;

	//! Draw the fog of war for each factions
    virtual void Upscale(FName Name, TMatrix3D<uint8> const *Original, class UTexture2D *Tex);

private:
    class FUpscalingDispatcher *UpscalingDispatcher;
};
