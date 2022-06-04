// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.


#include "Gamekit/Abilities/Abilities/GKMovementAbility.h"

// Gamekit
#include "Gamekit/Abilities/AbilityTasks/GKAbilityTask_MoveToDestination.h"
#include "Gamekit/Abilities/GKAbilitySystemComponent.h"
#include "Gamekit/Abilities/GKAttributeSet.h"


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

	UGKAttributeSet const* Attributes = Cast<UGKAttributeSet>(ActorInfo->AbilitySystemComponent->GetAttributeSet(UGKAttributeSet::StaticClass()));

	auto Task = UGKAbilityTask_MoveToDestination::MoveToDestination(
		this,
		NAME_None,
		Out.ImpactPoint, 
		10,							// Distance Tolerance 
		15,							// Angle Tolerance
		150,						// Turn Rate
		Attributes->GetMoveSpeed(), // Speed
		true,						// Move To Target (i.e not rot only)
		EGK_AbilityBehavior::PointTarget,
		false					// Debug
	);

	Task->OnCompleted.AddDynamic(this, &UGKMovementAbility::OnMovementEnded);
	Task->OnCancelled.AddDynamic(this, &UGKMovementAbility::OnMovementEnded);

	Task->ReadyForActivation();
}

void UGKMovementAbility::OnMovementEnded(const FGameplayAbilityTargetDataHandle& TargetData) {
	K2_EndAbility();
}
