// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Abilities/GKAbilityTypes.h"
#include "Gamekit/Animation/GKAnimationSet.h"
#include "Gamekit/Gamekit.h"
#include "Gamekit/Items/GKItemTypes.h"

// Unreal Engine
#include "Abilities/GameplayAbility.h"
#include "Kismet/BlueprintFunctionLibrary.h"

// Generated
#include "GKAbilityBlueprintLibrary.generated.h"

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
    FGKGameplayEffectContainerSpec AddTargetsToEffectContainerSpec(const FGKGameplayEffectContainerSpec &ContainerSpec,
                                                                   const TArray<FHitResult> &            HitResults,
                                                                   const TArray<AActor *> &              TargetActors);

    UFUNCTION(BlueprintCallable, Category = "Ability|Effect")
    TArray<FActiveGameplayEffectHandle> ApplyExternalEffectContainerSpec(
            const FGKGameplayEffectContainerSpec &ContainerSpec);

    //! Safe way to query the Animation set, should always return a valid animation
    //! NB: the animation can change if multiple animations are available for the same
    //! animation kind
    UFUNCTION(BlueprintCallable, Category = "Ability|Animation")
    class UAnimMontage *GetAnimation(const FGKAnimationSet &AnimationSet, EGK_AbilityAnimation AnimKind)
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
                                             bool &                                  HasHitResult);

    UFUNCTION(BlueprintPure, Category = "AbilitySlot")
    static FGKAbilitySlot AbilitySlotFromInput(EGK_MOBA_AbilityInputID AbilityInput);

    UFUNCTION(BlueprintPure, Category = "Debug")
    static FString GetGEHandleAsString(FActiveGameplayEffectHandle Handle){
        return Handle.ToString();
    }
};