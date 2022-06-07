// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Abilities/GKGameplayAbility.h"

// Unreal Engine
#include "CoreMinimal.h"

// Generated
#include "GKCancelAbility.generated.h"

/**
 * Simple ability that cancels all the other abilities
 */
UCLASS()
class GAMEKIT_API UGKCancelAbility : public UGKGameplayAbility
{
	GENERATED_BODY()

public:

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override
	{
		return true;
	}

	void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
					 	 const FGameplayAbilityActorInfo* ActorInfo,
					  	 const FGameplayAbilityActivationInfo ActivationInfo,
						 const FGameplayEventData* TriggerEventData) override;
};
