// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.


#include "Gamekit/Abilities/Abilities/GKMovementAbility.h"

// Gamekit
#include "Gamekit/Abilities/GKAbilities.h"
#include "Gamekit/Abilities/AbilityTasks/GKAbilityTask_MoveToDestination.h"
#include "Gamekit/Abilities/GKAbilitySystemComponent.h"
#include "Gamekit/Abilities/GKAttributeSet.h"
#include "Gamekit/Abilities/GKAbilitySystemGlobals.h"

// Unreal Engine
#include "GameFramework/PlayerController.h"


UGKMovementAbility::UGKMovementAbility() {
	GroundChannel = ECollisionChannel::ECC_WorldStatic;

	// The exclusiveness is meant to avoid cancelling previously casting ability
	// in the case of movement we do not mind cancelling the movement

	AbilityTags.AddTag(AbilityMove);
	// this ability is blocked by other exclusive abilities
	// but it does not block exclusive abilities
	AbilityTags.AddTag(AbilityExclusive);

	// Does not block exclusive ability i.e
	// we can start targeting while moving
	// but we cannot start targeting while casting

	// Another option would be to allow the movement ability to be started
	// but the task would only start once the Exclusive tag is removed
	// this would allow users to queue the move right away and simulate a kind of
	// ability queue
	BlockAbilitiesWithTag.RemoveTag(AbilityExclusive); // <= REMOVE

	// Is there instances where `ActivationBlockedTags` is not the same as CancelledByTags
	ActivationBlockedTags.AddTag(DisableRoot);
	ActivationBlockedTags.AddTag(DisableStun);
	ActivationBlockedTags.AddTag(DisableDead);

	CancelledByTags.AddTag(DisableRoot);
	CancelledByTags.AddTag(DisableStun);
	CancelledByTags.AddTag(DisableDead);
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

	// New prediction key
	FPredictionKey PredKey = ActivationInfo.GetActivationPredictionKey();

	// this is server side, activation is done until server receives the  targt data
	ActorInfo->AbilitySystemComponent->AbilityTargetDataSetDelegate(Handle, PredKey).AddUObject(this, &UGKMovementAbility::ReplicatedTargetData);

	// Make the target data right now
	FHitResult Out;
	if (!Controller->GetHitResultUnderCursor(GroundChannel, false, Out)) {
		return;
	}

	FGameplayAbilityTargetDataHandle CursorHitTarget;

	// this is gets cleanup by a shared ptr internally
	FGameplayAbilityTargetData_SingleTargetHit* ReturnData = new FGameplayAbilityTargetData_SingleTargetHit();
	ReturnData->HitResult = Out;
	CursorHitTarget.Add(ReturnData);
	
	// Send Target Data to server
	FGameplayTag ApplicationTag;
	ActorInfo->AbilitySystemComponent->CallServerSetReplicatedTargetData(
		Handle,
		PredKey,
		CursorHitTarget,
		ApplicationTag,
		ActorInfo->AbilitySystemComponent->ScopedPredictionKey
	);


	ExecuteMove(
		Out,
		Handle,
		ActorInfo,
		ActivationInfo,
		TriggerEventData
	);
}


void UGKMovementAbility::ReplicatedTargetData(const FGameplayAbilityTargetDataHandle & Data, FGameplayTag ActivationTag){

	FHitResult const& Out = *Data.Get(0)->GetHitResult();

	GetCurrentActorInfo()->AbilitySystemComponent->ConsumeClientReplicatedTargetData(
		GetCurrentAbilitySpecHandle(),
		GetCurrentActivationInfo().GetActivationPredictionKey()
	);

	ExecuteMove(
		Out, 
		GetCurrentAbilitySpecHandle(),
		GetCurrentActorInfo(),
		GetCurrentActivationInfo(), 
		nullptr);
}

void UGKMovementAbility::ExecuteMove(FHitResult const& Out,
									 const FGameplayAbilitySpecHandle Handle,
								 	 const FGameplayAbilityActorInfo * ActorInfo,
								 	 const FGameplayAbilityActivationInfo ActivationInfo,
									 const FGameplayEventData * TriggerEventData)
{
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
		false,
		CancelTags,
		true,					// UsePathfinding
		false 					// Debug
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
