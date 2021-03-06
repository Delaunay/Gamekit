// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Abilities/GKAbilityStatic.h"
#include "Gamekit/Abilities/GKAbilityTypes.h"
#include "Gamekit/Gamekit.h"

// Unreal Engine
#include "Abilities/GameplayAbility.h"

// Generated
#include "GKGameplayAbility.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGKCancelBackswingDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGKTargetingStartDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGKAbilityRemovalDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGKTargetingResultDelegate, bool, TargetCancelled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGKAbilityLevelUpDelegate, int, Level);

/* Dynamic Gameplay effect that is initialized from a DataTable
 */
UCLASS(Blueprintable)
class GAMEKIT_API UGKGameplayEffectDyn: public UGameplayEffect
{
    public:
    GENERATED_UCLASS_BODY()

    public:
    // Overide the Networking functions
    bool IsNameStableForNetworking() const override { return true; }

    bool IsSupportedForNetworking() const override { return true; }
};

/** Base Gameplay Ability to implement new skill/item/actions
 *
 * For ability with charges, just make the ability consume charges on activation
 * remove cooldown, give an effect that grant charges overtime
 * Add the Charge Gametag the the Ability Tag Requirement
 *
 * TODO: Measure GA Latency
 *	   : Measure RPC Latency (Ping-Pong) with BP
 *
 * TODO: Add an option to execute a queued ability
 * which would skip the Need to Request a target data from the user
 *
 */
UCLASS()
class GAMEKIT_API UGKGameplayAbility: public UGameplayAbility
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

    void ActivateAbility_Targeted();

    void ActivateAbility_Channel();

    void ActivateAbility_Charges();

    //! Ability that is Toggled on/off
    void ActivateAbility_Toggle();

    void K2_EndAbility() override;

    public:
    // Constructor and overrides
    UGKGameplayAbility();

    // DataTable
    // ---------

    //! DataTable used to fetch values for its data-driven effect
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Data")
    class UDataTable *AbilityDataTable;

    //! Row Name used to fetch the values for its data-driven effect
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Data")
    FName AbilityRowName;

    //! Retrieve the values from the DataTable for its data-driven effect
    //! copies the entire struct
    UFUNCTION(BlueprintCallable,
              Category    = "Ability|Data",
              DisplayName = "GetAbilityStatic",
              meta        = (ScriptName = "GetAbilityStatic"))
    void K2_GetAbilityStatic(FGKAbilityStatic &AbilityStatic, bool &Valid);

    //! Retrieve the values from the DataTable for its data-driven effect
    //! C++ version avoid to copy the entire struct
    FGKAbilityStatic *GetAbilityStatic() const;

    //! Called everytime the DataTable is modified, refresh the cached lookup
    UFUNCTION()
    void OnDataTableChanged_Native() const;

    //! Called everytime the Static data is loaded (init & reimport)
    virtual void LoadFromDataTable(FGKAbilityStatic &AbilityDef);

    //! Initialize data-driven effect after properties have been set
    //! i.e right after ``AbilityDataTable`` & ``AbilitRowName`` were set
    void PostInitProperties() override;

    //! Cached lookup, do not use!
    mutable FGKAbilityStatic *AbilityStatic;

    // Animations
    // ----------
    public:
    //! Lookup the animation montage to use before cast this ability
    UFUNCTION(BlueprintCallable)
    class UAnimMontage *GetAnimation();

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
    class UGKAbilityTask_PlayMontageAndWaitForEvent *AnimTask;

    protected:
    //! Spawn AbilityTarget Actor, the spawn is cached
    //! so abilities will reuse the targetting actor overtime
    class AGKAbilityTarget_Actor *SpawnAbilityTarget_Actor();

    //! New animation is taking over, make sure the ability is in a clean state for next call
    UFUNCTION()
    virtual void OnAbilityAnimationBlendOut(FGameplayTag EventTag, FGameplayEventData EventData);

    //! Animation was cancelled externally
    UFUNCTION()
    void OnAbilityAnimationAbort(FGameplayTag EventTag, FGameplayEventData EventData);

    //! Called when Animation reach the cast point through an AnimNotify
    UFUNCTION()
    void OnAbilityAnimationEvent(FGameplayTag EventTag, FGameplayEventData EventData);

    public:
    // Move to location during targeting
    UPROPERTY()
    class UGKAbilityTask_MoveToDestination *MoveToTargetTask;
    // class UAbilityTask_MoveToLocation* MoveToTargetTask;

    UFUNCTION()
    void OnAbilityMoveToTargetCompleted(const FGameplayAbilityTargetDataHandle &Data);

    UFUNCTION()
    void OnAbilityMoveToTargetCancelled(const FGameplayAbilityTargetDataHandle &Data);

    // Targeting
    UPROPERTY()
    class UGKAbilityTask_WaitForTargetData *TargetTask;

    UFUNCTION()
    void OnAbilityTargetingCancelled(const FGameplayAbilityTargetDataHandle &Data);

    UFUNCTION()
    void OnAbilityTargetAcquired(const FGameplayAbilityTargetDataHandle &Data);

    UPROPERTY(BlueprintAssignable)
    FGKTargetingResultDelegate TargetingResultDelegate;

    UPROPERTY(BlueprintAssignable)
    FGKTargetingStartDelegate TargetingStartDelegate;

    UPROPERTY(BlueprintAssignable)
    FGKAbilityRemovalDelegate OnAbilityRemoved;

    void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

    // -----------------------------------------------------------------
    //! Reads the TargetData and try to fetch a location and an actor
    UFUNCTION(BlueprintCallable, Category = Ability)
    bool GetTargetLocation(FGameplayAbilityTargetDataHandle TargetData,
                           FVector &                        Position,
                           AActor *&                        Target,
                           int32                            Index = 0);

    // Projectile
    UFUNCTION(BlueprintCallable, Category = Ability)
    void SpawnProjectile(FGameplayTag EventTag, FGameplayEventData EventData);

    UFUNCTION(BlueprintCallable, Category = Ability)
    void ApplyEffectsToTarget(FGameplayTag EventTag, FGameplayEventData EventData);

    public:

    /** Make gameplay effect container spec to be applied later, using the passed in container */
    UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "EventData"))
    virtual FGKGameplayEffectContainerSpec MakeEffectContainerSpecFromContainer(
            const FGKGameplayEffectContainer &Container,
            const FGameplayEventData &        EventData,
            int32                             OverrideGameplayLevel = -1);


    UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "EventData"))
    virtual FGKGameplayEffectContainerSpec MakeEffectContainerSpec(EGK_EffectSlot            EffectSlot,
                                                                   const FGameplayEventData& EventData,
                                                                   int32                     OverrideGameplayLevel = -1);

    FGKGameplayEffectContainerSpec MakeEffectContainerSpec(FGKAbilityEffects const& AbilitiyEffets,
                                                           const FGameplayEventData& EventData,
                                                           int32                     OverrideGameplayLevel = -1);

    /** Applies a gameplay effect container spec that was previously created */
    UFUNCTION(BlueprintCallable, Category = Ability)
    virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainerSpec(
            const FGKGameplayEffectContainerSpec &ContainerSpec);

    friend class AGKCharacterBase;

    // UGameplayAbility Overrides
    //---------------------------

    FGameplayTagContainer const* GetCooldownTags() const;

    FGameplayTagContainer const* GetCooldownTagsFromSpec(FGameplayAbilitySpec* Spec) const;

    UPROPERTY()
    UGameplayEffect *CooldownEffectInstance;

    //! Returns the gameplay effect used to apply cost
    //! The standard implementation use the CDO but we want to define
    //! the cooldown procedurally
    virtual UGameplayEffect *GetCostGameplayEffect() const override;

    UPROPERTY()
    UGameplayEffect *CostEffectInstance;

    // Make sure Ability with a level <= 0 are disabled
    bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                            const FGameplayAbilityActorInfo *ActorInfo,
                            const FGameplayTagContainer *    SourceTags     = nullptr,
                            const FGameplayTagContainer *    TargetTags     = nullptr,
                            OUT FGameplayTagContainer *OptionalRelevantTags = nullptr) const override;

    void CancelAbility(const FGameplayAbilitySpecHandle     Handle,
                       const FGameplayAbilityActorInfo *    ActorInfo,
                       const FGameplayAbilityActivationInfo ActivationInfo,
                       bool                                 bReplicateCancelAbility);

    //! Apply Cooldown without using GameEffect CDO
    void ApplyCooldown(const FGameplayAbilitySpecHandle     Handle,
                       const FGameplayAbilityActorInfo *    ActorInfo,
                       const FGameplayAbilityActivationInfo ActivationInfo) const override;


    //! Apply GameEffect without using GameEffect CDO
    void ApplyGameplayEffectToOwnerDynamic(const FGameplayAbilitySpecHandle     Handle,
                                           const FGameplayAbilityActorInfo *    ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           const UGameplayEffect *              GameplayEffect,
                                           float                                GameplayEffectLevel,
                                           int32                                Stacks = 1) const;

    //! Our curve is inside our json, not in a separate datatable
    //! but ScalableFloat expects a dedicated table
    //! if ValuesAreFinal == 1 then the Curve is the result of Values[0] * Values[Level]
    //! if ValuesAreFinal == 0 then the first value is the base & the subsequent values are levels multiplier
    FScalableFloat GenerateCurveDataFromArray(FName prefix, TArray<float> &Values, bool ValuesAreFinal, bool Cost);

    // Dynamic Init
    // ------------

    UGameplayEffect *NewCostEffectFromConfig(FGKAbilityCost &Conf);

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

    // ------------------------------------------------
    // Ability cancellation

    /** This ability gets canceled by those tags */
    UPROPERTY(EditDefaultsOnly, Category = Tags, meta = (Categories = "Tags"))
    FGameplayTagContainer CancelledByTags;

    FDelegateHandle CancelByTagsDelegateHandle;

    void SetupCancelByTags(const FGameplayAbilityActorInfo* ActorInfo);

    UFUNCTION()
    void CancelAbilityFromTag(const FGameplayTag Tag, int32 Count);

    void ClearCancelByTags(const FGameplayAbilityActorInfo* ActorInfo);
    // ------------------------------------------------

    //! Returns the Cooldown tags to listen to
    UFUNCTION(BlueprintCallable, Category = Cooldown)
    const FGameplayTagContainer &GetAbilityCooldownTags() const;

    //! Returns the Cost attributes to listen to
    UFUNCTION(BlueprintCallable, Category = Cost)
    TArray<FGameplayAttribute> GetAbilityCostAttribute() const;

    //! Level up the ability (RPC to server)
    UFUNCTION(BlueprintCallable, Category = Ability)
    void LevelUpAbility();

    //! Activate the hability and skip target acquisition
    //! It essentially skips a lot of replication logic
    //! TODO: Replicate to server
    UFUNCTION(BlueprintCallable, Category = Ability)
    void ActivateManual_PointTarget(FVector Point);

    //! Server RPC to client
    //! Note that the Level returned by this event is the correct one
    //! The level stored in the ability spec can be lagging behind
    //! if replication has not happened yet
    UPROPERTY(BlueprintAssignable)
    FGKAbilityLevelUpDelegate OnAbilityLevelUp;

    //! Called when the the Ability is active and it is pressed a second time
    virtual void InputPressed(const FGameplayAbilitySpecHandle     Handle,
                              const FGameplayAbilityActorInfo *    ActorInfo,
                              const FGameplayAbilityActivationInfo ActivationInfo) override;

    // Current Task taht is playing
    UAbilityTask *CurrentTask;

    UFUNCTION(BlueprintCallable, Category = Ability)
    bool IsPassive() const;

    // Used to grand passives
    void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

    //! Number of charges available to be cast
    UFUNCTION(BlueprintCallable, Category = Ability)
    int K2_NumCharges() const;

    int NumCharges(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const;

    void ApplyChargeCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const;

    void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

    // This is technically a Tag Requirement, but it is a bit different because the tag stack is consumed
    bool CheckChargeCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const;

    bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

    bool CheckEnergyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const;

    bool CheckTagRequirements(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const;

    //! Used to check if a target is valid
    //! TODO: actually use this during target picking
    bool CheckTargetTagRequirements(const FGameplayTagContainer* TargetTags, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const;

    //! Check if the actor has charges to cast the ability
    UFUNCTION(BlueprintCallable, Category = Ability, DisplayName = "CheckChargeCost")
    bool K2_CheckChargeCost() const;

    //! Check the actor has enough energy to cast the ability
    UFUNCTION(BlueprintCallable, Category = Ability, DisplayName = "CheckEnergyCost")
    bool K2_CheckEnergeyCost() const;

    //! Check both Actiation requirements/blocks, Source requirements/blocks and Target requirements / blocks
    UFUNCTION(BlueprintCallable, Category = Ability, DisplayName = "CheckTagRequirements")
    bool K2_CheckTagRequirements() const;

    
};

//! Return an effect that regenerate the given attribute overtime
//! Value is the amount regenerated per second
//! Period is the update tick in seconds
UGameplayEffect *NewPassiveRegenEffect(UObject *          Parent,
                                       FGameplayAttribute Attribute,
                                       float              Value,
                                       float              Period = 1.f,
                                       FName              name   = NAME_None);
