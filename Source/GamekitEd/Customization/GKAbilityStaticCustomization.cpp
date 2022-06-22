#include "GamekitEd/Customization/GKAbilityStaticCustomization.h"

// Gamekit
#include "Gamekit/Abilities/GKAbilityStatic.h"

// Gamekit Ed
#include "GamekitEd/GamekitEd.h"
#include "GamekitEd/Tools/GKGameplayAbilityEditorTool.h"

// Unreal Engine
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"

// Unreal Engine private
// #include "E:\UnrealEngine\Engine\Source\Editor\PropertyEditor\Private\PropertyNode.h"
// #include "PropertyNode.h"
// #include "PropertyEditor/PropertyNode.h"
// #include "Editor/PropertyEditor/PropertyNode.h"


TSharedRef<IDetailCustomization> FGKAbilityStaticCustomization::MakeInstance()
{
    return MakeShareable(new FGKAbilityStaticCustomization);
}

void FGKAbilityStaticCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    TArray<TSharedPtr<FStructOnScope>> StructToCustomize;

    DetailBuilder.GetStructsBeingCustomized(StructToCustomize);

    ensure(StructToCustomize.Num() == 1);

    // New category for our commands
    IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("Commands");
    Category.SetSortOrder(-1);

    FText Caption = NSLOCTEXT("GamekitEd", "Ability Generate", "Generate");

    // Add a button to the category
    // clang-format off
    FDetailWidgetRow& Row = Category.AddCustomRow(Caption);

    Row.ValueContent()
        [
            SNew(SButton)
            .Text(Caption)
            .OnClicked(FOnClicked::CreateStatic(&FGKAbilityStaticCustomization::ExecuteGenerateAbility, &DetailBuilder, StructToCustomize[0]))
        ];
    // clang-format on

}

// Private Accessor
// E:\UnrealEngine\Engine\Source\Editor\DataTableEditor\Private\SRowEditor.cpp
struct FGKStructFromDataTable : public FStructOnScope
{
    TWeakObjectPtr<UDataTable> DataTable;
    FName RowName;
};

FReply FGKAbilityStaticCustomization::ExecuteGenerateAbility(IDetailLayoutBuilder* DetailBuilder, TSharedPtr<FStructOnScope> Struct) {
    /*
    TStructOnScope<FGKAbilityStatic> Data;
    Data.InitializeFrom(*Struct.Get());
    FGKAbilityStatic* AbilityData = Data.Get();
    */

    // Pretty unsafe
    FGKStructFromDataTable* DataTableInfo = (FGKStructFromDataTable*)(Struct.Get());

    UDataTable* Table = DataTableInfo->DataTable.Get();
    FName RowName = DataTableInfo->RowName;

    UGKGameplayAbilityEditorTool::GenerateGameplayAbilityFromTable(RowName, Table);

    return FReply::Handled();
}
