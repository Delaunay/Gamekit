// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "GKBid.generated.h"

USTRUCT(BlueprintType)
struct FGKBid
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString id;

    UPROPERTY(BlueprintReadOnly)
    FString impid;

    UPROPERTY(BlueprintReadOnly)
    float price;

    UPROPERTY(BlueprintReadOnly)
    FString nurl;

    UPROPERTY(BlueprintReadOnly)
    FString burl;

    UPROPERTY(BlueprintReadOnly)
    FString lurl;

    UPROPERTY(BlueprintReadOnly)
    FString adm;

    UPROPERTY(BlueprintReadOnly)
    class UGKNativeMarkupResponse *adm_native;

    UPROPERTY(BlueprintReadOnly)
    FString adid;

    UPROPERTY(BlueprintReadOnly)
    TArray<FString> adomain;

    UPROPERTY(BlueprintReadOnly)
    FString bundle;

    UPROPERTY(BlueprintReadOnly)
    FString iurl;

    UPROPERTY(BlueprintReadOnly)
    FString cid;

    UPROPERTY(BlueprintReadOnly)
    FString crid;

    UPROPERTY(BlueprintReadOnly)
    FString tactic;

    UPROPERTY(BlueprintReadOnly)
    TArray<FString> cat;

    UPROPERTY(BlueprintReadOnly)
    TArray<int> attr;

    UPROPERTY(BlueprintReadOnly)
    int api;

    UPROPERTY(BlueprintReadOnly)
    int protocol;

    UPROPERTY(BlueprintReadOnly)
    int qagmediarating;

    UPROPERTY(BlueprintReadOnly)
    FString language;

    UPROPERTY(BlueprintReadOnly)
    FString dealid;

    UPROPERTY(BlueprintReadOnly)
    int w;

    UPROPERTY(BlueprintReadOnly)
    int h;

    UPROPERTY(BlueprintReadOnly)
    int wration;

    UPROPERTY(BlueprintReadOnly)
    int hration;

    UPROPERTY(BlueprintReadOnly)
    int exp;

    // UPROPERTY()
    // class UGKBidExt* ext;
};

USTRUCT(BlueprintType)
struct FGKNative_AssetResponse
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(BlueprintReadOnly)
    int id;

    UPROPERTY(BlueprintReadOnly)
    int required;

    UPROPERTY(BlueprintReadOnly)
    class UGKNative_TitleResponse *title;

    UPROPERTY(BlueprintReadOnly)
    class UGKNative_ImgResponse *img;

    UPROPERTY(BlueprintReadOnly)
    class UGKNative_VideoResponse *video;

    UPROPERTY(BlueprintReadOnly)
    class UGKNative_DataResponse *data;

    UPROPERTY(BlueprintReadOnly)
    class UGKNative_LinkResponse *link;

    // UPROPERTY()
    // class UGKNative_AssetResponseExt* ext;
};

USTRUCT(BlueprintType)
struct FGKNative_Pair
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString first;

    UPROPERTY(BlueprintReadOnly)
    FString second;
};

USTRUCT(BlueprintType)
struct FGKNative_EventTrackerResponse
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(BlueprintReadOnly)
    int event;

    UPROPERTY(BlueprintReadOnly)
    int method;

    UPROPERTY(BlueprintReadOnly)
    FString url;

    UPROPERTY(BlueprintReadOnly)
    TArray<FGKNative_Pair> customdata;

    // UPROPERTY()
    // class UGKNative_EventTrackerResponseExt* est;
};

UCLASS(BlueprintType)
class UGKNativeMarkupResponse: public UObject
{
    GENERATED_BODY()
    public:
    UPROPERTY(BlueprintReadOnly)
    FString ver;

    UPROPERTY(BlueprintReadOnly)
    TArray<FGKNative_AssetResponse> assets;

    UPROPERTY(BlueprintReadOnly)
    FString assetsurl;

    UPROPERTY(BlueprintReadOnly)
    FString dcourl;

    UPROPERTY(BlueprintReadOnly)
    class UGKNative_LinkResponse *link;

    UPROPERTY(BlueprintReadOnly)
    TArray<FString> imptrackers;

    UPROPERTY(BlueprintReadOnly)
    FString jstracker;

    UPROPERTY(BlueprintReadOnly)
    TArray<FGKNative_EventTrackerResponse> eventrackers;

    UPROPERTY(BlueprintReadOnly)
    FString privacy;

    // UPROPERTY()
    // class UGKNativeMarkupRequestExt* ext;
};

UCLASS(BlueprintType)
class UGKNative_TitleResponse: public UObject
{
    GENERATED_BODY()
    public:
    UPROPERTY(BlueprintReadOnly)
    FString text;

    UPROPERTY(BlueprintReadOnly)
    int len;

    // UPROPERTY()
    // class UGKNative_TitleResponseExt* ext;
};

UCLASS(BlueprintType)
class UGKNative_ImgResponse: public UObject
{
    GENERATED_BODY()
    public:
    UPROPERTY(BlueprintReadOnly)
    int type;

    UPROPERTY(BlueprintReadOnly)
    FString url;

    UPROPERTY(BlueprintReadOnly)
    int w;

    UPROPERTY(BlueprintReadOnly)
    int h;

    // UPROPERTY()
    // class UGKNative_ImgResponseExt* ext;
};

UCLASS(BlueprintType)
class UGKNative_DataResponse: public UObject
{
    GENERATED_BODY()
    public:
    UPROPERTY(BlueprintReadOnly)
    int type;

    UPROPERTY(BlueprintReadOnly)
    int len;

    UPROPERTY(BlueprintReadOnly)
    FString value;

    // UPROPERTY()
    // class UGKNative_DataResponseExt* ext;
};

UCLASS(BlueprintType)
class UGKNative_VideoResponse: public UObject
{
    GENERATED_BODY()
    public:
    UPROPERTY(BlueprintReadOnly)
    FString vasttag;

    // UPROPERTY()
    // class UGKNative_VideoResponseExt* ext;
};

UCLASS(BlueprintType)
class UGKNative_LinkResponse: public UObject
{
    GENERATED_BODY()
    public:
    UPROPERTY(BlueprintReadOnly)
    FString url;

    UPROPERTY(BlueprintReadOnly)
    TArray<FString> clicktrackers;

    UPROPERTY(BlueprintReadOnly)
    FString fallback;

    // UPROPERTY()
    // class UGKNative_LinkResponseExt* ext;
};
