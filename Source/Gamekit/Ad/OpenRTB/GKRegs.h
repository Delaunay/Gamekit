// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "GKRegs.generated.h"

UCLASS(BlueprintType)
class UGKRegs: public UObject
{
    GENERATED_BODY()
    public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int coppa;

    // UPROPERTY()
    // class FGKRegsExt* ext;
};
