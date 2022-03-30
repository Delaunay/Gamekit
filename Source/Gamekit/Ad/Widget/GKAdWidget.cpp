#include "Gamekit/Ad/Widget/GKAdWidget.h"

#include "Gamekit/Ad/GKAsyncAdRequest.h"

#include "Components/PanelWidget.h"


void UGKAdWidget::GetAdChildren(TArray<UGKAdWidget*>& Children) {
    UPanelWidget* Panel = this->GetParent();
   
    for (auto Child : Panel->GetAllChildren()) {
        auto AdWidget = Cast<UGKAdWidget>(Child);

        if (!AdWidget) {
            continue;
        }

        Children.Add(AdWidget);
    }
}

FGKBidRequest UGKAdWidget::GetBidRequest() {
    FGKBidRequest BidRequest;

    TArray<UGKAdWidget*> Children;
    GetAdChildren(Children);

    for (auto Child : Children) {
        if (!Child) {
            continue;
        }
        
        // TODO: Set our imp on an already existing BidRequest
        // TODO: Generate BiRequest.ID here
        // TODO: Generate ImpID here

        auto Imp = Child->GetImpressionSpec();
        BidRequest.imp.Add(Imp);
        ImpIdToWidget.Add(Imp.id, Child);
    }

    return BidRequest;
}

void UGKAdWidget::MakeAdRequest() {

    auto BidRequest = GetBidRequest();

    auto Task = UGKAsyncAdRequest::MakeAdRequest(BidRequest);
    Task->OnAdRequestSuccess.AddDynamic(this, &UGKAdWidget::OnReceiveBidResponse_Native);

    Task->Activate();
}

void UGKAdWidget::OnReceiveBidResponse_Native(FGKBidResponse BidResponse) {
    for (auto& Seat : BidResponse.seatbid) {
        for (auto& Bid : Seat.bid) {
            OnReceivedBid_Native(BidResponse, Seat, Bid);
        }
    }
}

void UGKAdWidget::OnReceivedBid_Native(FGKBidResponse const& BidResponse, FGKSeatBid const& Seatbid, FGKBid const& Bid) {
    if (Bid.impid == "") {
        return;
    }

    UGKAdWidget* ImpWidget = ImpIdToWidget.FindRef(Bid.impid);

    if (!ImpWidget) {
        return;
    }

    ImpWidget->ShowAd(Bid);
}