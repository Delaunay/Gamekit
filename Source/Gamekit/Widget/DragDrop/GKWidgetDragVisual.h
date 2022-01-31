#pragma once

#include "Blueprint/UserWidget.h"

#include "GKWidgetDragVisual.generated.h"


// This is supposed to reset the location of the Widget on Drag&Drop failures but it does not work
struct FGKWidgetLocationData {
    FGKWidgetLocationData() {}

    FGKWidgetLocationData(UUserWidget* Widget) {
        DesiredSize = Widget->GetDesiredSize();
        AlignmentInViewport = Widget->GetAlignmentInViewport();
        AnchorsInViewport = Widget->GetAnchorsInViewport();
    }

    void Reset(UUserWidget* Widget) {
        Widget->SetDesiredSizeInViewport(DesiredSize);
        Widget->SetAlignmentInViewport(AlignmentInViewport);
        Widget->SetAnchorsInViewport(AnchorsInViewport);
    }

    FVector2D DesiredSize;
    FVector2D AlignmentInViewport;
    FAnchors AnchorsInViewport;
};

/*! Widget used to visualize the drag & drop operation that is in progress
 * 
 * \rst
 * .. warning::
 * 
 *    If you use :member:`UPanelWidget::AddChild` on the :class:`UGKWidgetDraggable` it will lose its previous parent.
 *    :class:`UGKWidgetDragVisual` will try to restore the parent of :class:`UGKWidgetDraggable` on drag failure,
 *    nevertheless, failure to restore will make the dragged widget disappear.
 * 
 * \endrst
 */
UCLASS(BlueprintType, Blueprintable, meta = (DisableNativeTick))
class GAMEKIT_API UGKWidgetDragVisual : public UUserWidget
{
    GENERATED_BODY()

public:
    //! Current widget being dragged
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drag and Drop", meta = (ExposeOnSpawn = "true"))
    UUserWidget* DraggedWidget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drag and Drop", meta = (ExposeOnSpawn = "true"))
    UPanelWidget* PreviousParent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drag and Drop", meta = (ExposeOnSpawn = "true"))
    bool RestoreParentOnFailure;

    UFUNCTION()
    void OnDragCancelled_Native(UDragDropOperation* Operation);
};
