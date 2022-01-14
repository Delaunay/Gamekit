// BSD 3-Clause License Copyright (c) 2021, Pierre Delaunay All rights reserved.

#pragma once

#include "Gamekit.h"

#include "AbilitySystemComponent.h"

#include "Abilities/GKAbilityTypes.h"
#include "Abilities/Targeting/GKAbilityTarget_Actor.h"

#include "GKAbilitySystemComponent.generated.h"

class UGKGameplayAbility;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGKAbilityFailedDelegate, const UGameplayAbility*, Ability, const FGameplayTagContainer&, FailureReason);

/**
 *
 */
UCLASS(BlueprintType, Blueprintable)
class GAMEKIT_API UGKAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	// Constructors and overrides
	UGKAbilitySystemComponent();

	//! Version of function in AbilitySystemGlobals that returns correct type
	static UGKAbilitySystemComponent* GetAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent = false);

	//! Used to receive the Granted ability through network
	void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;

	UFUNCTION(BlueprintCallable)
	bool IsInitialized() const;

	/*! Called when the ability fails to activate
	 * /rst
	 * .. note::
	 *
	 *    You should set GAS global failure tags in the config else you will receive an empty container
	 *    of GameplayTags
	 * 
	 * /endrst
	 */
	UPROPERTY(BlueprintAssignable)
	FGKAbilityFailedDelegate OnAbilityFailed;

	//! Called when the Activate fails to active with the failure reason
	void OnAbilityFailed_Native(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason);

	//! Stop any abilities currently being activated, cancel backswing animation
	UFUNCTION(BlueprintCallable, Category = Abilities)
	void CancelAllPendingAbilities();

	//! Level up the given ability
	UFUNCTION(BlueprintCallable, Category = Abilities)
	void LevelUpAbility(FGameplayAbilitySpecHandle Handle);

	UFUNCTION(Server, Reliable)
	void ServerLevelUpAbility(FGameplayAbilitySpecHandle Handle);

	UFUNCTION(Client, Reliable)
	void ClientLevelUpAbility_Result(FGameplayAbilitySpecHandle Handle, int Level);

// protected:
	bool Initialized;

	friend class AGKGASCharacter;
	friend class UGKGameplayAbility;

protected:
	UPROPERTY()
	TMap<TSubclassOf<AGKAbilityTarget_Actor>, AGKAbilityTarget_Actor*> AbilityTarget_ActorCache;

	static TSubclassOf<AGKAbilityTarget_Actor> GetAbilityTarget_ActorClass(EGK_TargetingMode Mode);

public:
	AGKAbilityTarget_Actor* GetAbilityTarget_Actor(TSubclassOf<AGKAbilityTarget_Actor> AbilityTarget_ActorClass);

	AGKAbilityTarget_Actor* GetAbilityTarget_Actor(EGK_TargetingMode Mode);
};