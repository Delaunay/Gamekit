// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "GKSite.generated.h"

UCLASS(BlueprintType)
class UGKSite: public UObject
{
    GENERATED_BODY()
    public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString id;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString domain;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> cat;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> sectioncat;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> pagecat;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString page;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ref;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString search;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int mobile;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int privacypolicy;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int paid;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UGKPublisher *publisher;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UGKContent *content;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString keywords;

    // UPROPERTY()
    // class UGKSiteExt* ext;
};
