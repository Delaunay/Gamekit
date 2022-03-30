// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Abilities/Targeting/GKAbilityTarget_Actor.h"

// Unreal Engine
#include "Abilities/GameplayAbility.h"
#include "GameplayAbilitySpec.h"
#include "GameFramework/PlayerController.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

// deprecate this
void AGKAbilityTarget_Actor::StartTargeting(class UGameplayAbility* Ability) {
	auto GKAbility = Cast<UGKGameplayAbility>(Ability);
	if (GKAbility) {
		StartTargeting(GKAbility);
	}
}


AGKAbilityTarget_Actor::AGKAbilityTarget_Actor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickEnabled = false;
}

void AGKAbilityTarget_Actor::StartTargeting(UGKGameplayAbility* Ability) {
	OwningAbility = Ability;

	const FGameplayAbilityActorInfo* ActorInfo = (OwningAbility ? OwningAbility->GetCurrentActorInfo() : nullptr);
	AbilitySystemComponent = (ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr);

	// User ability activation is inhibited while this is active
	if (AbilitySystemComponent) {
		AbilitySystemComponent->SpawnedTargetActors.Add(this);
	}

	auto Data = Ability->GetAbilityStatic();
    if (Data)
    {
        InitializeFromAbilityData(*Data);
    }
}

void AGKAbilityTarget_Actor::InitializeFromAbilityData(FGKAbilityStatic const& AbilityData) {
	K2_InitializeFromAbilityData(AbilityData);
}

void AGKAbilityTarget_Actor::EnableUserInput() {
	SetActorTickEnabled(bTickEnabled);
	SetActorHiddenInGame(false);
}

void AGKAbilityTarget_Actor::StopTargeting() {
	// Remove all handler so the state is clean
	TargetDataReadyDelegate.Clear();
	CanceledDelegate.Clear();

	SetActorTickEnabled(false);
	SetActorHiddenInGame(true);

	if (AbilitySystemComponent) {
		AbilitySystemComponent->SpawnedTargetActors.Remove(this);
	}
}

void AGKAbilityTarget_Actor::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	// We should not reply on EndPlay anyway, because we want to reuse this actor all the time
	// Do not call ``AGameplayAbilityTargetActor::EndPlay`` on purpose
	AActor::EndPlay(EndPlayReason);
}