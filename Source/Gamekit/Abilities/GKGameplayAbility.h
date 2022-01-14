// BSD 3-Clause License Copyright (c) 2021, Pierre Delaunay All rights reserved.

#pragma once

#include "Gamekit.h"

#include "Abilities/GameplayAbility.h"
#include "Abilities/GKAbilityStatic.h"
#include "Abilities/GKAbilityTypes.h"

#include "GKGameplayAbility.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGKCancelBackswingDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGKTargetingStartDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGKTargetingResultDelegate, bool, TargetCancelled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGKAbilityLevelUpDelegate, int, Level);



/* Dynamic Gameplay effect that is initialized from a DataTable
 */
UCLASS(Blueprintable)
class GAMEKIT_API UGKGameplayEffectDyn : public UGameplayEffect
{
public:
	GENERATED_UCLASS_BODY()

public:
	// Overide the Networking functions
	bool IsNameStableForNetworking() const override {
		return true;
	}

	bool IsSupportedForNetworking() const override {
		return true;
	}
};


/** Base Gameplay Ability to implement new skill/item/actions
 *
 * For ability with charges, just make the ability consume charges on activation
 * remove cooldown, give an effect that grant charges overtime
 * Add the Charge Gametag the the Ability Tag Requirement
 */
UCLASS()
class GAMEKIT_API UGKGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	/*! Implements the generic activation flow of an ability in GameKit
	 * \rst
	 *
	 * .. image:: /_static/AbilityActivationFlow.png
	 *
     * \endrst
	 */
	UFUNCTION(BlueprintCallable, Category = Ability)
	void ActivateAbility_Native();

	//! Activation function for an hidden activity that does not show in UI
	void ActivateAbility_Hidden();

	//! Passive abilities are activated when granted
	void ActivateAbility_Passive();

	//! Ability that does not require a target to bet cast
	void ActivateAbility_NoTarget();

	//! Ability that targets another actor
	void ActivateAbility_ActorTarget();

	//! Ability that targets an area on the ground
	void ActivateAbility_PointTarget();

	//! Ability that is Toggled on/off
	void ActivateAbility_Toggle();

public:
	// Constructor and overrides
	UGKGameplayAbility();

	// DataTable
	// ---------

	//! DataTable used to fetch values for its data-driven effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability|Data")
	class UDataTable* AbilityDataTable;

	//! Row Name used to fetch the values for its data-driven effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability|Data")
	FName AbilityRowName;

	//! Retrieve the values from the DataTable for its data-driven effect
	//! copies the entire struct
	UFUNCTION(BlueprintCallable, Category = "Ability|Data", DisplayName = "GetAbilityStatic", meta = (ScriptName = "GetAbilityStatic"))
	void K2_GetAbilityStatic(FGKAbilityStatic& AbilityStatic, bool& Valid);

	//! Retrieve the values from the DataTable for its data-driven effect
	//! C++ version avoid to copy the entire struct
	FGKAbilityStatic* GetAbilityStatic();

	//! Called everytime the DataTable is modified, refresh the cached lookup
	UFUNCTION()
	void OnDataTableChanged_Native();

	//! Called everytime the Static data is loaded (init & reimport)
	virtual void LoadFromDataTable(FGKAbilityStatic& AbilityDef);

	//! Initialize data-driven effect after properties have been set
	//! i.e right after ``AbilityDataTable`` & ``AbilitRowName`` were set
	void PostInitProperties() override;

	//! Cached lookup, do not use!
	FGKAbilityStatic* AbilityStatic;

	// Animations
	// ----------
public:
	//! Lookup the animation montage to use before cast this ability
	UFUNCTION(BlueprintCallable)
	class UAnimMontage* GetAnimation();

	//! Animation to play before activating the ability
	//! Cached from the Character animation set 
	//! NB: If set as an ability property it will disable the animation set lookup 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	FGKAnimationArray AnimMontages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	FName StartSection = NAME_None;

	//! Speed at which the animation is playing, overriden if DyanmicCastPoint=true
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	float Rate = 1.f;

	//! The animation will play at a rate that will keep the cast-time
	//! constant across different animations.
	//! Useful if animations send the AbilityNotify at different times
	//! but the ability need to keep a consistent cast time
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	bool DyanmicCastPoint = true;

	//! If immediate == true, the animation is not played
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	bool Immediate;

	UPROPERTY()
	class UGKAbilityTask_PlayMontageAndWaitForEvent* AnimTask;

protected:
	//! New animation is taking over, make sure the ability is in a clean state for next call
	UFUNCTION()
	void OnAbilityAnimationBlendOut(FGameplayTag EventTag, FGameplayEventData EventData);

	//! Animation was cancelled externally
	UFUNCTION()
	void OnAbilityAnimationAbort(FGameplayTag EventTag, FGameplayEventData EventData);

	//! Called when Animation reach the cast point through an AnimNotify
	UFUNCTION()
	void OnAbilityAnimationEvent(FGameplayTag EventTag, FGameplayEventData EventData);

public:
	// Targeting
	UPROPERTY()
	class UGKAbilityTask_WaitForTargetData* TargetTask;

	UFUNCTION()
	void OnAbilityTargetingCancelled(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void OnAbilityTargetAcquired(const FGameplayAbilityTargetDataHandle& Data);

	UPROPERTY(BlueprintAssignable)
	FGKTargetingResultDelegate TargetingResultDelegate;

	UPROPERTY(BlueprintAssignable)
	FGKTargetingStartDelegate TargetingStartDelegate;

	// -----------------------------------------------------------------

	// Projectile
	void SpawnProjectile(FGameplayTag EventTag, FGameplayEventData EventData);

public:
	// TODO: remove this
	/** Map of gameplay tags to gameplay effect containers */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GameplayEffects)
	TMap<FGameplayTag, FGKGameplayEffectContainer> EffectContainerMap;

	/** Make gameplay effect container spec to be applied later, using the passed in container */
	UFUNCTION(BlueprintCallable, Category = Ability, meta=(AutoCreateRefTerm = "EventData"))
	virtual FGKGameplayEffectContainerSpec MakeEffectContainerSpecFromContainer(const FGKGameplayEffectContainer& Container, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);

	/** Search for and make a gameplay effect container spec to be applied later, from the EffectContainerMap */
	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "EventData"))
	virtual FGKGameplayEffectContainerSpec MakeEffectContainerSpec(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);

	/** Applies a gameplay effect container spec that was previously created */
	UFUNCTION(BlueprintCallable, Category = Ability)
	virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainerSpec(const FGKGameplayEffectContainerSpec& ContainerSpec);

	/** Applies a gameplay effect container, by creating and then applying the spec */
	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "EventData"))
	virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainer(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);

	friend class AGKCharacterBase;

	// UGameplayAbility Overrides
	//---------------------------

	//! Returns the gameplay effect used to determine cooldown
	//! The standard implementation use the CDO but we want to define
	//! the cooldown procedurally
	virtual UGameplayEffect* GetCooldownGameplayEffect() const override;

	UPROPERTY()
	UGameplayEffect* CooldownEffectInstance;

	//! Returns the gameplay effect used to apply cost
	//! The standard implementation use the CDO but we want to define
	//! the cooldown procedurally
	virtual UGameplayEffect* GetCostGameplayEffect() const override;

	UPROPERTY()
	UGameplayEffect* CostEffectInstance;

	// Make sure Ability with a level <= 0 are disabled
	bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility);

	//! Apply Cooldown without using GameEffect CDO
	void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	//! Apply Cost without using GameEffect CDO
	void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	//! Apply GameEffect without using GameEffect CDO
	void ApplyGameplayEffectToOwnerDynamic(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const UGameplayEffect* GameplayEffect, float GameplayEffectLevel, int32 Stacks = 1) const;

	//! Our curve is inside our json, not in a separate datatable
	//! but ScalableFloat expects a dedicated table
	//! if ValuesAreFinal == 1 then the Curve is the result of Values[0] * Values[Level]
	//! if ValuesAreFinal == 0 then the first value is the base & the subsequent values are levels multiplier
	FScalableFloat GenerateCurveDataFromArray(FName prefix, TArray<float>& Values, bool ValuesAreFinal, bool Cost);

	// Dynamic Init
	// ------------
	UGameplayEffect* NewCooldownEffectFromConfig(TArray<float>& Durations);

	UGameplayEffect* NewCostEffectFromConfig(FGKAbilityCost& Conf);

	// Extension
	// ---------
	// We have:
	//	- CheckCooldown				->  GetAbilityCooldownTags()
	//	- CheckCost					->  GetAbilityCostAttribute()
	//  - CheckTagRequirements      ->  GetActivationBlockedTag() & GetActivationRequiredTag()
	//
	// if all 3 returns true then the ability can be cast

	//! returns a list of tags that would prevent this ability from being activable
	UFUNCTION(BlueprintCallable, Category = ActivationTags)
	FGameplayTagContainer GetActivationBlockedTag();

	//! returns a list of tags that are required for this ability to be activable
	UFUNCTION(BlueprintCallable, Category = ActivationTags)
	FGameplayTagContainer GetActivationRequiredTag();

	//! Returns the Cooldown tags to listen to
	UFUNCTION(BlueprintCallable, Category = Cooldown)
	const FGameplayTagContainer& GetAbilityCooldownTags() const;

	//! Returns the Cost attributes to listen to
	UFUNCTION(BlueprintCallable, Category = Cost)
	TArray<FGameplayAttribute> GetAbilityCostAttribute() const;

	//! Check that Ability is not currently blocked
	virtual bool CheckTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const {
		return DoesAbilitySatisfyTagRequirements(AbilitySystemComponent, nullptr, nullptr, OptionalRelevantTags);
	}

	//! Checks the ability's status, returns true if the ability can be cast. Has no side effects.
	//! if the ability is blocked returns a list of GameplayTags causing the block.
	UFUNCTION(BlueprintCallable, Category = ActivationTags, DisplayName = "CheckTagRequirements", meta = (ScriptName = "CheckTagRequirements"))
	virtual bool K2_CheckTagRequirements(FGameplayTagContainer& RelevantTags);

	//! Level up the ability (RPC to server)
	UFUNCTION(BlueprintCallable, Category = Ability)
	void LevelUpAbility();

	//! Server RPC to client
	//! Note that the Level returned by this event is the correct one
	//! The level stored in the ability spec can be lagging behind
	//! if replication has not happened yet
	UPROPERTY(BlueprintAssignable)
	FGKAbilityLevelUpDelegate OnAbilityLevelUp;
};

//! Return an effect that regenerate the given attribute overtime
//! Value is the amount regenerated per second
//! Period is the update tick in seconds
UGameplayEffect* NewPassiveRegenEffect(UObject* Parent, FGameplayAttribute Attribute, float Value, float Period = 1.f, FName name = NAME_None);
