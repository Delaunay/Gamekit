// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "GKMetric.generated.h"

USTRUCT(BlueprintType)
struct FGKMetric
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float value;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString vendor;

    // UPROPERTY()
    // class UFGMetricExt* ext;
};
