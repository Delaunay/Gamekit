#pragma once

#include "Blueprint/UserWidget.h"

#include "GKWidgetDropZone.generated.h"


/*! Simple Drop Zone for draggable widget, simply implement UGKWidgetDropZone::WidgetDrop method
 */
UCLASS(BlueprintType, Blueprintable, meta = (DisableNativeTick))
class GAMEKIT_API UGKWidgetDropZone : public UUserWidget
{
    GENERATED_BODY()

public:
    //! Handle dropping of a widget
    bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

    //! Returns true if the widget was successfully dropped
    UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Drag and Drop")
    bool WidgetDrop(UUserWidget* Widget, FVector2D LocalLoc);
};
