// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "GKFormat.generated.h"

USTRUCT(BlueprintType)
struct FGKFormat
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int w;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int h;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int wratio;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int hratio;

    // UPROPERTY()
    // class UGKFormatExt* ext;
};
