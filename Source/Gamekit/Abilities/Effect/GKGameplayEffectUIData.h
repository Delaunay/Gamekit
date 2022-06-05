// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Abilities/GKAbilityStatic.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "GameplayEffectUIData.h"

// Generated
#include "GKGameplayEffectUIData.generated.h"

/**
 * 
 */
UCLASS()
class GAMEKIT_API UGKGameplayEffectUIData : public UGameplayEffectUIData
{
	GENERATED_BODY()
public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect|Data")
    FText Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect|Data")
    FText Description;

    // Our gameplay effect reuse gameplay ability data as much as possible
    // -------------------------------------------------------------------
    
	//! DataTable used to fetch values for its data-driven effect
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Data")
	class UDataTable* AbilityDataTable;

	//! Row Name used to fetch the values for its data-driven effect
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Data")
	FName AbilityRowName;

    //! Retrieve the values from the DataTable for its data-driven effect
    //! copies the entire struct
    UFUNCTION(BlueprintCallable,
        Category = "Ability|Data",
        DisplayName = "GetAbilityStatic",
        meta = (ScriptName = "GetAbilityStatic"))
    void K2_GetAbilityStatic(FGKAbilityStatic& AbilityStatic, bool& Valid);

    //! Retrieve the values from the DataTable for its data-driven effect
    //! C++ version avoid to copy the entire struct
    FGKAbilityStatic* GetAbilityStatic();

    void LoadFromDataTable(FGKAbilityStatic& AbilityStatic);

    //! Called everytime the DataTable is modified, refresh the cached lookup
    UFUNCTION()
    void OnDataTableChanged_Native();

    //! Initialize data-driven effect after properties have been set
    //! i.e right after ``AbilityDataTable`` & ``AbilitRowName`` were set
    void PostInitProperties() override;

    //! Cached lookup, do not use!
    FGKAbilityStatic* AbilityStatic;
};
