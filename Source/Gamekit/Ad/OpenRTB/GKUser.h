// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "GKUser.generated.h"

UCLASS(BlueprintType)
class UGKUser: public UObject
{
    GENERATED_BODY()
    public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString id;

    // UPROPERTY()
    // class UGKUserExt* ext;
};
