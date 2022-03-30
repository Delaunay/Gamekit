// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gamekit/Ad/OpenRTB/GKFormat.h"

#include "GKBanner.generated.h"

UCLASS(BlueprintType)
class UGKBanner: public UObject
{
    GENERATED_BODY()
    public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString id;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FGKFormat> format;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int w;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int h;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<int> btype;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<int> battr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int pos;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> mimes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int topframe;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<int> expdir;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<int> api;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int vcm;

    // UPROPERTY()
    // class UGKBannerExt* ext;
};
