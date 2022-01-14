#include "GKWidgetDragVisual.h"

#include "Blueprint/DragDropOperation.h"
#include "Components/PanelWidget.h"

void UGKWidgetDragVisual::OnDragCancelled_Native(UDragDropOperation* Operation) {

    if (RestoreParentOnFailure && DraggedWidget != nullptr && PreviousParent != nullptr) {
        DraggedWidget->RemoveFromParent();
        PreviousParent->AddChild(DraggedWidget);
        // PreviousLoc.Reset(DraggedWidget);
    }
}
