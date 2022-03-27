// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "FogOfWar/Upscaler/GK_Upscaler_Strategy.h"
#include "GKExplorationTransform.generated.h"

/**
 * 
 */
UCLASS()
class GAMEKIT_API UGKExplorationTransform: public UGKTransformerStrategyCanvas
{
	GENERATED_BODY()

public:
    void Initialize();

    void Transform(struct FGKFactionFog *FactionFog) override;

    /*
    class UMaterialInstanceDynamic *GetFactionMaterialInstance(FName Name);

    class UMaterialInterface *ExplorationMaterial;

    UPROPERTY(Transient)
    TMap<FName, class UMaterialInstanceDynamic *> MaterialInstances;
    */
};
