// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.
#include "Gamekit/Abilities/GKAbilitySystemGlobals.h"

UGKAbilitySystemGlobals::UGKAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) 
{}


void UGKAbilitySystemGlobals::InitGlobalTags() {
	UAbilitySystemGlobals::InitGlobalTags();

	if (ActivateFailNotYetLearnedName != NAME_None)
	{
		ActivateFailNotYetLearnedTag = FGameplayTag::RequestGameplayTag(ActivateFailNotYetLearnedName);
	}
}
