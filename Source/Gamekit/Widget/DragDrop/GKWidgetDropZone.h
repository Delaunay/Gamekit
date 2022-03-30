#pragma once

// Unreal Engine
#include "Blueprint/UserWidget.h"

// Generated
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
   
    // Implement to handle drop
    // bool OnDrop(FGeometry MyGeometry, FPointerEvent PointerEvent, UDragDropOperation* Operation);

    // Implement to handle pending drop
    // bool OnDragOver(FGeometry MyGeometry, FPointerEvent PointerEvent, UDragDropOperation *Operation);
};
