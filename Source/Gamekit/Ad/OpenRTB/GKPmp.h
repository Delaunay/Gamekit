// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gamekit/Ad/OpenRTB/GKDeal.h"

#include "GKPmp.generated.h"

UCLASS(BlueprintType)
class UGKPmp: public UObject
{
    GENERATED_BODY()
    public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int private_auction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FGKDeal> deals;

    // UPROPERTY()
    // class UGKPmpExt* ext;
};
