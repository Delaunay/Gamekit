// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Abilities/GKGameplayAbility.h"

// Unreal Engine
#include "CoreMinimal.h"

// Generated
#include "GKMovementAbility.generated.h"

/** Simple movement ability, go torwards the location under the cursor. 
 * 
 *  Note this assume `ECollisionChannel::ECC_WorldStatic` as the collision channel for ground
 */
UCLASS()
class GAMEKIT_API UGKMovementAbility : public UGKGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGKMovementAbility();

	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, 
					     const FGameplayAbilityActorInfo* ActorInfo, 
		                 const FGameplayAbilityActivationInfo ActivationInfo, 
		                 const FGameplayEventData* TriggerEventData) override;


	// Trace that always block
	UPROPERTY()
	TEnumAsByte<ECollisionChannel> GroundChannel;

	UFUNCTION()
	void OnMovementEnded(const FGameplayAbilityTargetDataHandle& TargetData);

	UFUNCTION()
	void OnMovementCancelled(const FGameplayAbilityTargetDataHandle& Data);

	class UGKAbilityTask_MoveToDestination* Task;

	void ReplicatedTargetData(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag);

	void ExecuteMove(
		FHitResult const& Out,
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData);
};
