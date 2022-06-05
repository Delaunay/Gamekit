// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.


#include "Abilities/Effect/GKGameplayEffectUIData.h"



void UGKGameplayEffectUIData::PostInitProperties()
{
    Super::PostInitProperties();

    if (AbilityDataTable && AbilityRowName.IsValid())
    {
        UE_LOG(LogGamekit, Log, TEXT("Loading From DataTable"));
        OnDataTableChanged_Native();
    }
}

void UGKGameplayEffectUIData::OnDataTableChanged_Native()
{
    // Reset Cache
    AbilityStatic = nullptr;
    bool Valid = false;

    FGKAbilityStatic* AbilityStaticOut = GetAbilityStatic();

    if (!AbilityStaticOut)
    {
        AbilityStatic = nullptr;
    }
}

FGKAbilityStatic* UGKGameplayEffectUIData::GetAbilityStatic()
{
    if (AbilityStatic)
    {
        return AbilityStatic;
    }

    if (!AbilityDataTable || !AbilityRowName.IsValid())
    {
        return nullptr;
    }

    // we do not have a cached value
    if (!AbilityStatic)
    {
        AbilityStatic = AbilityDataTable->FindRow<FGKAbilityStatic>(AbilityRowName, TEXT("Ability"), true);

        if (AbilityStatic != nullptr)
        {
            LoadFromDataTable(*AbilityStatic);
        }

        // Listen to data table change
        if (!AbilityDataTable->OnDataTableChanged().IsBoundToObject(this))
        {
            AbilityDataTable->OnDataTableChanged().AddUObject(this, &UGKGameplayEffectUIData::OnDataTableChanged_Native);
        }
    }

    return AbilityStatic;
}

void UGKGameplayEffectUIData::LoadFromDataTable(FGKAbilityStatic& Data) {

}

void UGKGameplayEffectUIData::K2_GetAbilityStatic(FGKAbilityStatic& AbilityStaticOut, bool& Valid)
{
    Valid = false;

    auto Result = GetAbilityStatic();
    if (Result != nullptr)
    {
        AbilityStaticOut = *Result;
        Valid = true;
    }
}