// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gamekit/Ad/OpenRTB/GKFormat.h"

#include "GKNative.generated.h"

UCLASS(BlueprintType)
class UGKNative: public UObject
{
    GENERATED_BODY()

    public:
    // This is UGKNativeMarkupRequest encoded in json
    UPROPERTY()
    FString request;

    // TODO: add a setter that sets request from UGKNativeMarkupRequest

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UGKNativeMarkupRequest *request_native;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ver;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int api;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<int> battr;

    // UPROPERTY()
    // class UGKNativeExt* ext;
};

USTRUCT(BlueprintType)
struct FGKNative_EventTrackerRequest
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int event;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<int> methods;

    // UPROPERTY()
    // class FGKNative_EventTrackerExt* ext;
};

USTRUCT(BlueprintType)
struct FGKNative_AssetRequest
{

    GENERATED_USTRUCT_BODY()

    UPROPERTY(BlueprintReadOnly)
    int id;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int required;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UGKNative_Title *title;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UGKNative_Img *img;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UGKNative_Video *video;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UGKNative_Data *data;

    // UPROPERTY()
    // class UGKAssetRequestExt* ext;
};

UCLASS(BlueprintType)
class UGKNativeMarkupRequest: public UObject
{
    GENERATED_BODY()
    public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ver;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int context;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int contextsubtype;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int plcmttype;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int plcmtcnt;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int seq;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FGKNative_AssetRequest> assets;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int aurlsupport;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int durlsupport;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FGKNative_EventTrackerRequest> eventtrackers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int privacy;

    // UPROPERTY()
    // class GKNativeMarkupRequestExt* ext;
};

UCLASS(BlueprintType)
class UGKNative_Title: public UObject
{
    GENERATED_BODY()
    public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int len;

    // UPROPERTY()
    // class UGKNative_TitleExt* ext;
};

UCLASS(BlueprintType)
class UGKNative_Img: public UObject
{
    GENERATED_BODY()
    public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int w;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int wmin;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int h;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int hmin;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> mimes;

    // UPROPERTY()
    // class* UGKNative_ImgExt ext;
};

UCLASS(BlueprintType)
class UGKNative_Video: public UObject
{
    GENERATED_BODY()
    public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> mimes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int minduration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int maxduration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<int> protocols;

    // UPROPERTY()
    // class UGKNative_VideoExt* ext;
};

UCLASS(BlueprintType)
class UGKNative_Data: public UObject
{
    GENERATED_BODY()
    public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int len;

    // UPROPERTY()
    // class UGKNative_DataExt* ext;
};
