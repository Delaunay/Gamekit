// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gamekit/Ad/OpenRTB/GKMetric.h"

#include "GKImp.generated.h"

USTRUCT(BlueprintType)
struct FGKImp
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString id;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FGKMetric> metric;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UGKBanner *banner;

    // UPROPERTY(EditAnywhere, BlueprintReadWrite)
    // class UGKVideo* video;

    // UPROPERTY(EditAnywhere, BlueprintReadWrite)
    // class UGKAudio* audio;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UGKNative *native;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UGKPmp *pmp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString displaymanager;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString displaymanagerver;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int instl;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString tagid;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float bidfloor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString bidfloorcur;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int clickbrowser;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int secure;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> iframebuster;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int exp;

    // UPROPERTY(EditAnywhere, BlueprintReadWrite)
    // class UGKImpExt* ext;
};
