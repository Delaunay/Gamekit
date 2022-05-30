// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Abilities/GKAbilityTypes.h"
#include "Gamekit/Abilities/Targeting/GKAbilityTarget_Actor.h"
#include "Gamekit/Gamekit.h"

// Unreal Engine
#include "AbilitySystemComponent.h"

// Generated
#include "GKAbilitySystemComponent.generated.h"

class UGKGameplayAbility;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGKAbilityFailedDelegate,
                                             const UGameplayAbility *,
                                             Ability,
                                             const FGameplayTagContainer &,
                                             FailureReason);

/* Queued Ability
 *
 * For an Ability to be queuable, all the data it requires
 * needs to be available when the Ability is first queued AND
 * at the time of execution
 *
 * So moving to a point on the map is fine.
 * Casting a spell on an ennemy will only be possible if
 * the enemy is in range & visible at the time.
 *
 * The queued ability saves all the data it requires
 * and will skip some activation steps (like requesting targets)
 *
 * Instead the data will be validated before execution
 */
USTRUCT()
struct GAMEKIT_API FGKQueuedAbility
{
    GENERATED_BODY();

    public:
    // Ability To be executed with all the appropriate information
    // FGameplayAbilitySpecHandle Handle;

    // FGameplayAbilityActorInfo ActorInfo;

    // FGameplayAbilityActivationInfo ActivationInfo;

    // Data available to the ability at Execution
    // TODO: Make/Use a TargetData Handle
    AActor *Target;

    FVector Location;
};

/**
 *
 * TODO: We need an ability Queue
 */
UCLASS(BlueprintType, Blueprintable)
class GAMEKIT_API UGKAbilitySystemComponent: public UAbilitySystemComponent
{
    GENERATED_BODY()

    public:
    // Constructors and overrides
    UGKAbilitySystemComponent();

    //! Version of function in AbilitySystemGlobals that returns correct type
    static UGKAbilitySystemComponent *GetAbilitySystemComponentFromActor(const AActor *Actor,
                                                                         bool          LookForComponent = false);

    //! Used to receive the Granted ability through network
    void OnGiveAbility(FGameplayAbilitySpec &AbilitySpec) override;

    void OnRemoveAbility(FGameplayAbilitySpec& Spec) override;

    UFUNCTION(BlueprintCallable)
    bool IsInitialized() const;

    // Queue Interface
    // ---------------
    void ExecuteQueuedAbility(FGKQueuedAbility const &) {}

    // Check if there are any Abilities in queue & execute them
    void ExecuteAbilityQueue(){};

    void ClearAbilityQueue() {}

    void AddToQueue() {}

    TArray<FGKQueuedAbility> AbilityQueue;

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
    void OnAbilityFailed_Native(const UGameplayAbility *Ability, const FGameplayTagContainer &FailureReason);

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

    //! Level up the given ability
    UFUNCTION(BlueprintCallable, Category = Abilities)
    void TryActivateAbility_Point(FGameplayAbilitySpecHandle Handle, FVector Point);

    UFUNCTION(Server, Reliable)
    void ServerTryActivateAbility_Point(FGameplayAbilitySpecHandle Handle, FVector Point);

    // protected:
    bool Initialized;

    friend class AGKGASCharacter;
    friend class UGKGameplayAbility;

    protected:
    UPROPERTY()
    TMap<TSubclassOf<AGKAbilityTarget_Actor>, AGKAbilityTarget_Actor *> AbilityTarget_ActorCache;

    static TSubclassOf<AGKAbilityTarget_Actor> GetAbilityTarget_ActorClass(EGK_TargetingMode Mode);

    public:
    AGKAbilityTarget_Actor *GetAbilityTarget_Actor(TSubclassOf<AGKAbilityTarget_Actor> AbilityTarget_ActorClass);

    AGKAbilityTarget_Actor *GetAbilityTarget_Actor(EGK_TargetingMode Mode);
};