#pragma once

// Unreal Engine
#include "IDetailCustomization.h"

// Generated
#include "GKEditorToolBase.generated.h"

UCLASS(Blueprintable, Abstract)
class UGKEditorToolBase: public UObject
{
    GENERATED_UCLASS_BODY()

};

/* Add buttons for every functions inside an object that is marked Exec
 */
struct FGKBaseEditorToolCustomization: public IDetailCustomization
{
    static TSharedRef<IDetailCustomization> MakeInstance();

    // modify layouts
    virtual void CustomizeDetails(IDetailLayoutBuilder &DetailBuilder) override;

    // Execute the functions
    static FReply ExecuteToolCommand(IDetailLayoutBuilder *DetailBuilder, UFunction *MethodToExecute);
};