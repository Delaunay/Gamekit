// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Abilities/Blueprint/GKAbilityBlueprintLibrary.h"

// Gamekit
#include "Gamekit/Abilities/GKAbilitySystemComponent.h"

// Unreal Engine
#include "Misc/ConfigCacheIni.h"


UGKAbilityBlueprintLibrary::UGKAbilityBlueprintLibrary(const FObjectInitializer &ObjectInitializer):
    Super(ObjectInitializer)
{
}

int UGKAbilityBlueprintLibrary::GetInputIDFromAbilitySpec(const FGameplayAbilitySpec &Spec) { return Spec.InputID; }

FGameplayAbilitySpecHandle UGKAbilityBlueprintLibrary::GetAbilityHandleFromAbilitySpec(const FGameplayAbilitySpec &Spec)
{
    return Spec.Handle;
}

UGameplayAbility *UGKAbilityBlueprintLibrary::GetGameplayAbilityFromAbilitySpec(const FGameplayAbilitySpec &Spec)
{
    return Spec.Ability;
}

UGameplayEffect *UGKAbilityBlueprintLibrary::GetGameplayEffectFromHandle(const FActiveGameplayEffectHandle &Handle)
{
    auto ASC = const_cast<UAbilitySystemComponent *>(Handle.GetOwningAbilitySystemComponent());
    return const_cast<UGameplayEffect *>(ASC->GetGameplayEffectDefForHandle(Handle));
}

FGKGameplayEffectContainerSpec UGKAbilityBlueprintLibrary::AddTargetsToEffectContainerSpec(
        const FGKGameplayEffectContainerSpec &ContainerSpec,
        const TArray<FHitResult> &            HitResults,
        const TArray<AActor *> &              TargetActors)
{
    FGKGameplayEffectContainerSpec NewSpec = ContainerSpec;
    NewSpec.AddTargets(HitResults, TargetActors);
    return NewSpec;
}

TArray<FActiveGameplayEffectHandle> UGKAbilityBlueprintLibrary::ApplyExternalEffectContainerSpec(
        const FGKGameplayEffectContainerSpec &ContainerSpec)
{
    TArray<FActiveGameplayEffectHandle> AllEffects;

    // Iterate list of gameplay effects
    for (const FGameplayEffectSpecHandle &SpecHandle: ContainerSpec.TargetGameplayEffectSpecs)
    {
        if (SpecHandle.IsValid())
        {
            // If effect is valid, iterate list of targets and apply to all
            for (TSharedPtr<FGameplayAbilityTargetData> Data: ContainerSpec.TargetData.Data)
            {
                AllEffects.Append(Data->ApplyGameplayEffectSpec(*SpecHandle.Data.Get()));
            }
        }
    }
    return AllEffects;
}

FHitResult UGKAbilityBlueprintLibrary::GetTargetDataHitResult(FGameplayAbilityTargetDataHandle const &Handle,
                                                              int32                                   Index,
                                                              bool &                                  HasHitResult)
{

    HasHitResult           = false;
    auto const *TargetData = Handle.Get(Index);

    if (TargetData == nullptr)
    {
        return FHitResult();
    }
    auto HitResult = TargetData->GetHitResult();

    if (HitResult == nullptr)
    {
        return FHitResult();
    }

    HasHitResult = true;
    return *HitResult;
}

FGKAbilitySlot UGKAbilityBlueprintLibrary::AbilitySlotFromInput(EGK_MOBA_AbilityInputID AbilityInput)
{
    return FGKAbilitySlot(AbilityInput);
}
