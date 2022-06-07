// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GKGameplayAbility.h"
#include "GKAttackAbility.generated.h"

/**
 * Attack loop, play the attack animation on loop
 */
UCLASS()
class GAMEKIT_API UGKAttackAbility : public UGKGameplayAbility
{
	GENERATED_BODY()


public:
	// New animation is going to take over to restart the entire activation process
	// but skip the target acquisition step
	virtual void OnAbilityAnimationBlendOut(FGameplayTag EventTag, FGameplayEventData EventData) override {
		OnAbilityTargetAcquired(EventData.TargetData);
	}
};
