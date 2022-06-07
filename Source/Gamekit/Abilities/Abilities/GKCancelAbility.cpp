// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

// Header
#include "Gamekit/Abilities/Abilities/GKCancelAbility.h"

// Unreal Engine
#include "AbilitySystemComponent.h"


void UGKCancelAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData) 
{
	CommitAbility(Handle, ActorInfo, ActivationInfo);
	ActorInfo->AbilitySystemComponent->CancelAllAbilities();
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}
