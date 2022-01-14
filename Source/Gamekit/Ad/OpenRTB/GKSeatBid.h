// BSD 3-Clause License Copyright (c) 2021, Pierre Delaunay All rights reserved.

#pragma once 

#include "CoreMinimal.h"

#include "GKBid.h"

#include "GKSeatBid.generated.h"

USTRUCT(BlueprintType)
struct FGKSeatBid
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(BlueprintReadOnly)
    TArray<FGKBid> bid;

    UPROPERTY(BlueprintReadOnly)
    FString seat;

    UPROPERTY(BlueprintReadOnly)
    int group;

    // UPROPERTY()
    // class UGKSeatBidExt* ext;
};
