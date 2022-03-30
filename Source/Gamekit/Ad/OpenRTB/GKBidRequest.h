// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gamekit/Ad/OpenRTB/GKImp.h"

#include "GKBidRequest.generated.h"

USTRUCT(BlueprintType)
struct FGKBidRequest
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString id;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FGKImp> imp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UGKSite *site;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UGKApp *app;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UGKDevice *device;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UGKUser *user;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int test;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int at;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int tmax;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> wseat;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> bseat;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int allimps;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> cur;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> wlang;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> bcat;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> badv;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> bapp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UGKSource *source;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UGKRegs *regs;

    // UPROPERTY()
    // FGKBidRequestExt ext;
};
