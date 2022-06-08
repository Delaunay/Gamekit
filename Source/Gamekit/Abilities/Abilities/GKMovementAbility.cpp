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
	static FGameplayTag Exclusive = FGameplayTag::RequestGameplayTag("Ability.Exclusive");

	AbilityTags.AddTag(Move);
	// this ability is blocked by other exclusive abilities
	// but it does not block exclusive abilities
	AbilityTags.AddTag(Exclusive);

	// Does not block exclusive ability i.e
	// we can start targeting while moving
	// but we cannot start targeting while casting

	// Another option would be to allow the movement ability to be started
	// but the task would only start once the Exclusive tag is removed
	// this would allow users to queue the move right away and simulate a kind of
	// ability queue
	BlockAbilitiesWithTag.RemoveTag(Exclusive);

	// Is there instances where `ActivationBlockedTags` is not the same as CancelledByTags
	ActivationBlockedTags.AddTag(Root);
	ActivationBlockedTags.AddTag(Stun);
	ActivationBlockedTags.AddTag(Dead);

	CancelledByTags.AddTag(Stun);
	CancelledByTags.AddTag(Root);
	CancelledByTags.AddTag(Dead);
}

void UGKMovementAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData) 
{
	APlayerController* Controller = ActorInfo->PlayerController.Get();

	SetupCancelByTags(ActorInfo);

	if (!Controller) {
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	FHitResult Out;
	Controller->GetHitResultUnderCursor(GroundChannel, false, Out);

	UGKAttributeSet const* Attributes = Cast<UGKAttributeSet>(ActorInfo->AbilitySystemComponent->GetAttributeSet(UGKAttributeSet::StaticClass()));

	// cancel all the another abilities 
	auto ASC = GetAbilitySystemComponentFromActorInfo();
	ASC->CancelAbilities(nullptr, nullptr, this);

	FGameplayTagContainer CancelTags;
	// CancelTags.AddTag(FGameplayTag::RequestGameplayTag("Debuff.Stun"));
	// CancelTags.AddTag(FGameplayTag::RequestGameplayTag("Debuff.Root"));

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
		CancelTags,
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
