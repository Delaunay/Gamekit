// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Abilities/GKAttributeSet.h"

// Gamekit
#include "Gamekit/Abilities/GKAbilitySystemComponent.h"
#include "Gamekit/Characters/GKCharacter.h"
#include "Gamekit/Controllers/GKPlayerController.h"

// Unreal Engine
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"

UGKAttributeSet::UGKAttributeSet():
    Health(100.f), MaxHealth(100.f), Mana(100.f), MaxMana(100.f), Armor(1.0f),
    MoveSpeed(600.0f), Damage(0.0f)
{
    NameToAttribute = {
        {FName("Health"), Health},
        {FName("MaxHealth"), MaxHealth},
        {FName("Mana"), Mana},
        {FName("MaxMana"), MaxMana},
        {FName("Damage"), Damage},
        {FName("Armor"), Armor},
        {FName("MoveSpeed"), MoveSpeed},
        {FName("Experience"), Experience}
    };
}

void UGKAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UGKAttributeSet, Health);
    DOREPLIFETIME(UGKAttributeSet, MaxHealth);
    DOREPLIFETIME(UGKAttributeSet, Mana);
    DOREPLIFETIME(UGKAttributeSet, MaxMana);
    DOREPLIFETIME(UGKAttributeSet, Armor);
    DOREPLIFETIME(UGKAttributeSet, Damage);
    DOREPLIFETIME(UGKAttributeSet, MoveSpeed);
    DOREPLIFETIME(UGKAttributeSet, Experience);
}

void UGKAttributeSet::OnRep_Experience(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGKAttributeSet, Experience, OldValue);
}

void UGKAttributeSet::OnRep_Health(const FGameplayAttributeData &OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGKAttributeSet, Health, OldValue);
}

void UGKAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData &OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGKAttributeSet, MaxHealth, OldValue);
}

void UGKAttributeSet::OnRep_Mana(const FGameplayAttributeData &OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGKAttributeSet, Mana, OldValue);
}

void UGKAttributeSet::OnRep_MaxMana(const FGameplayAttributeData &OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGKAttributeSet, MaxMana, OldValue);
}

void UGKAttributeSet::OnRep_Damage(const FGameplayAttributeData &OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGKAttributeSet, Damage, OldValue);
}

void UGKAttributeSet::OnRep_Armor(const FGameplayAttributeData &OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGKAttributeSet, Armor, OldValue);
}

void UGKAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData &OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGKAttributeSet, MoveSpeed, OldValue);
}

void UGKAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData &      AffectedAttribute,
                                                  const FGameplayAttributeData &MaxAttribute,
                                                  float                         NewMaxValue,
                                                  const FGameplayAttribute &    AffectedAttributeProperty)
{
    UAbilitySystemComponent *AbilityComp     = GetOwningAbilitySystemComponent();
    const float              CurrentMaxValue = MaxAttribute.GetCurrentValue();
    if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
    {
        // Change current value to maintain the current Val / Max percent
        const float CurrentValue = AffectedAttribute.GetCurrentValue();
        float       NewDelta =
                (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

        AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
    }
}

void UGKAttributeSet::PreAttributeChange(const FGameplayAttribute &Attribute, float &NewValue)
{
    // This is called whenever attributes change, so for max health/mana we want to scale the current totals to match
    Super::PreAttributeChange(Attribute, NewValue);

    if (Attribute == GetMaxHealthAttribute())
    {
        AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
    }
    else if (Attribute == GetMaxManaAttribute())
    {
        AdjustAttributeForMaxChange(Mana, MaxMana, NewValue, GetManaAttribute());
    }
    else if (Attribute == GetMoveSpeedAttribute())
    {
        SetMovementComponentMaxSpeed(NewValue);
    }
}

void UGKAttributeSet::SetMovementComponentMaxSpeed(float Value) {
    auto ActorInfo = GetActorInfo();

    if (!ActorInfo)
        return;

    auto Avatar = GetActorInfo()->AvatarActor;
    if (!Avatar.IsValid())
        return;

    
    UCharacterMovementComponent* MoveComp = Cast<UCharacterMovementComponent>(Avatar->GetComponentByClass(UCharacterMovementComponent::StaticClass()));
    
    MoveComp->MaxWalkSpeed = Value;
}

void UGKAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData &Data)
{
    Super::PostGameplayEffectExecute(Data);

    FGameplayEffectContextHandle Context    = Data.EffectSpec.GetContext();
    UAbilitySystemComponent *    Source     = Context.GetOriginalInstigatorAbilitySystemComponent();
    const FGameplayTagContainer &SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

    // Compute the delta between old and new, if it is available
    float DeltaValue = 0;
    if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive)
    {
        // If this was additive, store the raw delta value to be passed along later
        DeltaValue = Data.EvaluatedData.Magnitude;
    }

    // Get the Target actor, which should be our owner
    AActor *          TargetActor      = nullptr;
    AController *     TargetController = nullptr;
    AGKCharacterBase *TargetCharacter  = nullptr;
    if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
    {
        TargetActor      = Data.Target.AbilityActorInfo->AvatarActor.Get();
        TargetController = Cast<AController>(Data.Target.AbilityActorInfo->PlayerController.Get());
        TargetCharacter  = Cast<AGKCharacterBase>(TargetActor);
    }

    if (Data.EvaluatedData.Attribute == GetDamageAttribute())
    {
        // Get the Source actor
        AActor *          SourceActor      = nullptr;
        AController *     SourceController = nullptr;
        AGKCharacterBase *SourceCharacter  = nullptr;

        if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
        {
            SourceActor      = Source->AbilityActorInfo->AvatarActor.Get();
            SourceController = Cast<AController>(Source->AbilityActorInfo->PlayerController.Get());
            if (SourceController == nullptr && SourceActor != nullptr)
            {
                if (APawn *Pawn = Cast<APawn>(SourceActor))
                {
                    SourceController = Pawn->GetController();
                }
            }

            // Use the controller to find the source pawn
            if (SourceController)
            {
                SourceCharacter = Cast<AGKCharacterBase>(SourceController->GetPawn());
            }
            else
            {
                SourceCharacter = Cast<AGKCharacterBase>(SourceActor);
            }

            // Set the causer actor based on context if it's set
            if (Context.GetEffectCauser())
            {
                SourceActor = Context.GetEffectCauser();
            }
        }

        // Try to extract a hit result
        FHitResult HitResult;
        if (Context.GetHitResult())
        {
            HitResult = *Context.GetHitResult();
        }

        // Store a local copy of the amount of damage done and clear the damage attribute
        const float LocalDamageDone = GetDamage();
        SetDamage(0.f);

        if (LocalDamageDone > 0)
        {
            // Apply the health change and then clamp it
            const float OldHealth = GetHealth();
            SetHealth(FMath::Clamp(OldHealth - LocalDamageDone, 0.0f, GetMaxHealth()));

            if (TargetCharacter)
            {
                // This is proper damage
                // TargetCharacter->HandleDamage(LocalDamageDone, HitResult, SourceTags, SourceCharacter, SourceActor);

                // Call for all health changes
                // TargetCharacter->HandleHealthChanged(-LocalDamageDone, SourceTags);
            }
        }
    }
    else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        // Handle other health changes such as from healing or direct modifiers
        // First clamp it
        SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));

        if (TargetCharacter)
        {
            // Call for all health changes
            // TargetCharacter->HandleHealthChanged(DeltaValue, SourceTags);
        }
    }
    else if (Data.EvaluatedData.Attribute == GetManaAttribute())
    {
        // Clamp mana
        SetMana(FMath::Clamp(GetMana(), 0.0f, GetMaxMana()));

        if (TargetCharacter)
        {
            // Call for all mana changes
            // TargetCharacter->HandleManaChanged(DeltaValue, SourceTags);
        }
    }
    else if (Data.EvaluatedData.Attribute == GetMoveSpeedAttribute())
    {
        if (TargetCharacter)
        {
            // Call for all movespeed changes
            // TargetCharacter->HandleMoveSpeedChanged(DeltaValue, SourceTags);
        }
    }
}

FGameplayAttributeData UGKAttributeSet::GetAttribute(FName AttributeName)
{
    FGameplayAttributeData *Result = NameToAttribute.Find(AttributeName);
    if (Result == nullptr)
    {
        return FGameplayAttributeData();
    }

    return *Result;
}

float UGKAttributeSet::GetBaseValue(FName AttributeName) { return GetAttribute(AttributeName).GetBaseValue(); }

float UGKAttributeSet::GetCurrentValue(FName AttributeName) { return GetAttribute(AttributeName).GetCurrentValue(); }