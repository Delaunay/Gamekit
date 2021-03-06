// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

// include
#include "Tools/GKGameplayAbilityEditorTool.h"


// GamekitEd
#include "GamekitEd/GamekitEd.h"

// Gamekit
#include "Gamekit/Abilities/GKAbilityStatic.h"
#include "Gamekit/Characters/GKUnitStatic.h"
#include "Gamekit/GKGamekitSettings.h"

// Unreal Engine
#include "Factories/BlueprintFactory.h"
#include "IPythonScriptPlugin.h"
#include "AssetToolsModule.h"
#include "EditorAssetLibrary.h"


#define CODE(X) #X

// the ; are necessary because the preprocessor is stripping the newlines
// but python is fine with inline expression as long as they are split with ;
FString GenerateAbilityCommand = CODE(
from importlib import reload;
import gamekit.abilities as abilities;

reload(abilities);
abilities.generate_gamekit_abilities()
);

void UGKGameplayAbilityEditorTool::GenerateGameplayAbilities() 
{
    IPythonScriptPlugin::Get()->ExecPythonCommand(*GenerateAbilityCommand);
}

UClass* GetDefaultParentAbility() {
    UBlueprintGeneratedClass* Obj = LoadObject<UBlueprintGeneratedClass>(
        nullptr, 
        TEXT("/Gamekit/Abilities/GAC_AbilityBase_Prototype.GAC_AbilityBase_Prototype_C")
    );

    // This is what python was doing but seems a bit redundant
    return GetDefault<UBlueprintGeneratedClass>(Obj->GetClass())->GetClass();
}


void GenerateAbility_CPP(FName AbilityName, FString Destination, class UDataTable* Table) {

    FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");

    UBlueprintFactory* BlueprintFactory = NewObject<UBlueprintFactory>(GetTransientPackage());
    // GetDefaultParentAbility();
    BlueprintFactory->ParentClass = UGKGameplayAbility::StaticClass();
    BlueprintFactory->bEditAfterNew = false;
    BlueprintFactory->SupportedClass = UBlueprint::StaticClass();

    FString Name = FString::Format(TEXT("GA_{0}"), { AbilityName.ToString()});
    FString BP_Name = FString::Format(TEXT("{0}/{1}.{1}_C"), { Destination , Name });

    UObject* NewAbility = AssetToolsModule.Get().CreateAsset(
        Name, 
        Destination,
        nullptr, 
        BlueprintFactory
    );

    if (NewAbility) {
        // Saving here force the generation of the BlueprintGeneratedClass
        UEditorAssetLibrary::SaveLoadedAsset(NewAbility);

        UBlueprintGeneratedClass* AbilityBP = LoadObject<UBlueprintGeneratedClass>(nullptr, *BP_Name);

        ensure(AbilityBP);

        UBlueprintGeneratedClass* AbilityCDO = GetMutableDefault<UBlueprintGeneratedClass>(AbilityBP->GetClass());

        if (AbilityCDO) {
            // This is how far I got to port the python script into C++
            // that part of the code seems to have no helpers
           
            /*
            FProperty* TableProp = AbilityCDO->FindPropertyByName("AbilityDatatable");
            FObjectPropertyBase* TableObj = CastField<FObjectPropertyBase>(Prop)
            TableObj->SetObjectPropertyValue(ValueAddr, NewValue);

            FProperty* RowProp = AbilityCDO->FindPropertyByName("AbilityRowName");
            FNameProperty* RowName = CastField<FNameProperty>(RowProp);
            RowName->

            UEditorAssetLibrary::SaveLoadedAsset(AbilityCDO);
            */
        }
    }
}

void GenerateGameplayAbilitiesFromTable_CPP(FName Name, class UDataTable* Table) 
{
    UE_LOG(LogGamekitEd, Log, TEXT("Auto Regenerating abilities for %s"), *Name.ToString());

    UGKGamekitSettings* Settings = UGKGamekitSettings::Get();
    FString Destination = Settings->AbilityOutput;

    for(FName RowName: Table->GetRowNames()) {
        FGKAbilityStatic* Row = Table->FindRow<FGKAbilityStatic>(RowName, "", false);

        if (!Row->bAutoGenerate){
            continue;
        }
    
        FString FullAssetPath = FString::Format(TEXT("{0}/GA_{1}"), { Destination, RowName.ToString() });

        if (!UEditorAssetLibrary::DoesAssetExist(FullAssetPath)) {
            GenerateAbility_CPP(RowName, Destination, Table);
        } else {
            UE_LOG(LogGamekitEd, Log, TEXT("Skipping asset %s"), *FullAssetPath);
        }
    }
}


FString GenerateAbilitiesTemplate = TEXT(
    "from importlib import reload\n"
    "import gamekit.abilities as abilities\n"
    "\n"
    "reload(abilities)\n"
    "abilities.generate_abilities_from_table(\"{0}\", \"{1}\")\n"
);

void GenerateGameplayAbilitiesFromTable_Python(FName Name, class UDataTable* Table)
{
    UGKGamekitSettings* Settings = UGKGamekitSettings::Get();
    FString Destination = Settings->AbilityOutput;

    FSoftObjectPath DataTablePath = Table;

    FString PythonCode = FString::Format(*GenerateAbilitiesTemplate, { DataTablePath.ToString(), Destination });

    IPythonScriptPlugin::Get()->ExecPythonCommand(*PythonCode);

}
void UGKGameplayAbilityEditorTool::GenerateGameplayAbilitiesFromTable(FName Name, class UDataTable* Table) {
    GenerateGameplayAbilitiesFromTable_Python(Name, Table);
}

FString GenerateAbilityTemplate = TEXT(
    "from importlib import reload\n"
    "import gamekit.abilities as abilities\n"
    "\n"
    "reload(abilities)\n"
    "abilities.generate_ability(\"{0}\", \"{1}\", \"{2}\")\n"
);


void UGKGameplayAbilityEditorTool::GenerateGameplayAbilityFromTable(FName RowName, class UDataTable* Table) {
    UGKGamekitSettings* Settings = UGKGamekitSettings::Get();
    FString Destination = Settings->AbilityOutput;

    FSoftObjectPath DataTablePath = Table;

    FString PythonCode = FString::Format(*GenerateAbilityTemplate, { 
        DataTablePath.ToString(), 
        RowName.ToString(),
        Destination 
    });

    IPythonScriptPlugin::Get()->ExecPythonCommand(*PythonCode);
}

void UGKGameplayAbilityEditorTool::GenerateUnitsFromTable(FName Name, class UDataTable* Table) {
    for (FName RowName : Table->GetRowNames()) {
        FGKUnitStatic* Row = Table->FindRow<FGKUnitStatic>(RowName, "", false);
    }
}
