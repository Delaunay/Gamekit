// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tools/GKEditorToolBase.h"
#include "GKGameplayAbilityEditorTool.generated.h"

/**
 * 
 */
UCLASS()
class GAMEKITED_API UGKGameplayAbilityEditorTool : public UGKEditorToolBase
{
	GENERATED_BODY()

public:
	UFUNCTION(Exec)
	static void GenerateGameplayAbilities();

	static void GenerateGameplayAbilitiesFromTable(FName Name, class UDataTable* Table);

	static void GenerateGameplayAbilityFromTable(FName RowName, class UDataTable* Table);

	static void GenerateUnitsFromTable(FName Name, class UDataTable* Table);

	// This is not saved/loaded
	UPROPERTY(EditAnywhere)
	bool bTestSomething;
};
