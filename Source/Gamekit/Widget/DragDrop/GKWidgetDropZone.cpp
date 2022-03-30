#include "Gamekit/Widget/DragDrop/GKWidgetDropZone.h"

// Unreal Engine
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/PanelWidget.h"
#include "Kismet/KismetInputLibrary.h"

bool UGKWidgetDropZone::NativeOnDrop(const FGeometry &     InGeometry,
                                     const FDragDropEvent &InDragDropEvent,
                                     UDragDropOperation *  InOperation)
{
    if (InOperation == nullptr)
    {
        return false;
    }

    UUserWidget *Payload = Cast<UUserWidget>(InOperation->Payload);

    if (Payload == nullptr)
    {
        return false;
    }

    auto AbsolutePos = UKismetInputLibrary::PointerEvent_GetScreenSpacePosition(InDragDropEvent);
    auto LocalPos    = USlateBlueprintLibrary::AbsoluteToLocal(InGeometry, AbsolutePos);

    // Call the blueprint version
    bool Success = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

    if (!Success)
    {
        InOperation->OnDragCancelled.Broadcast(InOperation);
    }
    else
    {
        InOperation->OnDrop.Broadcast(InOperation);
    }

    return Success;
}
