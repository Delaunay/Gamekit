// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.


#include "Gamekit/Abilities/Abilities/GKMovementAbility.h"

// Gamekit
#include "Gamekit/Abilities/AbilityTasks/GKAbilityTask_MoveToDestination.h"
#include "Gamekit/Abilities/GKAbilitySystemComponent.h"
#include "Gamekit/Abilities/GKAttributeSet.h"


UGKMovementAbility::UGKMovementAbility() {
	GroundChannel = ECollisionChannel::ECC_WorldStatic;

	static FGameplayTag Root = FGameplayTag::RequestGameplayTag("Debuff.Root");
	static FGameplayTag Stun = FGameplayTag::RequestGameplayTag("Debuff.Stun");
	static FGameplayTag Dead = FGameplayTag::RequestGameplayTag("State.Dead");
	static FGameplayTag Move = FGameplayTag::RequestGameplayTag("Ability.Move");

	AbilityTags.AddTag(Move);

	ActivationBlockedTags.AddTag(Root);
	ActivationBlockedTags.AddTag(Stun);
	ActivationBlockedTags.AddTag(Dead);
}

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

	Task = UGKAbilityTask_MoveToDestination::MoveToDestination(
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
	Task->OnCancelled.AddDynamic(this, &UGKMovementAbility::OnMovementCancelled);

	FGameplayTagContainer OutTags;
	CommitAbility(Handle, ActorInfo, ActivationInfo, &OutTags);
	Task->ReadyForActivation();
}

void UGKMovementAbility::OnMovementEnded(const FGameplayAbilityTargetDataHandle& TargetData) {
	if (IsValid(Task)) {
		OnGameplayAbilityCancelled.RemoveAll(Task);
		Task = nullptr;
	}
	K2_EndAbility();
}

void UGKMovementAbility::OnMovementCancelled(const FGameplayAbilityTargetDataHandle& Data) {
	if (IsValid(Task)) {
		Task->EndTask();
		OnGameplayAbilityCancelled.RemoveAll(Task);
		Task = nullptr;
	}
	K2_CancelAbility();
}
