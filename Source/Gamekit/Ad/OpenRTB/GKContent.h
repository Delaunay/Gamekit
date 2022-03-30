// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "GKContent.generated.h"

UCLASS(BlueprintType)
class UGKContent: public UObject
{
    GENERATED_BODY()
    public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString id;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int episode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString title;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString series;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString season;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString artist;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString genre;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString album;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString isrc;

    // UPROPERTY(EditAnywhere, BlueprintReadWrite)
    // class UGKProducer* producer;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString url;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> cat;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int prodq;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int context;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString contentrating;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString userrating;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int qagmediarating;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString keywords;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int livestream;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int sourcerelationship;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int len;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString language;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int embeddable;

    // UPROPERTY(EditAnywhere, BlueprintReadWrite)
    // TArray<FGKData> data;

    // UPROPERTY()
    // class UGKContentExt* ext;
};
