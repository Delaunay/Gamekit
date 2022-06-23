// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Abilities/Blueprint/GKAbilityBlueprintLibrary.h"

// Gamekit
#include "Gamekit/Abilities/GKAbilitySystemComponent.h"
#include "Gamekit/Abilities/GKAbilitySystemGlobals.h"

// Unreal Engine
#include "Misc/ConfigCacheIni.h"
#include "NativeGameplayTags.h"

#define NAMESPACE "GamekitAbilities"

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
        const TArray<FHitResult>             &HitResults,
        const TArray<AActor *>               &TargetActors)
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
                                                              bool                                   &HasHitResult)
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

TArray<FGKFailureTagMapping> GenerateFailureMapping()
{
    // TODO: make this a datatable
    auto &ASGlobals = (UGKAbilitySystemGlobals &)(UGKAbilitySystemGlobals::Get());

    FText AbilityCost       = NSLOCTEXT(NAMESPACE, "AbilityCost", "Ability cost not met");
    FText AbilityCooldown   = NSLOCTEXT(NAMESPACE, "AbilityCooldown", "Ability is on cooldown");
    FText AbilityBlocked    = NSLOCTEXT(NAMESPACE, "AbilityBlocked", "Ability is blocked by an active effect");
    FText AbilityMissing    = NSLOCTEXT(NAMESPACE, "AbilityMissing", "Ability requirements are not met");
    FText AbilityNetwork    = NSLOCTEXT(NAMESPACE, "AbilityNetwork", "Internal error");
    FText AbilityDead       = NSLOCTEXT(NAMESPACE, "AbilityDead", "You are dead");
    FText AbilityNotLearned = NSLOCTEXT(NAMESPACE, "AbilityNotLearned", "Ability was not learnt yet");
    FText AbilityNoCharge   = NSLOCTEXT(NAMESPACE, "AbilityNoCharge", "Ability does not have charges anymore");

    return TArray<FGKFailureTagMapping>{
            FGKFailureTagMapping{FailureCost, AbilityCost},
            FGKFailureTagMapping{FailureCooldown, AbilityCooldown},
            FGKFailureTagMapping{FailureBlocked, AbilityBlocked},
            FGKFailureTagMapping{FailureMissing, AbilityMissing},
            FGKFailureTagMapping{FailureNetwork, AbilityNetwork},
            FGKFailureTagMapping{FailureDead, AbilityDead},
            FGKFailureTagMapping{FailureNotLearned, AbilityNotLearned},
            FGKFailureTagMapping{FailureCharge, AbilityNoCharge},
    };
}

TArray<FGKFailureTagMapping> const &GetFailureMapping()
{
    static TArray<FGKFailureTagMapping> Mappings = GenerateFailureMapping();
    return Mappings;
}

FText UGKAbilityBlueprintLibrary::GetFailureReasonFor(UGameplayAbility     *Ability,
                                                      FGameplayTagContainer ReasonTags,
                                                      bool                 &bHandled)
{
    FGameplayTag GameplayTag = ReasonTags.GetByIndex(0);
    bHandled                 = false;

    for (FGKFailureTagMapping const &Entry: GetFailureMapping())
    {
        if (Entry.FailureTag == GameplayTag)
        {
            bHandled = true;
            return Entry.LocalReason;
        }
    }

    return NSLOCTEXT(NAMESPACE, "AbilityUnknown", "Internal Failure reason");
}

UEnum *UGKAbilityBlueprintLibrary::GetDefaultAbilityInputEnum()
{
    static UEnum *InputEnum = StaticEnum<EGK_MOBA_AbilityInputID>();
    return InputEnum;
}

void UGKAbilityBlueprintLibrary::GetAbilityData(class UDataTable *Table,
                                                FName             Row,
                                                bool             &bValid,
                                                FGKAbilityStatic &AbilityData)
{
    FGKAbilityStatic *Result = Table->FindRow<FGKAbilityStatic>(Row, "", false);
     bValid = false;

    if (Result != nullptr)
    {
        bValid      = true;
        AbilityData = *Result;
    }
}

void UGKAbilityBlueprintLibrary::DisableAbilityAutoGeneration(class UDataTable *Table, FName Row)
{
    FGKAbilityStatic *Result = Table->FindRow<FGKAbilityStatic>(Row, "", false);

    if (Result != nullptr)
    {
        Result->bAutoGenerate = false;
    }
}