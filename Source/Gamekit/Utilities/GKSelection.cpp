// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.


#include "Utilities/GKSelection.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/KismetSystemLibrary.h"


UGKBoxSelectionComponent::UGKBoxSelectionComponent() { 
    ActorClassFilter = AActor::StaticClass();
    ExtentMargin     = FVector(0.f, 0.f, 100.f);
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
}


void UGKBoxSelectionComponent::StartOrUpdateBoxSelection(class APlayerController *Controller,
                                                         ETraceTypeQuery          Trace) 
{
    if (Selecting) {
        StartBoxSelection(Controller, Trace);
    } else {
        UpdateBoxSelection(Controller);
    }
}

void UGKBoxSelectionComponent::StartBoxSelection(APlayerController * Controller,
                                                 ETraceTypeQuery Trace) 
{ 
    if (Controller == nullptr) {
        UE_LOG(LogGamekit, Log, TEXT("StartBoxSelection: Controller == null"));
        return;
    }

    if (Selecting) {
        UE_LOG(LogGamekit, Log, TEXT("StartBoxSelection: Already selecting"));
        return;
    }

    TraceChannel = Trace;
    Controller->GetHitResultUnderCursorByChannel(
        TraceChannel,
        false,
        HitResult
    );

    Start = HitResult.ImpactPoint;
    Box   = FBox(Start, Start);
    Selecting = true;
}

void UGKBoxSelectionComponent::UpdateBoxSelection(APlayerController *Controller) {
    if (!Selecting) {
        UE_LOG(LogGamekit, Log, TEXT("UpdateBoxSelection: Not selecting!"));
        return;
    }
    if (Controller == nullptr) {
        UE_LOG(LogGamekit, Log, TEXT("UpdateBoxSelection: Controller == null"));
        return;
    }

    Controller->GetHitResultUnderCursorByChannel(
        TraceChannel, 
        false,
        HitResult
    );

    End = HitResult.ImpactPoint;
    Box = FBox(Start, Start);
    Box += End;
}

void UGKBoxSelectionComponent::FetchBoxSelection(const UObject *World,  TArray<AActor *> &Out) {
    if (!Selecting) {
        UE_LOG(LogGamekit, Log, TEXT("FetchBoxSelection: Not selecting!"));
        return;
    }

    UKismetSystemLibrary::BoxOverlapActors(
        World,
        Box.GetCenter(), 
        Box.GetExtent() + ExtentMargin,
        ObjectTypes, 
        ActorClassFilter, 
        ActorsToIgnore,
        Out
    );
}

void UGKBoxSelectionComponent::EndBoxSelection(const UObject *World) {
    Selecting = false;
}

void UGKBoxSelectionComponent::DrawBoxSelection(const UObject *World) {
    if (!Selecting) {
        UE_LOG(LogGamekit, Log, TEXT("DrawBoxSelection: Not selecting!"));
        return;
    }

    auto Center = Box.GetCenter();
    auto Extent = Box.GetExtent();

    UKismetSystemLibrary::DrawDebugSphere(World,
                                          Start,  // Center
                                          10.f,              // Radius
                                          12,                // Segements
                                          FLinearColor::Red, // Color
                                          1.f,               // Duration
                                          2.f                // Thickness
    );

    UKismetSystemLibrary::DrawDebugSphere(World,
                                          End,    // Center
                                          10.f,              // Radius
                                          12,                // Segements
                                          FLinearColor::Red, // Color
                                          1.f,               // Duration
                                          2.f                // Thickness
    );
    UKismetSystemLibrary::DrawDebugBox(World, 
                                       Center, 
                                       Extent + ExtentMargin,
                                       FLinearColor::White,
                                       FRotator::ZeroRotator,
                                       0.f,                   // Duration
                                       2.f                    // Thickness
    );
}


FVector UGKBoxSelectionComponent::GetCenter() { return Box.GetCenter(); }
FVector UGKBoxSelectionComponent::GetExtent() { return Box.GetExtent(); }
FVector UGKBoxSelectionComponent::GetSize()   { return Box.GetSize();   }
