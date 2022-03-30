// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "GKApp.generated.h"

UCLASS(BlueprintType)
class UGKApp: public UObject
{
    GENERATED_BODY()
    public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString id;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString bundle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString domain;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString storeurl;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> cat;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> sectioncat;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> pagecat;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ver;

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
    // class FGKAppExt* ext;
};
