#include "GKAsyncAdRequest.h"

#include "Interfaces/IHttpResponse.h"
#include "JsonObjectConverter.h"

UGKAsyncAdRequest *UGKAsyncAdRequest::MakeAdRequest(FGKBidRequest const &Request)
{
    UGKAsyncAdRequest *AsyncAdRequest = NewObject<UGKAsyncAdRequest>();

    FString URL = "";
    FString OutPayload;

    FJsonObjectConverter::UStructToJsonObjectString(Request, OutPayload);
    auto HttpRequest = FHttpModule::Get().CreateRequest();

    // Prepare request
    HttpRequest->SetVerb("POST");
    HttpRequest->SetHeader("Content-Type", "application/json");
    HttpRequest->SetURL(*FString::Printf(TEXT("%s"), *URL));
    HttpRequest->SetContentAsString(OutPayload);
    // --

    HttpRequest->OnProcessRequestComplete().BindUObject(AsyncAdRequest, &UGKAsyncAdRequest::OnAdRequestResponse_Native);
    AsyncAdRequest->HttpRequest = &HttpRequest.Get();
    return AsyncAdRequest;
}

void UGKAsyncAdRequest::Activate()
{
    if (!HttpRequest->ProcessRequest())
    {
        this->OnAdRequestResponse_Failure_Native(nullptr, nullptr);
    }
}

void UGKAsyncAdRequest::OnAdRequestResponse_Native(FHttpRequestPtr  Request,
                                                   FHttpResponsePtr Response,
                                                   bool             bWasSuccessful)
{
    if (bWasSuccessful)
    {
        return OnAdRequestResponse_Success_Native(Request, Response);
    }

    return OnAdRequestResponse_Failure_Native(Request, Response);
}

void UGKAsyncAdRequest::OnAdRequestResponse_Success_Native(FHttpRequestPtr Request, FHttpResponsePtr Response)
{
    FString inPayload = Response->GetContentAsString();

    FGKBidResponse BidResponse;

    if (!FJsonObjectConverter::JsonObjectStringToUStruct(inPayload, &BidResponse, 0, 0))
    {
        OnDeserializationFailure.Broadcast();
        return;
    }

    OnAdRequestSuccess.Broadcast(BidResponse);
}

void UGKAsyncAdRequest::OnAdRequestResponse_Failure_Native(FHttpRequestPtr Request, FHttpResponsePtr Response)
{
    OnRequestSendFailure.Broadcast();
}