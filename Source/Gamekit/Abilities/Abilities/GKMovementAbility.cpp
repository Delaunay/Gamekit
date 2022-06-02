// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.


#include "Gamekit/Abilities/Abilities/GKMovementAbility.h"

// Gamekit
#include "Gamekit/Abilities/AbilityTasks/GKAbilityTask_MoveToDestination.h"


void UGKMovementAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData) 
{
	APlayerController* Controller = ActorInfo->PlayerController.Get();

	if (!Controller) {
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	FHitResult Out;
	Controller->GetHitResultUnderCursor(GroundChannel, false, Out);

	auto Task = UGKAbilityTask_MoveToDestination::MoveToDestination(
		this,
		NAME_None,
		Out.ImpactPoint, 
		10,						// Distance Tolerance 
		15,						// Angle Tolerance
		150,					// Turn Rate
		600,					// Speed
		true,					// Move To Target (i.e not rot only)
		EGK_AbilityBehavior::PointTarget,
		false					// Debug
	);

	Task->OnCompleted.AddDynamic(this, &UGKMovementAbility::OnMovementEnded);
	Task->OnCancelled.AddDynamic(this, &UGKMovementAbility::OnMovementEnded);
}

void UGKMovementAbility::OnMovementEnded(const FGameplayAbilityTargetDataHandle& TargetData) {
	K2_EndAbility();
}
