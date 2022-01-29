#include "GKWidgetDraggable.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

//! Compute the drag offset and call NativeOnDragDetected
FReply UGKWidgetDraggable::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {

    if (IsDraggable_Native()) {
        return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, DragKey).NativeReply;
    }

    // Blueprint logic
    return UUserWidget::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

//! Create the Drag Widget visual & Create a new DragDrop operation
void UGKWidgetDraggable::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) {

    OutOperation = UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass());

    // OutOperation = NewObject<UDragDropOperation>(this);
    OutOperation->Payload = this; 
    OutOperation->Pivot = EDragPivot::CenterCenter;
    OutOperation->Offset = FVector2D(0, 0);
    OutOperation->OnDragCancelled.AddDynamic(this, &UGKWidgetDraggable::OnDropCancelled_Native);
    OutOperation->OnDrop.AddDynamic(this, &UGKWidgetDraggable::OnDropSuccess_Native);

    if (DragVisualClass) {
        // Kept: for documentation; you need to use CreateWidget else the children widget will not get created
        // DragVisualInstance = NewObject<UGKWidgetDragVisual>(this, DragVisualClass);
        // DragVisualInstance = Cast<UGKWidgetDragVisual>(UGameplayStatics::SpawnObject(DragVisualClass, this));

        UGKWidgetDragVisual* DragVisualInstance = CreateWidget<UGKWidgetDragVisual>(this, DragVisualClass);
        DragVisualInstance->DraggedWidget = this;
        DragVisualInstance->PreviousParent = GetParent();
        DragVisualInstance->RestoreParentOnFailure = true;
        // DragVisualInstance->PreviousLoc = FGKWidgetLocationData(this);

        OutOperation->DefaultDragVisual = DragVisualInstance;
        OutOperation->OnDragCancelled.AddDynamic(DragVisualInstance, &UGKWidgetDragVisual::OnDragCancelled_Native);
    }

    // Blueprint logic
    UUserWidget::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
}

void UGKWidgetDraggable::OnDropCancelled_Native(class UDragDropOperation *Operation) { OnDropCancelled(Operation); }
void UGKWidgetDraggable::OnDropSuccess_Native(class UDragDropOperation* Operation) { OnDropSuccess(Operation); }

bool UGKWidgetDraggable::IsDraggable_Native() {
    // Blueprint logic
    return IsDraggable();
}

void UGKWidgetDraggable::NativeConstruct() {
    DragKey = EKeys::LeftMouseButton;
    UUserWidget::NativeConstruct();
}
