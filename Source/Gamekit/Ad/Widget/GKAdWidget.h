// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Ad/OpenRTB/GKBidRequest.h"
#include "Gamekit/Ad/OpenRTB/GKBidResponse.h"
#include "Gamekit/Ad/OpenRTB/GKNative.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

// Generated
#include "GKAdWidget.generated.h"

/**
 * Refactor this: It should be a subsystem that makes a single BidRequest for all the AdWidget
 * 
 * i.e this should be turned into a ImpWidget & it should fetch the Ad SubSystem to queue the Impression
 * then the request can be made
 */
UCLASS()
class GAMEKIT_API UGKAdWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// Move this to the AdSubsystem
	TMap<FString, UGKAdWidget*> ImpIdToWidget;

public:
	// Move this to the AdSubsystem
	//! Get All AdWidget contained inside this widget
	virtual void GetAdChildren(TArray<UGKAdWidget*>& Children);
	
	// Move this to the AdSubsystem
	//! Make a bid request with the current AdWidget
	virtual FGKBidRequest GetBidRequest();

	// Move this to the AdSubsystem
	//! Send the request to the exchange
	virtual void MakeAdRequest();

	// Move this to the AdSubsystem
	//! Called when the exchange replies with  bids
	virtual void OnReceiveBidResponse_Native(FGKBidResponse BidResponse);

	// Move this to the AdSubsystem
	virtual void OnReceivedBid_Native(FGKBidResponse const& BidResponse, FGKSeatBid const& Seatbid, FGKBid const& Bid);

	// WidgetImp Interface
	// -------------------
	//! Generate a Impression spec from the AdWidget
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	FGKImp GetImpressionSpec();

	//! Show the ad on the widget after a successful bid
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowAd(FGKBid const& Bid);
};