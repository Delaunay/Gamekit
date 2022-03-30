// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "GKDeal.generated.h"

USTRUCT(BlueprintType)
struct FGKDeal
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString id;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float bidfloor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString bidfloorcur;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int at;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> wseat;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> wadomain;

    // UPROPERTY()
    // class UGKDealExt* ext;
};
