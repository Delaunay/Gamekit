// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Abilities/GKGameplayAbility.h"

// Gamekit
#include "Gamekit/Abilities/AbilityTasks/GKAbilityTask_MoveToDestination.h"
#include "Gamekit/Abilities/AbilityTasks/GKAbilityTask_PlayMontageAndWaitForEvent.h"
#include "Gamekit/Abilities/GKAbilitySystemComponent.h"
#include "Gamekit/Abilities/GKAbilitySystemGlobals.h"
#include "Gamekit/Abilities/GKCastPointAnimNotify.h"
#include "Gamekit/Abilities/GKTargetType.h"
#include "Gamekit/Abilities/Targeting/GKAbilityTarget_PlayerControllerTrace.h"
#include "Gamekit/Abilities/Targeting/GKAbilityTask_WaitTargetData.h"
#include "Gamekit/Characters/GKCharacter.h"
#include "Gamekit/Controllers/GKPlayerController.h"
#include "Gamekit/Gamekit.h"
#include "Gamekit/Projectiles/GKProjectile.h"

// Unreal Engine
#include "Abilities/Tasks/AbilityTask_MoveToLocation.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Engine/NetSerialization.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameplayCue_Types.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UGKGameplayEffectDyn::UGKGameplayEffectDyn(const FObjectInitializer &ObjectInitializer): Super(ObjectInitializer) {}

UGKGameplayAbility::UGKGameplayAbility()
{
    InstancingPolicy   = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

    ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
    ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Debuff.Stun")));

    static FGameplayTag Exclusive = FGameplayTag::RequestGameplayTag(FName("Ability.Exclusive"));
    BlockAbilitiesWithTag.AddTag(Exclusive);
    AbilityTags.AddTag(Exclusive);

    Immediate     = false;
    AbilityStatic = nullptr;

    CooldownEffectInstance = nullptr;
    CostEffectInstance     = nullptr;
}

void UGKGameplayAbility::PostInitProperties()
{
    Super::PostInitProperties();

    if (AbilityDataTable && AbilityRowName.IsValid())
    {
        UE_LOG(LogGamekit, Log, TEXT("Loading From DataTable"));
        OnDataTableChanged_Native();
    }
}

void UGKGameplayAbility::OnDataTableChanged_Native()
{
    // Reset Cache
    AbilityStatic = nullptr;
    bool Valid    = false;

    FGKAbilityStatic *AbilityStaticOut = GetAbilityStatic();

    if (!AbilityStaticOut)
    {
        AbilityStatic = nullptr;
    }
}

void UGKGameplayAbility::LoadFromDataTable(FGKAbilityStatic &AbilityDef)
{
    UE_LOG(LogGamekit, Log, TEXT("Init Ability from table %s"), *AbilityDef.Name.ToString());

    CooldownEffectInstance = NewCooldownEffectFromConfig(AbilityDef.Cooldown);
    CostEffectInstance     = NewCostEffectFromConfig(AbilityDef.Cost);
}

FGKAbilityStatic *UGKGameplayAbility::GetAbilityStatic()
{
    if (AbilityStatic)
    {
        return AbilityStatic;
    }

    if (!AbilityDataTable || !AbilityRowName.IsValid())
    {
        return nullptr;
    }

    // we do not have a cached value
    if (!AbilityStatic)
    {
        AbilityStatic = AbilityDataTable->FindRow<FGKAbilityStatic>(AbilityRowName, TEXT("Ability"), true);

        if (AbilityStatic != nullptr)
        {
            LoadFromDataTable(*AbilityStatic);
        }

        // Listen to data table change
        if (!AbilityDataTable->OnDataTableChanged().IsBoundToObject(this))
        {
            AbilityDataTable->OnDataTableChanged().AddUObject(this, &UGKGameplayAbility::OnDataTableChanged_Native);
        }
    }

    return AbilityStatic;
}

void UGKGameplayAbility::K2_GetAbilityStatic(FGKAbilityStatic &AbilityStaticOut, bool &Valid)
{
    Valid = false;

    auto Result = GetAbilityStatic();
    if (Result != nullptr)
    {
        AbilityStaticOut = *Result;
        Valid            = true;
    }
}



FGKGameplayEffectContainerSpec UGKGameplayAbility::MakeEffectContainerSpec(EGK_EffectSlot            EffectSlot,
                                                                           const FGameplayEventData& EventData,
                                                                           int32                     OverrideGameplayLevel)
{

    auto Data = GetAbilityStatic();

    FGKAbilityEffects const* Effects = Data->AbilityEffects.Find(EffectSlot);

    if (Effects) {
        return MakeEffectContainerSpec(*Effects, EventData, OverrideGameplayLevel);
    }

    return FGKGameplayEffectContainerSpec();
}

FGKGameplayEffectContainerSpec UGKGameplayAbility::MakeEffectContainerSpec(FGKAbilityEffects const&  AbilitiyEffets,
                                                                           const FGameplayEventData& EventData,
                                                                           int32                     OverrideGameplayLevel)
{
    // First figure out our actor info
    FGKGameplayEffectContainerSpec ReturnSpec;
    AActor* OwningActor = GetOwningActorFromActorInfo();
    AGKCharacterBase* OwningCharacter = Cast<AGKCharacterBase>(OwningActor);
    UGKAbilitySystemComponent* OwningASC = UGKAbilitySystemComponent::GetAbilitySystemComponentFromActor(OwningActor);

    if (OwningASC)
    {
        // If we don't have an override level, use the default on the ability itself
        if (OverrideGameplayLevel == INDEX_NONE)
        {
            OverrideGameplayLevel = OverrideGameplayLevel = this->GetAbilityLevel();
        }

        // Build GameplayEffectSpecs for each applied effect
        for (const FGKAbilityEffect& Effect : AbilitiyEffets.Effects)
        {
            ReturnSpec.TargetGameplayEffectSpecs.Add(
                MakeOutgoingGameplayEffectSpec(Effect.GameplayEffectClass, OverrideGameplayLevel)
            );
        }
    }
    return ReturnSpec;
}

FGKGameplayEffectContainerSpec UGKGameplayAbility::MakeEffectContainerSpecFromContainer(
        const FGKGameplayEffectContainer &Container,
        const FGameplayEventData         &EventData,
        int32                             OverrideGameplayLevel)
{
    // First figure out our actor info
    FGKGameplayEffectContainerSpec ReturnSpec;
    AActor                        *OwningActor     = GetOwningActorFromActorInfo();
    AGKCharacterBase              *OwningCharacter = Cast<AGKCharacterBase>(OwningActor);
    UGKAbilitySystemComponent *OwningASC = UGKAbilitySystemComponent::GetAbilitySystemComponentFromActor(OwningActor);

    if (OwningASC)
    {
        // If we have a target type, run the targeting logic. This is optional, targets can be added later
        if (Container.TargetType.Get())
        {
            TArray<FHitResult>   HitResults;
            TArray<AActor *>     TargetActors;
            const UGKTargetType *TargetTypeCDO = Container.TargetType.GetDefaultObject();
            AActor              *AvatarActor   = GetAvatarActorFromActorInfo();
            TargetTypeCDO->GetTargets(OwningCharacter, AvatarActor, EventData, HitResults, TargetActors);
            ReturnSpec.AddTargets(HitResults, TargetActors);
        }

        // If we don't have an override level, use the default on the ability itself
        if (OverrideGameplayLevel == INDEX_NONE)
        {
            // OwningASC->GetDefaultAbilityLevel();
            OverrideGameplayLevel = OverrideGameplayLevel = this->GetAbilityLevel();
        }

        // Build GameplayEffectSpecs for each applied effect
        for (const TSubclassOf<UGameplayEffect> &EffectClass: Container.TargetGameplayEffectClasses)
        {
            ReturnSpec.TargetGameplayEffectSpecs.Add(
                    MakeOutgoingGameplayEffectSpec(EffectClass, OverrideGameplayLevel));
        }
    }
    return ReturnSpec;
}

TArray<FActiveGameplayEffectHandle> UGKGameplayAbility::ApplyEffectContainerSpec(
        const FGKGameplayEffectContainerSpec &ContainerSpec)
{
    TArray<FActiveGameplayEffectHandle> AllEffects;

    // Iterate list of effect specs and apply them to their target data
    for (const FGameplayEffectSpecHandle &SpecHandle: ContainerSpec.TargetGameplayEffectSpecs)
    {
        AllEffects.Append(K2_ApplyGameplayEffectSpecToTarget(SpecHandle, ContainerSpec.TargetData));
    }
    return AllEffects;
}


// This allow animation to be standard, and Gameplay designed can tweak the CastPoint independently
// from the actual animation
float ResolveAnimationPlayRate(FGKAbilityStatic const &Data, class UAnimMontage *Montage)
{
    if (Montage == nullptr)
    {
        return 1.f;
    }
    float Rate = 1.f;

    // 4.27
    // TArray<FAnimNotifyEventReference> OutActiveNotifies;
    // Montage->GetAnimNotifies(0, Montage->GetPlayLength(), false, OutActiveNotifies);

    FAnimNotifyContext NotifyContext;
    Montage->GetAnimNotifies(0.f, Montage->GetPlayLength(), NotifyContext);

    for (auto &NotifyRef: NotifyContext.ActiveNotifies)
    {
        FAnimNotifyEvent const *NotifyEvent = NotifyRef.GetNotify();
        auto                    Notify      = Cast<UGKCastPointAnimNotify>(NotifyEvent->Notify);

        // Find the ability Cast point on this animation
        // Create a CastPointAnimNotify class and check here
        if (Notify)
        {
            // if the Notify event is at 2 sec but the cast point is 1 sec
            // play the animation 2x so the Notify is indeed sent after 1 sec
            Rate = NotifyEvent->GetTriggerTime() / Data.CastTime;

            // Realisticly though we can only cast one ability with animation
            // So maybe we can reuse the same tag for everything
            // TODO: Here we could set the Event Tag to match the ability
            // Notify->CastPointEventTag = ;

            // We should probably only have one cast point
            break;
        }
    }

    return Rate;
}

void UGKGameplayAbility::OnAbilityTargetingCancelled(const FGameplayAbilityTargetDataHandle &Data)
{
    TargetingResultDelegate.Broadcast(true);
    TargetTask->EndTask();
    TargetTask = nullptr;
    // ------------------

    K2_CancelAbility();
}

void UGKGameplayAbility::ActivateManual_PointTarget(FVector Point)
{

    FHitResult Result;
    Result.ImpactPoint = Point;

    auto StartLocation    = FGameplayAbilityTargetingLocationInfo();
    auto TargetDataHandle = StartLocation.MakeTargetDataHandleFromHitResult(this, Result);

    OnAbilityTargetAcquired(TargetDataHandle);
}

FVector GetImpactPoint(const FGameplayAbilityTargetDataHandle &Data, int Index, bool &Valid)
{
    Valid = false;

    if (Data.Num() <= 0 || Data.Num() >= Index)
    {
        return FVector::ZeroVector;
    }

    const FGameplayAbilityTargetData *FirstTarget = Data.Get(Index);

    if (FirstTarget->HasHitResult())
    {
        Valid = true;
        return FirstTarget->GetHitResult()->ImpactPoint;
    }

    auto Actors = FirstTarget->GetActors();

    if (Actors.Num() <= 0)
    {
        return FVector::ZeroVector;
    }

    auto ActorRef = Actors[0];
    if (!ActorRef.IsValid())
    {
        return FVector::ZeroVector;
    }

    Valid = true;
    return ActorRef->GetActorLocation();
}

void UGKGameplayAbility::OnAbilityTargetAcquired(const FGameplayAbilityTargetDataHandle &Data)
{
    TargetingResultDelegate.Broadcast(false);

    if (TargetTask && IsValid(TargetTask))
    {
        TargetTask->EndTask();
        TargetTask = nullptr;
    }

    // cancel all the another abilities 
    auto ASC = GetAbilitySystemComponentFromActorInfo();
    ASC->CancelAbilities(nullptr, nullptr, this);

    UGKAttributeSet const* Attributes = Cast<UGKAttributeSet>(ASC->GetAttributeSet(UGKAttributeSet::StaticClass()));

    // Both rotates to face target and move towards it
    MoveToTargetTask = UGKAbilityTask_MoveToDestination::MoveToTarget(
        this,
        NAME_None,
        Data,
        GetAbilityStatic()->CastMaxRange, // Dist Tol
        15.f,                             // Angle Tol
        150.f,                            // Turn Rate
        Attributes->GetMoveSpeed(),       // Speed
        true, // Move to target
        true, // Use Movement Component
        GetAbilityStatic()->AbilityBehavior,
        FGameplayTagContainer(),
        true);

    // MoveToTargetStartDelegate.Broadcast();
    MoveToTargetTask->OnCancelled.AddDynamic(this, &UGKGameplayAbility::OnAbilityMoveToTargetCancelled);
    MoveToTargetTask->OnCompleted.AddDynamic(this, &UGKGameplayAbility::OnAbilityMoveToTargetCompleted);
    MoveToTargetTask->ReadyForActivation();
}

void UGKGameplayAbility::OnAbilityMoveToTargetCancelled(const FGameplayAbilityTargetDataHandle &Data)
{
    TargetingResultDelegate.Broadcast(true);

    MoveToTargetTask->EndTask();
    MoveToTargetTask = nullptr;
    // ------------------

    K2_CancelAbility();
}

void UGKGameplayAbility::OnAbilityMoveToTargetCompleted(const FGameplayAbilityTargetDataHandle &Data)
{
    if (MoveToTargetTask && IsValid(MoveToTargetTask))
    {
        MoveToTargetTask->EndTask();
        MoveToTargetTask = nullptr;
    }

    if (Immediate)
    {
        UGKGameplayAbility::OnAbilityAnimationEvent(FGameplayTag(), FGameplayEventData());
        return;
    }

    // Warning if an Animation is already playing
    // user need to cancel backswing to be able to cast before
    // animation ends
    // TODO: when implementing Action Queue handle that
    if (IsValid(AnimTask))
    {
        AnimTask->EndTask();
        // return;
    }

    auto Montage = GetAnimation();

    // Compute the Animation rate
    // so the CastPoint is triggered at the right time
    if (DyanmicCastPoint)
    {
        FGKAbilityStatic *AbilityData = GetAbilityStatic();

        if (AbilityData)
        {
            Rate = ResolveAnimationPlayRate(*AbilityData, Montage);
        }
    }

    // ## We are ready to play the animation
    AnimTask = UGKAbilityTask_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(
            this,
            NAME_None,
            Montage,
            // Listen to our ability event, empty matches everything
            FGameplayTagContainer(),
            // Attach target data to the Animation task so it can be forwarded
            // to the activation
            Data,
            Rate,
            StartSection,
            false,
            1.f);

    // Channel: Spell is cast after the animation ends BUT
    // resources are committed when the animation starts
    // During the channel the player cannot move.
    //
    // Normal: Spell is cast after animation reach the cast point
    // and the resource are committed then

    AnimTask->OnBlendOut.AddDynamic(this, &UGKGameplayAbility::OnAbilityAnimationBlendOut);
    AnimTask->OnInterrupted.AddDynamic(this, &UGKGameplayAbility::OnAbilityAnimationAbort);
    AnimTask->OnCancelled.AddDynamic(this, &UGKGameplayAbility::OnAbilityAnimationAbort);
    AnimTask->EventReceived.AddDynamic(this, &UGKGameplayAbility::OnAbilityAnimationEvent);

    // Run
    AnimTask->ReadyForActivation();
}

void UGKGameplayAbility::OnAbilityAnimationBlendOut(FGameplayTag EventTag, FGameplayEventData EventData)
{
    K2_EndAbility();
    AnimTask = nullptr;
}

void UGKGameplayAbility::OnAbilityAnimationAbort(FGameplayTag EventTag, FGameplayEventData EventData)
{
    K2_CancelAbility();
    AnimTask = nullptr;
}

bool UGKGameplayAbility::K2_CheckTagRequirements(FGameplayTagContainer &RelevantTags)
{
    UAbilitySystemComponent *const AbilitySystemComponent = CurrentActorInfo->AbilitySystemComponent.Get();
    return CheckTagRequirements(*AbilitySystemComponent, &RelevantTags);
}

bool UGKGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo *ActorInfo,
                                            const FGameplayTagContainer     *SourceTags,
                                            const FGameplayTagContainer     *TargetTags,
                                            FGameplayTagContainer           *OptionalRelevantTags) const
{
    auto ASC = GetAbilitySystemComponentFromActorInfo();
    if (!ASC)
    {
        return false;
    }

    FGameplayAbilitySpec *Spec = ASC->FindAbilitySpecFromHandle(Handle);

    if (Spec == nullptr || Spec->Level <= 0)
    {
        const FGameplayTag &NotYetLearnedTag =
                ((UGKAbilitySystemGlobals &)UGKAbilitySystemGlobals::Get()).ActivateFailNotYetLearnedTag;
        if (OptionalRelevantTags && NotYetLearnedTag.IsValid())
        {
            OptionalRelevantTags->AddTag(NotYetLearnedTag);
        }

        return false;
    }

    return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGKGameplayAbility::CancelAbility(const FGameplayAbilitySpecHandle     Handle,
                                       const FGameplayAbilityActorInfo     *ActorInfo,
                                       const FGameplayAbilityActivationInfo ActivationInfo,
                                       bool                                 bReplicateCancelAbility)
{
    Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

    if (AnimTask && IsValid(AnimTask))
    {
        AnimTask->ExternalCancel();
    }

    AnimTask = nullptr;
}

void UGKGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle     Handle,
                                       const FGameplayAbilityActorInfo     *ActorInfo,
                                       const FGameplayAbilityActivationInfo ActivationInfo) const
{
    // No generated Effect
    if (!CooldownEffectInstance)
    {
        // This uses the GameEffect CDO, when the Ability is not instantiated
        return Super::ApplyCooldown(Handle, ActorInfo, ActivationInfo);
    }

    return ApplyGameplayEffectToOwnerDynamic(
            Handle, ActorInfo, ActivationInfo, CooldownEffectInstance, GetAbilityLevel(Handle, ActorInfo), 1);
}

void UGKGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle     Handle,
                                   const FGameplayAbilityActorInfo     *ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo) const
{
    // No generated Effect
    if (!CostEffectInstance)
    {
        // This uses the GameEffect CDO, when the Ability is not instantiated
        return Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
    }

    return ApplyGameplayEffectToOwnerDynamic(
            Handle, ActorInfo, ActivationInfo, CostEffectInstance, GetAbilityLevel(Handle, ActorInfo), 1);
}

void UGKGameplayAbility::ApplyGameplayEffectToOwnerDynamic(const FGameplayAbilitySpecHandle     Handle,
                                                           const FGameplayAbilityActorInfo     *ActorInfo,
                                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                                           const UGameplayEffect               *GameplayEffect,
                                                           float                                GameplayEffectLevel,
                                                           int32                                Stacks) const
{
    if (!GameplayEffect || !(HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo)))
    {
        return;
    }

    FScopedGameplayCueSendContext GameplayCueSendContext;

    UAbilitySystemComponent *AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();

    // NOTE: UE is doing this on their side, FGameplayEffectSpecHandle is a shared pointer
    // Let just use the struct as is and see
    //
    //		FGameplayEffectSpecHandle SpecHandle = new FGameplayEffectSpec
    //
    auto Spec = FGameplayEffectSpec(GameplayEffect, MakeEffectContext(Handle, ActorInfo), GameplayEffectLevel);

    FGameplayAbilitySpec *AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
    if (AbilitySpec)
    {
        Spec.SetByCallerTagMagnitudes = AbilitySpec->SetByCallerTagMagnitudes;
    }

    Spec.StackCount = Stacks;

    ApplyAbilityTagsToGameplayEffectSpec(Spec, AbilitySpec);

    // ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
    AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(Spec, AbilitySystemComponent->GetPredictionKeyForNewAction());
}

void UGKGameplayAbility::OnAbilityAnimationEvent(FGameplayTag EventTag, FGameplayEventData EventData)
{
    // Only the authority commits the ability
    // in the BP side I had a double commit issue
    // but it did not happen in C++ :thinking:
    // might be that 2nd replicated CommitAbility was received by the server after the 1st EndAbility was called on the
    // server making the 2nd Commit valid while in C++ the time window for this to happen is much shorter making the bug
    // less likely to happen
    //
    // Note: cooldowns will prevent the bug from happening
    if (!K2_HasAuthority())
    {
        // needs to wait for CommitAbility being replicated on client and then spawn the projectile everywhere
        return;
    }

    if (K2_CommitAbility())
    {
        SpawnProjectile(EventTag, EventData);
        return;
    }

    K2_CancelAbility();
}

void UGKGameplayAbility::ActivateAbility_Hidden() {}

void UGKGameplayAbility::ActivateAbility_Passive() {}

void UGKGameplayAbility::ActivateAbility_NoTarget()
{
    // Cancel previous actions
    UGKGameplayAbility::OnAbilityTargetAcquired(FGameplayAbilityTargetDataHandle());
}

void UGKGameplayAbility::ActivateAbility_ActorTarget()
{
    // The TargetActor has the logic to handle ActorTarget & PointTarget
    ActivateAbility_PointTarget();
}

AGKAbilityTarget_Actor *UGKGameplayAbility::SpawnAbilityTarget_Actor()
{
    auto ASC = Cast<UGKAbilitySystemComponent>(GetCurrentActorInfo()->AbilitySystemComponent.Get());
    if (!ASC)
    {
        return nullptr;
    }

    FGKAbilityStatic *Data = GetAbilityStatic();

    if (!Data || !Data->AbilityTargetActorClass)
    {
        return nullptr;
    }

    auto TargetActor = ASC->GetAbilityTarget_Actor(TSubclassOf<AGKAbilityTarget_Actor>(Data->AbilityTargetActorClass));

    if (TargetActor)
    {
        TargetActor->InitializeFromAbilityData(*GetAbilityStatic());
    }

    return TargetActor;
}

void UGKGameplayAbility::ActivateAbility_PointTarget()
{
    auto TargetActor = SpawnAbilityTarget_Actor();

    if (!IsValid(TargetActor))
    {
        return;
    }

    TargetTask = UGKAbilityTask_WaitForTargetData::WaitForTargetDataUsingActor(
            this,                                          // UGameplayAbility * OwningAbility,
            NAME_None,                                     // FName TaskInstanceName
            EGameplayTargetingConfirmation::UserConfirmed, // ConfirmationType
            TargetActor                                    // TargetActor
    );

    // FIXME: Those are not set on the initial spawn 
    // but they should be set when StartTargeting is called
    // Inside the `WaitForTargetDataUsingActor` task
    TargetActor->OwningAbility = this;
    TargetActor->SourceActor = GetActorInfo().AvatarActor.Get();

    TargetingStartDelegate.Broadcast();
    TargetTask->Cancelled.AddDynamic(this, &UGKGameplayAbility::OnAbilityTargetingCancelled);
    TargetTask->ValidData.AddDynamic(this, &UGKGameplayAbility::OnAbilityTargetAcquired);
    TargetTask->ReadyForActivation();
}

void UGKGameplayAbility::ActivateAbility_Toggle()
{
    if (IsActive())
    {
        // Does not play animation on toggle off
        K2_EndAbility();
    }
    else
    {
        // Plays animation on toggle on
        UGKGameplayAbility::OnAbilityTargetAcquired(FGameplayAbilityTargetDataHandle());
    }
};


void UGKGameplayAbility::K2_EndAbility() {
    ClearCancelByTags(CurrentActorInfo);

    Super::K2_EndAbility();
}

void UGKGameplayAbility::ActivateAbility_Native()
{
    if (GetAbilityLevel() <= 0){
        return;
    }

    SetupCancelByTags(CurrentActorInfo);

    // Cancel current targeting task if any
    // this is to free up the delegates
    if (TargetTask && IsValid(TargetTask)) {
        TargetTask->ExternalCancel();
        TargetTask->EndTask();
        TargetTask = nullptr;
    }

    // This is the real cancel targeting
    GetAbilitySystemComponentFromActorInfo()->TargetCancel();

    auto AbilityData = GetAbilityStatic();

    if (!AbilityData)
    {
        return;
    }

    switch (AbilityData->AbilityBehavior)
    {
    case EGK_AbilityBehavior::ActorTarget:
        return ActivateAbility_ActorTarget();
    case EGK_AbilityBehavior::Hidden:
        return ActivateAbility_Hidden();
    case EGK_AbilityBehavior::NoTarget:
        return ActivateAbility_NoTarget();
    case EGK_AbilityBehavior::Passive:
        return ActivateAbility_Passive();
    case EGK_AbilityBehavior::PointTarget:
        return ActivateAbility_PointTarget();
    case EGK_AbilityBehavior::Toggle:
        return ActivateAbility_Toggle();
    }
}

const FGameplayTagContainer &UGKGameplayAbility::GetAbilityCooldownTags() const { return *Super::GetCooldownTags(); }

TArray<FGameplayAttribute> UGKGameplayAbility::GetAbilityCostAttribute() const
{
    TArray<FGameplayAttribute> Out;
    auto                       CostEffect = GetCostGameplayEffect();

    if (!CostEffect)
    {
        return Out;
    }

    for (auto &Modifiers: CostEffect->Modifiers)
    {
        Out.Add(Modifiers.Attribute);
    }
    return Out;
}

void UGKGameplayAbility::LevelUpAbility()
{
    if (IsInstantiated() == false || CurrentActorInfo == nullptr)
    {
        return;
    }

    auto ASC = Cast<UGKAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);

    if (ASC)
    {
        ASC->LevelUpAbility(CurrentSpecHandle);
    }
}

UGameplayEffect *UGKGameplayAbility::GetCooldownGameplayEffect() const
{
    if (CooldownEffectInstance)
    {
        UE_LOG(LogGamekit, Verbose, TEXT("Returns generated cooldown effect"));
        return CooldownEffectInstance;
    }

    return Super::GetCooldownGameplayEffect();
}

UGameplayEffect *UGKGameplayAbility::GetCostGameplayEffect() const
{
    if (CostEffectInstance)
    {
        UE_LOG(LogGamekit, Verbose, TEXT("Returns generated cost effect"));
        return CostEffectInstance;
    }

    return Super::GetCostGameplayEffect();
}

UGameplayEffect *UGKGameplayAbility::NewCooldownEffectFromConfig(TArray<float> &Durations)
{
    if (Durations.Num() <= 0)
    {
        UE_LOG(LogGamekit, Warning, TEXT("Cooldown value is not set!"));
        return nullptr;
    }

    auto RowName = FName(FString("Cooldown.") + AbilityStatic->Name.ToString());

    UGameplayEffect *CooldownEffect = NewObject<UGKGameplayEffectDyn>(this /*, TEXT("GameplayEffect.Cooldown") */);
    CooldownEffect->DurationPolicy  = EGameplayEffectDurationType::HasDuration;

    auto ScalableDuration             = GenerateCurveDataFromArray(RowName, Durations, true, false);
    CooldownEffect->DurationMagnitude = ScalableDuration;

    CooldownEffect->InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(RowName));

    return CooldownEffect;
};

void UGKGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) {
    UGameplayAbility::OnRemoveAbility(ActorInfo, Spec);
    OnAbilityRemoved.Broadcast();
}

UGameplayEffect *UGKGameplayAbility::NewCostEffectFromConfig(FGKAbilityCost &Conf)
{
    if (Conf.Value.Num() <= 0)
    {
        UE_LOG(LogGamekit, Warning, TEXT("Cost value is not set!"));
        return nullptr;
    }

    auto RowName = FName(FString("Cost.") + AbilityStatic->Name.ToString());

    UGameplayEffect *CostEffect = NewObject<UGKGameplayEffectDyn>(this /*, TEXT("GameplayEffect.Cost")*/);
    CostEffect->DurationPolicy  = EGameplayEffectDurationType::Instant;
    CostEffect->Modifiers.SetNum(1);

    FGameplayModifierInfo &AttributeCost = CostEffect->Modifiers[0];

    auto ScalableCost               = GenerateCurveDataFromArray(RowName, Conf.Value, true, true);
    AttributeCost.ModifierMagnitude = ScalableCost;
    AttributeCost.ModifierOp        = EGameplayModOp::Additive;
    AttributeCost.Attribute         = Conf.Attribute;

    return CostEffect;
};

UGameplayEffect *NewPassiveRegenEffect(UObject           *Parent,
                                       FGameplayAttribute Attribute,
                                       float              Value,
                                       float              Period,
                                       FName              name)
{
    if (Value <= 0)
    {
        return nullptr;
    }

    // Note that Giving it a name HERE makes the ability not work
    // Health Regen woudl work but mana would not
    UGameplayEffect *Regen = NewObject<UGKGameplayEffectDyn>(Parent, name /*, TEXT("GameplayEffect.Regen")*/);
    Regen->DurationPolicy  = EGameplayEffectDurationType::Infinite;
    Regen->Period          = Period;
    Regen->Modifiers.SetNum(1);

    FGameplayModifierInfo &RegenModifier = Regen->Modifiers[0];

    RegenModifier.ModifierMagnitude = FScalableFloat(Value * Period);
    RegenModifier.ModifierOp        = EGameplayModOp::Additive;
    RegenModifier.Attribute         = Attribute;

    auto TagName = FName(FString("Regen.") + Attribute.GetName());
    Regen->InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TagName));

    return Regen;
};

FScalableFloat UGKGameplayAbility::GenerateCurveDataFromArray(FName          RowName,
                                                              TArray<float> &Values,
                                                              bool           ValuesAreFinal,
                                                              bool           Cost)
{
    UCurveTable *Table = UAbilitySystemGlobals::Get().GetGlobalCurveTable();
    if (Table == nullptr)
    {
        UE_LOG(LogGamekit,
               Warning,
               TEXT("UAbilitySystemGlobals::GlobalCurveTableName is not set cannot generate curve data %s"),
               *RowName.ToString());
        return FScalableFloat();
    }

    if (Values.Num() <= 0)
    {
        UE_LOG(LogGamekit,
               Warning,
               TEXT("GenerateCurveDataFromDataTable cannot generate a curve with no values %s"),
               *RowName.ToString());
        return FScalableFloat();
    }

    if (Values.Num() == 1)
    {
        float BaseValue = Values[0];
        if (Cost)
        {
            BaseValue = -abs(BaseValue);
        }
        return FScalableFloat(BaseValue);
    }

    FSimpleCurve &Curve = Table->AddSimpleCurve(RowName);

    for (int i = 1 - int(ValuesAreFinal); i < Values.Num(); i++)
    {
        float v;

        if (ValuesAreFinal)
        {
            v = Values[i] / Values[0];
        }
        else
        {
            v = Values[i];
        }

        Curve.AddKey(i + 1, v);
    }

    float BaseValue = Values[0];
    if (Cost)
    {
        BaseValue = -abs(BaseValue);
    }

    FScalableFloat ScalableFloat(BaseValue);
    ScalableFloat.Curve.RowName    = RowName;
    ScalableFloat.Curve.CurveTable = Table;
    return ScalableFloat;
}

FGameplayTagContainer UGKGameplayAbility::GetActivationBlockedTag() { return ActivationBlockedTags; }

FGameplayTagContainer UGKGameplayAbility::GetActivationRequiredTag() { return ActivationRequiredTags; }

bool UGKGameplayAbility::GetTargetLocation(FGameplayAbilityTargetDataHandle TargetData,
                                           FVector                         &Position,
                                           AActor                         *&Target,
                                           int32                            Index)
{
    if (TargetData.Num() == 0)
    {
        UE_LOG(LogGamekit, Warning, TEXT("UGKGameplayAbility::SpawnProjectile: No TargetData"));
        return false;
    }

    if (Index == 0 && TargetData.Num() > 1)
    {
        UE_LOG(LogGamekit, Warning, TEXT("UGKGameplayAbility::SpawnProjectile: Has %d TargetData"), TargetData.Num());
    }

    if (TargetData.Get(Index)->HasHitResult())
    {
        auto HitActor = TargetData.Get(Index)->GetHitResult()->HitObjectHandle.FetchActor<AActor>();
        Target        = HitActor;
        Position      = TargetData.Get(Index)->GetHitResult()->ImpactPoint;
        return true;
    }
    else
    {
        auto Actors = TargetData.Get(Index)->GetActors();

        if (Index == 0 && Actors.Num() == 0)
        {
            UE_LOG(LogGamekit, Warning, TEXT("UGKGameplayAbility::SpawnProjectile: No Actor Data"));
            return false;
        }

        Target   = Actors[0].Get();
        Position = Target->GetActorLocation();
        return true;
    }

    return false;
}

void UGKGameplayAbility::SpawnProjectile(FGameplayTag EventTag, FGameplayEventData EventData)
{
    FGKAbilityStatic *Data = GetAbilityStatic();

    if (!Data || !Data->ProjectileActorClass)
    {
        return;
    }

    auto &TargetData = EventData.TargetData;

    auto ActorInfo = GetCurrentActorInfo();
    auto Actor     = ActorInfo->AvatarActor;
    auto Pawn      = Cast<APawn>(Actor.Get());

    AActor *Target = nullptr;
    FVector Direction;

    if (!GetTargetLocation(EventData.TargetData, Direction, Target, 0))
    {
        return;
    }

    FVector    Loc = Actor->GetActorLocation();
    FTransform Transform;
    Transform.SetLocation(Actor->GetActorLocation() + Actor->GetActorForwardVector() * 64.0f);

    // ---

    // From: GameplayAbilities\Public\Abilities\Tasks\AbilityTask_SpawnActor.h
    // > Long term we can also use this task as a sync point.
    // > If the executing client could wait execution until the server createsand replicate sthe
    // > actor down to him.We could potentially also use this to do predictive actor spawning / reconciliation.
    //
    // Pending UE does it we might have to do it here
    FActorSpawnParameters SpawnInfo;
    SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    SpawnInfo.Owner                          = Pawn;
    SpawnInfo.Instigator                     = Pawn;
    SpawnInfo.bDeferConstruction             = true;

    auto ProjectileInstance =
            Actor->GetWorld()->SpawnActor<AGKProjectile>(Data->ProjectileActorClass, Transform, SpawnInfo);

    Direction.Z                         = Loc.Z;
    ProjectileInstance->ProjectileData  = Data->ProjectileData;
    ProjectileInstance->Target          = Target;
    ProjectileInstance->Direction       = (Direction - Loc);

    ProjectileInstance->GameplayEffects = MakeEffectContainerSpec(EGK_EffectSlot::TargetEffect, EventData);

    ensure(ProjectileInstance->GameplayEffects.TargetGameplayEffectSpecs.Num() > 0);

    // The attachment is probably Character/class/Skeleton defined
    // ...

    UGameplayStatics::FinishSpawningActor(ProjectileInstance, Transform);

    // we need a multi-cast here to tell people we are ready to make the Projectile move
    // this would assume that the network works with a queue so given we modified the properties
    // earlier they will be updated first
    //
    // ProjectileInstance->Ready()
}

UAnimMontage *UGKGameplayAbility::GetAnimation()
{
    if (AnimMontages.Animations.Num() > 0)
    {
        return AnimMontages.Sample();
    }

    auto ActorInfo = GetCurrentActorInfo();
    auto Actor     = ActorInfo->AvatarActor;
    auto Pawn      = Cast<AGKCharacterBase>(Actor.Get());

    if (!Pawn)
    {
        UE_LOG(LogGamekit,
               Warning,
               TEXT("UGKGameplayAbility::GetAnimation could not fetch pawn animation for cast ability"));
        return nullptr;
    }

    auto UnitConf    = Pawn->GetUnitStatic();
    auto AbilityConf = GetAbilityStatic();

    if (UnitConf && AbilityConf)
    {
        AnimMontages = UnitConf->AnimationSet.GetAnimations(AbilityConf->AbilityAnimation);
    }

    return AnimMontages.Sample();
}

void UGKGameplayAbility::InputPressed(const FGameplayAbilitySpecHandle     Handle,
                                      const FGameplayAbilityActorInfo     *ActorInfo,
                                      const FGameplayAbilityActivationInfo ActivationInfo)
{

    // TODO: Move this to a movement Ability
    //
    // We could also use it to add multiple points
    // but not as flexible as an Ability queue
    auto MovementTask = Cast<UGKAbilityTask_MoveToDestination>(CurrentTask);
    if (MovementTask && IsValid(MovementTask))
    {
        auto       Controller = Cast<APlayerController>(ActorInfo->PlayerController.Get());
        FHitResult Result;
        // TODO:: Fix this trace
        ETraceTypeQuery Channel = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_WorldStatic);
        Controller->GetHitResultUnderCursorByChannel(Channel, false, Result);
        MovementTask->Destination = Result.ImpactPoint;
    }

    // This is a bit laggy
    //
    // Have to check the replicate on this one
    // For movement we only need the movement between the server & client to match
    // Maybe we can reduce the amount of data shared
    // Maybe we can update underlying task with its new point
    // Would prevent us to see the stop
    //
    // Might be better to move the movement out of GA
    // and use regular Task instead
    // But we do not have access to the current task running ?
    //
    // Dont cancel abilities that cannot be recast after cancel
    // This means you can cancel abilities by recasting it
    // before the cost & Cooldown gets expanded
    else if (CanActivateAbility(Handle, ActorInfo))
    {
        // Cancel the current ability
        CancelAbility(Handle, ActorInfo, ActivationInfo, true);

        // Activate a new ability
        ActorInfo->AbilitySystemComponent->TryActivateAbility(Handle);
    }
}


void UGKGameplayAbility::SetupCancelByTags(const FGameplayAbilityActorInfo* ActorInfo) {
    if (CancelledByTags.IsValid()) {
        auto Delegate = FOnGameplayEffectTagCountChanged::FDelegate::CreateUObject(
            this,
            &UGKGameplayAbility::CancelAbilityFromTag
        );

        CancelByTagsDelegateHandle = Delegate.GetHandle();

        ActorInfo->AbilitySystemComponent->RegisterGenericGameplayTagEvent().Add(Delegate);
    }
}

void UGKGameplayAbility::CancelAbilityFromTag(const FGameplayTag Tag, int32 Count) {
    if (CancelledByTags.HasTag(Tag)) {
        K2_CancelAbility();
        ClearCancelByTags(CurrentActorInfo);
    }
}

void UGKGameplayAbility::ClearCancelByTags(const FGameplayAbilityActorInfo* ActorInfo) {
    if (CancelByTagsDelegateHandle.IsValid()) {
        ActorInfo->AbilitySystemComponent->RegisterGenericGameplayTagEvent().Remove(CancelByTagsDelegateHandle);
    }
}

/*
// Applies a gameplay effect container, by creating and then applying the spec
UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "EventData"))
virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainer(FGameplayTag              ContainerTag,
    const FGameplayEventData& EventData,
    int32 OverrideGameplayLevel = -1);


TArray<FActiveGameplayEffectHandle> UGKGameplayAbility::ApplyEffectContainer(FGameplayTag              ContainerTag,
                                                                             const FGameplayEventData &EventData,
                                                                             int32 OverrideGameplayLevel)
{
    FGKGameplayEffectContainerSpec Spec = MakeEffectContainerSpec(ContainerTag, EventData, OverrideGameplayLevel);
    return ApplyEffectContainerSpec(Spec);
}

// Search for and make a gameplay effect container spec to be applied later, from the EffectContainerMap 
virtual FGKGameplayEffectContainerSpec MakeEffectContainerSpec(FGameplayTag              ContainerTag,
    const FGameplayEventData& EventData,
    int32 OverrideGameplayLevel = -1);

// TODO: remove this
// Map of gameplay tags to gameplay effect containers 
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GameplayEffects)
TMap<FGameplayTag, FGKGameplayEffectContainer> EffectContainerMap;

FGKGameplayEffectContainerSpec UGKGameplayAbility::MakeEffectContainerSpec(FGameplayTag              ContainerTag,
    const FGameplayEventData& EventData,
    int32 OverrideGameplayLevel)
{
    FGKGameplayEffectContainer* FoundContainer = EffectContainerMap.Find(ContainerTag);

    if (FoundContainer)
    {
        return MakeEffectContainerSpecFromContainer(*FoundContainer, EventData, OverrideGameplayLevel);
    }
    return FGKGameplayEffectContainerSpec();
}


*/