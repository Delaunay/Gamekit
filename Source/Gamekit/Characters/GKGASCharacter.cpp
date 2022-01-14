#include "GKGASCharacter.h"

#include "Abilities/GKAbilitySystemComponent.h"
#include "Abilities/GKAttributeSet.h"
#include "Abilities/GKAbilityTypes.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"



AGKGASCharacter::AGKGASCharacter() {
    PrimaryActorTick.bCanEverTick = false;
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);
    bAlwaysRelevant = true;
    
    TagDead = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
    TagDeathDispel = FGameplayTag::RequestGameplayTag(FName("Dispel.Death"));
}

UAbilitySystemComponent* AGKGASCharacter::GetAbilitySystemComponent() const {
    return AbilitySystemComponent;
}

UGKAttributeSet* AGKGASCharacter::GetAttributeSet() const {
    return AttributeSet;
}

void AGKGASCharacter::InitializeGameplayAbilities() {
    if (GetLocalRole() != ROLE_Authority || AbilitySystemComponent == nullptr || AbilitySystemComponent->Initialized) {
        return;
    }
    
    for (TSubclassOf<UGKGameplayAbility>& Ability : DefaultAbilities)
    {
        AbilitySystemComponent->GiveAbility(
            FGameplayAbilitySpec(Ability, 1, INDEX_NONE, this)
        );
    }
    
    AbilitySystemComponent->Initialized = true;
}

void AGKGASCharacter::GetActivatableAbilities(TArray<FGameplayAbilitySpecHandle>& Abilities) {
    auto ASC = GetAbilitySystemComponent();

    for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
    {
        if ((Spec.SourceObject == this) && DefaultAbilities.Contains(Spec.Ability->GetClass()))
        {
            Abilities.Add(Spec.Handle);
        }
    }
}

void AGKGASCharacter::ClearGameplayAbilities() {
    if (GetLocalRole() != ROLE_Authority || AbilitySystemComponent == nullptr || !AbilitySystemComponent->Initialized) {
        return;
    }
    
    TArray<FGameplayAbilitySpecHandle> Abilities;
    GetActivatableAbilities(Abilities);
    
    for (auto& Ability: Abilities)
    {
        AbilitySystemComponent->ClearAbility(Ability);
    }
    
    AbilitySystemComponent->Initialized = false;
}

void AGKGASCharacter::AGKGASCharacter::Die() {
    ClearGameplayAbilities();

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCharacterMovement()->GravityScale = 0;
    GetCharacterMovement()->Velocity = FVector(0);

    OnCharacterDied.Broadcast(this);

    if (AbilitySystemComponent != nullptr)
    {
        AbilitySystemComponent->CancelAllAbilities();

        FGameplayTagContainer EffectTagsToRemove;
        EffectTagsToRemove.AddTag(TagDeathDispel);
        int32 NumEffectsRemoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectTagsToRemove);

        AbilitySystemComponent->AddLooseGameplayTag(TagDead);
    }
    
    if (DeathMontage)
    {
        PlayAnimMontage(DeathMontage);
    }
}