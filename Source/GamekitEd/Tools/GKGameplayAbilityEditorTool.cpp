// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

// include
#include "Tools/GKGameplayAbilityEditorTool.h"


// GamekitEd
#include "GamekitEd/GamekitEd.h"

// Gamekit
#include "Gamekit/Abilities/GKAbilityStatic.h"
#include "Gamekit/Characters/GKUnitStatic.h"

// Unreal Engine
#include "IPythonScriptPlugin.h"


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


void GenerateAbility() {

}

void UGKGameplayAbilityEditorTool::GenerateGameplayAbilitiesFromTable(FName Name, class UDataTable* Table) 
{
    for(FName RowName: Table->GetRowNames()) {
        FGKAbilityStatic* Row = Table->FindRow<FGKAbilityStatic>(RowName, "", false);
    
    }
}

void UGKGameplayAbilityEditorTool::GenerateUnitsFromTable(FName Name, class UDataTable* Table) {
    for (FName RowName : Table->GetRowNames()) {
        FGKUnitStatic* Row = Table->FindRow<FGKUnitStatic>(RowName, "", false);
    }
}
