// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Abilities/Execution/GKDamageExecution.h"

// Gamekit
#include "Gamekit/Abilities/GKAbilitySystemComponent.h"
#include "Gamekit/Abilities/GKAttributeSet.h"

struct GKDamageStatics
{
    DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
    DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);

    GKDamageStatics()
    {
        // Capture the Target's DefensePower attribute. Do not snapshot it, because we want to use the health value at
        // the moment we apply the execution.
        DEFINE_ATTRIBUTE_CAPTUREDEF(UGKAttributeSet, Armor, Target, false);

        // Also capture the source's raw Damage, which is normally passed in directly via the execution
        DEFINE_ATTRIBUTE_CAPTUREDEF(UGKAttributeSet, Damage, Source, true);
    }
};

static const GKDamageStatics &DamageStatics()
{
    static GKDamageStatics DmgStatics;
    return DmgStatics;
}

UGKDamageExecution::UGKDamageExecution()
{
    RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
    RelevantAttributesToCapture.Add(DamageStatics().DamageDef);
}

void UGKDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters &ExecutionParams,
                                                OUT FGameplayEffectCustomExecutionOutput &OutExecutionOutput) const
{
    UAbilitySystemComponent *TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
    UAbilitySystemComponent *SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

    AActor *SourceActor =
            SourceAbilitySystemComponent ? SourceAbilitySystemComponent->GetAvatarActor_Direct() : nullptr;
    AActor *TargetActor =
            TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor_Direct() : nullptr;

    const FGameplayEffectSpec &Spec = ExecutionParams.GetOwningSpec();

    // Gather the tags from the source and target as that can affect which buffs should be used
    const FGameplayTagContainer *SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer *TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    FAggregatorEvaluateParameters EvaluationParameters;
    EvaluationParameters.SourceTags = SourceTags;
    EvaluationParameters.TargetTags = TargetTags;

    // --------------------------------------
    //	Damage Done = Damage * AttackPower / DefensePower
    //	If DefensePower is 0, it is treated as 1.0
    // --------------------------------------

    float DefensePower = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, DefensePower);

    if (DefensePower == 0.0f)
    {
        DefensePower = 1.0f;
    }

    float AttackPower = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef, EvaluationParameters, AttackPower);

    float Damage = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef, EvaluationParameters, Damage);

    float DamageDone = Damage * AttackPower / DefensePower;
    if (DamageDone > 0.f)
    {
        OutExecutionOutput.AddOutputModifier(
                FGameplayModifierEvaluatedData(DamageStatics().DamageProperty, EGameplayModOp::Additive, DamageDone));
    }
}