// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

// Include
#include "Gamekit/Abilities/GKAbilitySystemComponent.h"

// Gamekit
#include "Gamekit/Abilities/GKAbilityStatic.h"
#include "Gamekit/Abilities/GKGameplayAbility.h"
#include "Gamekit/Abilities/Targeting/GKAbilityTarget_PlayerControllerTrace.h"
#include "Gamekit/Characters/GKCharacter.h"

// Generated
#include "AbilitySystemGlobals.h"

UGKAbilitySystemComponent::UGKAbilitySystemComponent()
{
    ReplicationMode = EGameplayEffectReplicationMode::Mixed;
    AbilityFailedCallbacks.AddUObject(this, &UGKAbilitySystemComponent::OnAbilityFailed_Native);
}

void UGKAbilitySystemComponent::OnAbilityFailed_Native(const UGameplayAbility *     Ability,
                                                       const FGameplayTagContainer &FailureReason)
{
    OnAbilityFailed.Broadcast(Ability, FailureReason);
}


void UGKAbilitySystemComponent::OnRemoveAbility(FGameplayAbilitySpec& Spec) {
    Super::OnRemoveAbility(Spec);

    AGKCharacterBase* Character = Cast<AGKCharacterBase>(GetOwner());

    if (Character == nullptr)
    {
        UE_LOG(LogGamekit,
            Warning,
            TEXT("Removing an Ability to a non AGKCharacterBase, replication will not work %s"),
            *GetOwner()->StaticClass()->GetName());
        return;
    }

    // "Grant" Client Ability
    Character->OnAbilityRemoved_Native(Spec);
}

void UGKAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec &AbilitySpec)
{
    // Generate the ActorInfo an set it on the instance
    Super::OnGiveAbility(AbilitySpec);

    if (GetOwner() == nullptr)
    {
        UE_LOG(LogGamekit, Warning, TEXT("Ability does not have a owner!?"));
        return;
    }

    AGKCharacterBase *Character = Cast<AGKCharacterBase>(GetOwner());
    if (Character == nullptr)
    {
        UE_LOG(LogGamekit,
               Warning,
               TEXT("Granting an Ability to a non AGKCharacterBase, replication will not work %s"),
               *GetOwner()->StaticClass()->GetName());
        return;
    }

    // "Grant" Client Ability
    Character->OnNewAbility_Native(AbilitySpec);
}

UGKAbilitySystemComponent *UGKAbilitySystemComponent::GetAbilitySystemComponentFromActor(const AActor *Actor,
                                                                                         bool          LookForComponent)
{
    return Cast<UGKAbilitySystemComponent>(
            UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, LookForComponent));
}

bool UGKAbilitySystemComponent::IsInitialized() const { return Initialized; }

void UGKAbilitySystemComponent::CancelAllPendingAbilities()
{
    // TODO: what effect does cancel have on Passive/Toggled abilities ?
    ABILITYLIST_SCOPE_LOCK();

    for (FGameplayAbilitySpec &Spec: ActivatableAbilities.Items)
    {
        if (Spec.IsActive())
        {
            CancelAbilitySpec(Spec, nullptr);
        }
    }

    // Cancel backswing, -1 tries to make the animation blendout seaminglessly
    // 0.f make a hard reset
    CurrentMontageStop(0.f);
}

void UGKAbilitySystemComponent::LevelUpAbility(FGameplayAbilitySpecHandle Handle) { ServerLevelUpAbility(Handle); }

void UGKAbilitySystemComponent::ServerLevelUpAbility_Implementation(FGameplayAbilitySpecHandle Handle)
{
    FGameplayAbilitySpec *Spec = FindAbilitySpecFromHandle(Handle);

    if (!Spec)
    {
        return;
    }

    auto Ability = Cast<UGKGameplayAbility>(Spec->Ability);

    if (!Ability)
    {
        return;
    }

    FGKAbilityStatic *AbilityData = Ability->GetAbilityStatic();

    if (!AbilityData || Spec->Level + 1 > AbilityData->MaxLevel)
    {
        return;
    }

    Spec->Level += 1;
    MarkAbilitySpecDirty(*Spec, false);
    ClientLevelUpAbility_Result(Handle, Spec->Level);
    return;
};

//*
void UGKAbilitySystemComponent::ClientLevelUpAbility_Result_Implementation(FGameplayAbilitySpecHandle Handle, int Level)
{
    FGameplayAbilitySpec *Spec = FindAbilitySpecFromHandle(Handle);

    auto DefaultAbility = Spec->Ability;
    if (DefaultAbility->GetInstancingPolicy() == EGameplayAbilityInstancingPolicy::InstancedPerActor)
    {
        DefaultAbility = Spec->GetPrimaryInstance();
    }

    auto Ability = Cast<UGKGameplayAbility>(DefaultAbility);

    if (Ability != nullptr && Level > 0)
    {
        Ability->OnAbilityLevelUp.Broadcast(Level);
    }
}

// this cannot work since we need to be able to instantiate blueprint classes
TSubclassOf<AGKAbilityTarget_Actor> UGKAbilitySystemComponent::GetAbilityTarget_ActorClass(EGK_TargetingMode Mode)
{
    switch (Mode)
    {
    case EGK_TargetingMode::PlayerControllerTrace:
        return TSubclassOf<AGKAbilityTarget_Actor>(AGKAbilityTarget_PlayerControllerTrace::StaticClass());

    default:
    case EGK_TargetingMode::None:
        return TSubclassOf<AGKAbilityTarget_Actor>();
    }
}

AGKAbilityTarget_Actor *UGKAbilitySystemComponent::GetAbilityTarget_Actor(EGK_TargetingMode Mode)
{
    return GetAbilityTarget_Actor(GetAbilityTarget_ActorClass(Mode));
}

AGKAbilityTarget_Actor *UGKAbilitySystemComponent::GetAbilityTarget_Actor(
        TSubclassOf<AGKAbilityTarget_Actor> AbilityTarget_ActorClass)
{
    auto AbilityTarget_Actor = AbilityTarget_ActorCache.FindRef(AbilityTarget_ActorClass);

    if (!IsValid(AbilityTarget_Actor) && AbilityTarget_ActorClass)
    {
        // NewObject does a lot LESS than SpawnActor
        // I have to check it in more depth to know how to register a newly created object
        // AbilityTarget_Actor = NewObject<AGKAbilityTarget_Actor>(this, GetAbilityTarget_ActorClass());

        AbilityTarget_Actor = GetWorld()->SpawnActor<AGKAbilityTarget_Actor>(AbilityTarget_ActorClass);
        AbilityTarget_ActorCache.Add(AbilityTarget_ActorClass, AbilityTarget_Actor);
    }

    return AbilityTarget_Actor;
}

void UGKAbilitySystemComponent::TryActivateAbility_Point(FGameplayAbilitySpecHandle Handle, FVector Point)
{
    ServerTryActivateAbility_Point(Handle, Point);
}

void UGKAbilitySystemComponent::ServerTryActivateAbility_Point_Implementation(FGameplayAbilitySpecHandle Handle,
                                                                              FVector                    Point)
{
    FGameplayAbilitySpec *Spec = FindAbilitySpecFromHandle(Handle);

    if (!Spec)
    {
        return;
    }

    auto Ability = Cast<UGKGameplayAbility>(Spec->Ability);

    if (!Ability)
    {
        return;
    }

    Ability->ActivateManual_PointTarget(Point);
}


void UGKAbilitySystemComponent::BindAbilityActivationToInputComponentFromEnum(UInputComponent* InputComponent, FGKGameplayAbilityInputBinds BindInfo) {

    UEnum* EnumBinds = BindInfo.AbilityInputEnum;
    bIsNetDirty = true;
    GetBlockedAbilityBindings_Mutable().SetNumZeroed(EnumBinds->NumEnums());

    for (int32 idx = 0; idx < EnumBinds->NumEnums(); ++idx)
    {
        const FString FullStr = EnumBinds->GetNameStringByIndex(idx);

        // Pressed event
        {
            FInputActionBinding AB(FName(*FullStr), IE_Pressed);
            AB.ActionDelegate.GetDelegateForManualSet().BindUObject(this, &UAbilitySystemComponent::AbilityLocalInputPressed, idx);
            InputComponent->AddActionBinding(AB);
        }

        // Released event
        {
            FInputActionBinding AB(FName(*FullStr), IE_Released);
            AB.ActionDelegate.GetDelegateForManualSet().BindUObject(this, &UAbilitySystemComponent::AbilityLocalInputReleased, idx);
            InputComponent->AddActionBinding(AB);
        }
    }

    // Bind Confirm/Cancel. Note: these have to come last!
    if (BindInfo.ConfirmCommand.IsEmpty() == false)
    {
        FInputActionBinding AB(FName(*BindInfo.ConfirmCommand), IE_Pressed);
        AB.ActionDelegate.GetDelegateForManualSet().BindUObject(this, &UAbilitySystemComponent::LocalInputConfirm);
        InputComponent->AddActionBinding(AB);
    }

    if (BindInfo.CancelCommand.IsEmpty() == false)
    {
        FInputActionBinding AB(FName(*BindInfo.CancelCommand), IE_Pressed);
        AB.ActionDelegate.GetDelegateForManualSet().BindUObject(this, &UAbilitySystemComponent::LocalInputCancel);
        InputComponent->AddActionBinding(AB);
    }

    if (BindInfo.CancelTargetInputID >= 0)
    {
        GenericCancelInputID = BindInfo.CancelTargetInputID;
    }
    if (BindInfo.ConfirmTargetInputID >= 0)
    {
        GenericConfirmInputID = BindInfo.ConfirmTargetInputID;
    }
}