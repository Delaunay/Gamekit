// Include
#include "GamekitEd/Tools/GKEditorToolBase.h"

// Unreal Engine
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"

UGKEditorToolBase::UGKEditorToolBase(class FObjectInitializer const&) {

}


TSharedRef<IDetailCustomization> FGKBaseEditorToolCustomization::MakeInstance() 
{
    return MakeShareable(new FGKBaseEditorToolCustomization);
}

void FGKBaseEditorToolCustomization::CustomizeDetails(IDetailLayoutBuilder &DetailBuilder)
{
    TArray<TWeakObjectPtr<UObject>> ObjectsToCustomize;
    DetailBuilder.GetObjectsBeingCustomized(ObjectsToCustomize);

    TSet<UClass *> Classes;
    for (auto WeakObject: ObjectsToCustomize)
    {
        UObject *Instance = WeakObject.Get();
        if (Instance)
        {
            Classes.Add(Instance->GetClass());
        }
    }

    // New category for our commands
    IDetailCategoryBuilder &Category = DetailBuilder.EditCategory("Commands");

    // Find all functions
    for (UClass *Class: Classes)
    {
        for (TFieldIterator<UFunction> FuncIt(Class); FuncIt; ++FuncIt)
        {
            UFunction *Function = *FuncIt;
            if (Function->HasAnyFunctionFlags(FUNC_Exec) && (Function->NumParms == 0))
            {
                const FString FunctionName = Function->GetName();
                const FText   Caption      = FText::FromString(FunctionName);

                // Add a button to the category
                // clang-format off
                Category.AddCustomRow(Caption)
                        .ValueContent()
                [
                    SNew(SButton)
                    .Text(Caption)
                    .OnClicked(FOnClicked::CreateStatic(&FGKBaseEditorToolCustomization::ExecuteToolCommand, &DetailBuilder, Function))
                ];
                // clang-format on
            }
        }
    }
}

FReply FGKBaseEditorToolCustomization::ExecuteToolCommand(IDetailLayoutBuilder *DetailBuilder,
                                                          UFunction            *MethodToExecute)
{
    TArray<TWeakObjectPtr<UObject>> ObjectsToCustomize;
    DetailBuilder->GetObjectsBeingCustomized(ObjectsToCustomize);

    for (auto WeakObject: ObjectsToCustomize)
    {
        UObject *Instance = WeakObject.Get();
        if (Instance)
        {
            Instance->CallFunctionByNameWithArguments(*MethodToExecute->GetName(), *GLog, nullptr, true);
        }
    }

    return FReply::Handled();
}
