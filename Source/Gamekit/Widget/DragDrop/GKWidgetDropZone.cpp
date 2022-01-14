#include "GKWidgetDropZone.h"

#include "Kismet/KismetInputLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/DragDropOperation.h"


bool UGKWidgetDropZone::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) {
    if (InOperation == nullptr) {
        return false;
    }

    UUserWidget* Payload = Cast<UUserWidget>(InOperation->Payload);

    if (Payload == nullptr) {
        return false;
    }

    auto AbsolutePos = UKismetInputLibrary::PointerEvent_GetScreenSpacePosition(InDragDropEvent);
    auto LocalPos = USlateBlueprintLibrary::AbsoluteToLocal(InGeometry, AbsolutePos);

    return WidgetDrop(Payload, LocalPos);
}

