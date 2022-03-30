// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "GKPublisher.generated.h"

UCLASS(BlueprintType)
class UGKPublisher: public UObject
{
    GENERATED_BODY()
    public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString id;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> cat;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString domain;

    // UPROPERTY()
    // class UGKPublisherExt* ext;
};
