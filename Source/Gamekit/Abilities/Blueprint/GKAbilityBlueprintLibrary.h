// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Abilities/GKAbilityStatic.h"
#include "Gamekit/Abilities/GKAbilityTypes.h"
#include "Gamekit/Animation/GKAnimationSet.h"

#include "Gamekit/Abilities/GKAbilityInterface.h"
#include "Gamekit/Gamekit.h"

// Unreal Engine
#include "Abilities/GameplayAbility.h"
#include "Kismet/BlueprintFunctionLibrary.h"

// Generated
#include "GKAbilityBlueprintLibrary.generated.h"

USTRUCT(BlueprintType)
struct GAMEKIT_API FGKFailureTagMapping
{
    GENERATED_USTRUCT_BODY()

    public:
    FGKFailureTagMapping() {}

    FGKFailureTagMapping(FGameplayTag FailureTag, FText LocalReason): FailureTag(FailureTag), LocalReason(LocalReason)
    {
    }

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayTag FailureTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText LocalReason;
};

/**
 * Expose the Gameplay Ability System to the editor
 */
UCLASS()
class GAMEKIT_API UGKAbilityBlueprintLibrary: public UBlueprintFunctionLibrary
{
    GENERATED_UCLASS_BODY()

    public:
    // Debug FGameplayAbilitySpec
    // --------------------------
    UFUNCTION(BlueprintPure, Category = "Debug")
    static int GetInputIDFromAbilitySpec(const FGameplayAbilitySpec &Spec);

    UFUNCTION(BlueprintPure, Category = "Debug")
    static FGameplayAbilitySpecHandle GetAbilityHandleFromAbilitySpec(const FGameplayAbilitySpec &Spec);

    UFUNCTION(BlueprintPure, Category = "Debug")
    static class UGameplayAbility *GetGameplayAbilityFromAbilitySpec(const FGameplayAbilitySpec &Spec);

    UFUNCTION(BlueprintPure, Category = "Debug")
    static UGameplayEffect *GetGameplayEffectFromHandle(const FActiveGameplayEffectHandle &Handle);

    // TODO: This does a full copy
    UFUNCTION(BlueprintCallable, Category = "Ability|Effect")
    static FGKGameplayEffectContainerSpec AddTargetsToEffectContainerSpec(
            const FGKGameplayEffectContainerSpec &ContainerSpec,
            const TArray<FHitResult>             &HitResults,
            const TArray<AActor *>               &TargetActors);

    UFUNCTION(BlueprintCallable, Category = "Ability|Effect")
    static TArray<FActiveGameplayEffectHandle> ApplyExternalEffectContainerSpec(
            const FGKGameplayEffectContainerSpec &ContainerSpec);

    //! Safe way to query the Animation set, should always return a valid animation
    //! NB: the animation can change if multiple animations are available for the same
    //! animation kind
    UFUNCTION(BlueprintCallable, Category = "Ability|Animation")
    static class UAnimMontage *GetAnimation(const FGKAnimationSet &AnimationSet, EGK_AbilityAnimation AnimKind)
    {
        return AnimationSet.GetAnimations(AnimKind).Sample();
    }

    // FGameplayAbilityTargetData Getter
    // UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
    // static FHitResult GetTargetDataHitResult(FGameplayAbilityTargetData const &value,
    //                                          bool &HasHitResult);

    UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
    static FHitResult GetTargetDataHitResult(FGameplayAbilityTargetDataHandle const &Handle,
                                             int32                                   Index,
                                             bool                                   &HasHitResult);

    UFUNCTION(BlueprintPure, Category = "AbilitySlot")
    static FGKAbilitySlot AbilitySlotFromInput(EGK_MOBA_AbilityInputID AbilityInput);

    UFUNCTION(BlueprintPure, Category = "Debug")
    static FString GetGEHandleAsString(FActiveGameplayEffectHandle Handle) { return Handle.ToString(); }

    UFUNCTION(BlueprintPure, Category = "UI")
    static FText GetFailureReasonFor(UGameplayAbility *Ability, FGameplayTagContainer ReasonTags, bool &bHandled);

    UFUNCTION(BlueprintPure)
    static UEnum *GetDefaultAbilityInputEnum();

    // This is used to enable fetching a row inside a datatable in python
    UFUNCTION(BlueprintPure)
    static void GetAbilityData(class UDataTable *Table, FName Row, bool &bValid, FGKAbilityStatic &AbilityData);

    UFUNCTION(BlueprintPure)
    static FText GetDisableName(FGameplayTagContainer Tags);

    UFUNCTION(BlueprintCallable)
    static void DisableAbilityAutoGeneration(class UDataTable *Table, FName Row);

    UFUNCTION(BlueprintPure)
    static void BreakAbilitySpec(FGameplayAbilitySpec const     &AbilitySpec,
                                 UGameplayAbility              *&Ability,
                                 int                            &Level,
                                 int                            &InputID,
                                 FGameplayAbilityActivationInfo &ActivationInfo)
    {
        Ability        = AbilitySpec.Ability;
        Level          = AbilitySpec.Level;
        InputID        = AbilitySpec.InputID;
        ActivationInfo = AbilitySpec.ActivationInfo;
    }

    UFUNCTION(BlueprintPure)
    static void BreakGameplayEffectContextHandle(FGameplayEffectContextHandle const &EffectContext,
                                                 bool                               &bIsValid,
                                                 FGameplayTagContainer              &ActorTagContainer,
                                                 FGameplayTagContainer              &SpecTagContainer,
                                                 UGameplayAbility                  *&Ability,
                                                 UAbilitySystemComponent           *&AbilitySystem)
    {
        bIsValid = EffectContext.IsValid();
        EffectContext.GetOwnedGameplayTags(ActorTagContainer, SpecTagContainer);
        Ability       = const_cast<UGameplayAbility *>(EffectContext.GetAbility());
        AbilitySystem = const_cast<UAbilitySystemComponent *>(EffectContext.GetInstigatorAbilitySystemComponent());
    }

    UFUNCTION(BlueprintCallable)
    static void GetActiveEffectsWithAnyTags(UAbilitySystemComponent             *AbilitySystem,
                                            FGameplayTagContainer const         &Tags,
                                            TArray<FActiveGameplayEffectHandle> &ActiveGameplayEffects);
};
