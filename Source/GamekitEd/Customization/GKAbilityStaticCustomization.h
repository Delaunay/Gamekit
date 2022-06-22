#pragma once

// Unreal Engine
#include "IDetailCustomization.h"

// Generated
// #include "GKDataTableCustomization.generated.h"

/* This does get triggered when the class we register is a USTRUCT
 * but `GetStructsBeingCustomized` does not return anything 
 */
struct FGKAbilityStaticCustomization : public IDetailCustomization
{
    static TSharedRef<IDetailCustomization> MakeInstance();

    // modify layouts
    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

    static FReply ExecuteGenerateAbility(IDetailLayoutBuilder* DetailBuilder, TSharedPtr<FStructOnScope> StructInstance);
};

