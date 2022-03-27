// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "FogOfWar/Upscaler/GK_Upscaler_Strategy.h"
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

	void Transform(struct FGKFactionFog *FactionFog) override;

	void Initialize() override;

protected:
    class UMaterialInstanceDynamic * GetFactionMaterialInstance(FName Name);

	UPROPERTY(Transient)
	TMap<FName, class UMaterialInstanceDynamic *> UpscaleMaterialInstance;
};
