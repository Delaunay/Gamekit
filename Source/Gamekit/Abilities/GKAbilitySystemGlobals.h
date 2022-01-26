// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"

#include "GKAbilitySystemGlobals.generated.h"

/**
 *  
 */
UCLASS(config = Game)
class GAMEKIT_API UGKAbilitySystemGlobals: public UAbilitySystemGlobals
{
    GENERATED_UCLASS_BODY()
public:

	/** TryActivate failed due to the ability not being learned */
	UPROPERTY()
	FGameplayTag ActivateFailNotYetLearnedTag;

	UPROPERTY(config)
	FName ActivateFailNotYetLearnedName;

	UPROPERTY()
    FGameplayTag DeathTag;

	UPROPERTY(config)
	FName DeathName;

	UPROPERTY()
    FGameplayTag DeathDispelTag;

	UPROPERTY(config)
    FName DeathDispelName;

	virtual void InitGlobalTags() override;

	//
	// THINK: We could a secondary global curve here for our curve generation system
	//
};
