// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/FogOfWar/Upscaler/GK_Upscaler_Strategy.h"

// Unreal Engine
#include "CoreMinimal.h"

// Generated
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
