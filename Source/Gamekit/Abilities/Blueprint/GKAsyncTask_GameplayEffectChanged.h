// BSD 3-Clause License Copyright (c) 2021, Pierre Delaunay All rights reserved.
#pragma once

#include "AbilitySystemComponent.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"

#include "GKAsyncTask_GameplayEffectChanged.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FGKOnGameplayEffectAdded,
                                              FActiveGameplayEffectHandle,
                                              GameplayHandle,
                                              UGameplayEffect *,
                                              GameplayEffect,
                                              FGameplayTagContainer,
                                              GameplayTags,
                                              float,
                                              Duration);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGKOnGameplayEffectStackChanged,
                                             FActiveGameplayEffectHandle,
                                             GameplayHandle,
                                             int32,
                                             StackCount);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGKOnGameplayEffectRemoved, 
                                            FActiveGameplayEffectHandle, 
                                            GameplayHandle);

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
            UAbilitySystemComponent *AbilitySystemComponent);

    public:
    virtual void OnGameplayEffectAdded_Native(UAbilitySystemComponent *   Target,
                                              const FGameplayEffectSpec & SpecApplied,
                                              FActiveGameplayEffectHandle ActiveHandle);

    virtual void OnGameplayEffectRemoved_Native(const FActiveGameplayEffect &EffectRemoved);

    virtual void OnGameplayEffectStackChange_Native(FActiveGameplayEffectHandle EffectHandle,
                                                    int32                       NewStackCount,
                                                    int32                       PreviousStackCount);

    public:
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
