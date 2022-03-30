// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gamekit/Ad/OpenRTB/GKSeatBid.h"

#include "GKBidResponse.generated.h"

USTRUCT(BlueprintType)
struct FGKBidResponse
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString id;

    UPROPERTY(BlueprintReadOnly)
    TArray<FGKSeatBid> seatbid;

    UPROPERTY(BlueprintReadOnly)
    FString bidid;

    UPROPERTY(BlueprintReadOnly)
    FString cur;

    UPROPERTY(BlueprintReadOnly)
    FString customdata;

    UPROPERTY(BlueprintReadOnly)
    int nbr;

    // UPROPERTY()
    // class UGKBidResponseExt* ext;
};
