// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "GKSource.generated.h"

UCLASS(BlueprintType)
class UGKSource: public UObject
{
    GENERATED_BODY()
    public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int fd;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString tid;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString pchain;

    // UPROPERTY()
    // class UGKSourceExt* ext;
};
