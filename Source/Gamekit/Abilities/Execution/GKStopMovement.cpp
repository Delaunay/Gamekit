// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Abilities/Execution/GKStopMovement.h"

// Gamekit
#include "Gamekit/Abilities/GKAbilitySystemComponent.h"
#include "Gamekit/Abilities/GKAttributeSet.h"
#include "Gamekit/Abilities/Abilities/GKMovementAbility.h"
#include "Gamekit/Abilities/GKAbilitySystemGlobals.h"

// Unreal Engine
#include "GameFramework/MovementComponent.h"


UGKStopMovement::UGKStopMovement()
{
    AbilityToStop = UGameplayAbility::StaticClass();

    AbilityTagsToStop = FGameplayTagContainer(AbilityMove);

    bUseTags = true;
}

void UGKStopMovement::Execute_Implementation(const FGameplayEffectCustomExecutionParameters &ExecutionParams,
                                             OUT FGameplayEffectCustomExecutionOutput &OutExecutionOutput) const
{
    UAbilitySystemComponent *TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();

    // Cancel Moving Ability
    if (bUseTags) {
        TargetAbilitySystemComponent->CancelAbilities(&AbilityTagsToStop);
    } else {
        UGameplayAbility* CDO = Cast<UGameplayAbility>(AbilityToStop->GetDefaultObject());
        TargetAbilitySystemComponent->CancelAbility(CDO);
    }


    // Stop all movement commands if any
    AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor_Direct() : nullptr;

    if (TargetActor) {
        UMovementComponent* Comp = Cast<UMovementComponent>(TargetActor->GetComponentByClass(UMovementComponent::StaticClass()));

        if (Comp) {
            Comp->StopMovementImmediately();
        }
    }

    OutExecutionOutput.MarkConditionalGameplayEffectsToTrigger();
}