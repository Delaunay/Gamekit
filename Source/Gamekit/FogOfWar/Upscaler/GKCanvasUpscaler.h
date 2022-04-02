// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/FogOfWar/Upscaler/GK_Upscaler_Strategy.h"

// Unreal Engine
#include "CoreMinimal.h"

// Generated
#include "GKCanvasUpscaler.generated.h"

/**
 *
 */
UCLASS()
class GAMEKIT_API UGKCanvasUpscaler: public UGKTransformerStrategyCanvas
{
    GENERATED_BODY()

    public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|Upscaling")
    class UMaterialInterface *UpscalingMaterial;

    void Transform(class AGKFogOfWarActorTeam *FactionFog) override;

    void Initialize() override;

    protected:
    class UMaterialInstanceDynamic *GetFactionMaterialInstance(FName Name);

    UPROPERTY(Transient)
    TMap<FName, class UMaterialInstanceDynamic *> UpscaleMaterialInstance;
};
