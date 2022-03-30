// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "OpenRTB/GKBidRequest.h"

#include "GKAdLibrary.generated.h"

// 0. Automatic Info gathering
// ---------------------------
// Populate the bidRequest with the client information
//
// 1. Easy Exchange Integration
// ----------------------------
// TODO: Make an Integration interface, that specify how a publisher integrates
// with the exchange
// Enable the Integration interface to make BidRequest from a template
//
// 2. Easy Ad Slot Creation
// ------------------------
//
// TODO: Make UMGAdWidget interface with a specialization for each content
//		- Banner
//		- Video
//		- Audio
//		- NativeTitle
//		- NativeImg
//		- NativeVideo
//		- NativeData
//
// Standard
//  UMGAdWidget.GetAdChildren()
//	UMGAdWidget.GetImpressionSpec()				-> returns the Impression object for the BidRequest
//  UMGAdWidget.GetBidRequest(IntegrationDef)	-> returns a full bid request
//  UMGAdWidget.MakeAdRequest(IntegrationDef)	-> Send the Ad request to the exchange and update the widget when the
//  response is received
//	UMGAdWidget.InitFromResponse(adm)			-> render the ad using the response
//
// Native:
//	UMGAdWidget.GetNativeAssetRequestSpec()		->
//  UMGAdWidget.GetNativeMarkupRequest()        -> Build the Native Request
//  UMGAdWidget.GetImpressionSpec()	            -> Returns the impression with the native request
//
// UMGAdWidgetNative    -> is a container that can contain multiple Native object
// UMGAdWidgetContainer -> Contains multiple Ad widget and generate a full BidRequest
//
UCLASS()
class GAMEKIT_API UGKAdLibrary: public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

    public:
    //! Convert a data URI into a 2D Texture
    //! Example: data:image/png;base64,owifeiojIWOD
    UFUNCTION(BlueprintCallable, Category = "Ad|Render")
    static UTexture2D *TextureFromURI(FString URI);

    //! Make a texture form an array of pixels
    UFUNCTION(BlueprintCallable, Category = "Ad|Render")
    static UTexture2D *TextureFromPixels(TArray<uint8> &Buffer);

    //! Initialize a BidRequest with as many fields set as possible
    UFUNCTION(BlueprintCallable, Category = "Ad")
    static FGKBidRequest MakeBidRequest();

    //! Initialize a BidRequest with as little fields as possible
    //! i.e just enough to be able to get to the auction
    UFUNCTION(BlueprintCallable, Category = "Ad")
    static FGKBidRequest MakeBidRequestMinimal();
};
