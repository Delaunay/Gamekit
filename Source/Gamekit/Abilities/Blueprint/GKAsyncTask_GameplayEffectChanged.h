// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.
#pragma once

// Unreal Engine
#include "AbilitySystemComponent.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"

// Generated
#include "GKAsyncTask_GameplayEffectChanged.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FGKOnGameplayEffectAdded,
                                              FActiveGameplayEffectHandle, GameplayHandle,
                                              UGameplayEffect *,GameplayEffect,
                                              FGameplayTagContainer, GameplayTags,
                                              float, Duration,
                                              int, StackCount);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGKOnGameplayEffectStackChanged,
                                             FActiveGameplayEffectHandle, GameplayHandle,
                                             int, StackCount);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGKOnGameplayEffectRemoved, FActiveGameplayEffectHandle, GameplayHandle);

/**
 * Avoids to update UI values on tick
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = AsyncTask))
class GAMEKIT_API UGKAsyncTask_GameplayEffectChanged: public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

    public:
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
    static UGKAsyncTask_GameplayEffectChanged *ListenForGameplayEffectChange(
        UAbilitySystemComponent *AbilitySystemComponent,
        FGameplayTagContainer SelectedTags = FGameplayTagContainer()
    );

    public:
    virtual void OnGameplayEffectAdded_Native(UAbilitySystemComponent *   Target,
                                              const FGameplayEffectSpec & SpecApplied,
                                              FActiveGameplayEffectHandle ActiveHandle);

    virtual void OnGameplayEffectRemoved_Native(const FActiveGameplayEffect &EffectRemoved);

    virtual void OnGameplayEffectStackChange_Native(FActiveGameplayEffectHandle EffectHandle,
                                                    int32                       NewStackCount,
                                                    int32                       PreviousStackCount);

    public:


    // List all the allowed tags
    UPROPERTY()
    FGameplayTagContainer OneOf;

    UPROPERTY(BlueprintAssignable)
    FGKOnGameplayEffectAdded OnGameplayEffectAdded;

    UPROPERTY(BlueprintAssignable)
    FGKOnGameplayEffectStackChanged OnGameplayEffectStackChanged;

    UPROPERTY(BlueprintAssignable)
    FGKOnGameplayEffectRemoved OnGameplayEffectRemoved;

    public:
    UFUNCTION(BlueprintCallable)
    void EndTask();

    protected:
    UPROPERTY()
    UAbilitySystemComponent *AbilitySystemComponent;
};
