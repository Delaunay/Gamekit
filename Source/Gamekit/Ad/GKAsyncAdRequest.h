// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "HttpModule.h"
#include "Kismet/BlueprintAsyncActionBase.h"

#include "OpenRTB/GKBidRequest.h"
#include "OpenRTB/GKBidResponse.h"

#include "GKAsyncAdRequest.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGKOnAdRequestSuccess, FGKBidResponse, BidResponse);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGKOnDeserializationFailure);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGKOnNoBid);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGKOnRequestSendFailure);

/**
 * Focus on native support
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = AsyncTask))
class GAMEKIT_API UGKAsyncAdRequest: public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

    public:
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
    static UGKAsyncAdRequest *MakeAdRequest(FGKBidRequest const &Request);

    virtual void Activate() override;

    //! We got bids
    UPROPERTY(BlueprintAssignable)
    FGKOnAdRequestSuccess OnAdRequestSuccess;

    //! We could not deserialize the reply sent back the exchange
    UPROPERTY(BlueprintAssignable)
    FGKOnDeserializationFailure OnDeserializationFailure;

    //! Nobody bid on the ad slot we provided
    UPROPERTY(BlueprintAssignable)
    FGKOnNoBid OnNoBid;

    //! We were not able to send the request
    UPROPERTY(BlueprintAssignable)
    FGKOnRequestSendFailure OnRequestSendFailure;

    protected:
    void OnAdRequestResponse_Native(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

    void OnAdRequestResponse_Success_Native(FHttpRequestPtr Request, FHttpResponsePtr Response);

    void OnAdRequestResponse_Failure_Native(FHttpRequestPtr Request, FHttpResponsePtr Response);

    // FHttpRequestRef ;
    IHttpRequest *HttpRequest;
};
