//  MIT License Copyright(c) 2020 Dan Kestranek
#pragma once

// Unreal Engine
#include "AbilitySystemComponent.h"
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintAsyncActionBase.h"

// Generated
#include "GKAsyncTaskCooldownChanged.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCooldownChanged,
                                               FGameplayTag,
                                               CooldownTag,
                                               float,
                                               TimeRemaining,
                                               float,
                                               Duration);

/**
 * Blueprint node to automatically register a listener for changes (Begin and End) to an array of Cooldown tags.
 * Useful to use in UI.
 *
 * Avoids to update UI values on tick
 *
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = AsyncTask))
class GAMEKIT_API UGKAsyncTaskCooldownChanged: public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

    public:
    UPROPERTY(BlueprintAssignable)
    FOnCooldownChanged OnCooldownBegin;

    UPROPERTY(BlueprintAssignable)
    FOnCooldownChanged OnCooldownEnd;

    // Listens for changes (Begin and End) to cooldown GameplayEffects based on the cooldown tag.
    // UseServerCooldown determines if the Sever's cooldown is returned in addition to the local predicted cooldown.
    // If using ServerCooldown, TimeRemaining and Duration will return -1 to signal local predicted cooldown has begun.
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
    static UGKAsyncTaskCooldownChanged *ListenForCooldownChange(UAbilitySystemComponent *AbilitySystemComponent,
                                                                FGameplayTagContainer    CooldownTags,
                                                                bool                     UseServerCooldown);

    // You must call this function manually when you want the AsyncTask to end.
    // For UMG Widgets, you would call it in the Widget's Destruct event.
    UFUNCTION(BlueprintCallable)
    void EndTask();

    virtual void SetReadyToDestroy() override;

    bool HasCooldownTags() const { return DelegateHandles.Num() > 0; }

    bool IsDestroyed() const { return bDestroyed;}

    protected:
    bool bDestroyed;

    UPROPERTY()
    UAbilitySystemComponent *ASC;

    FGameplayTagContainer CooldownTags;

    TArray<FDelegateHandle> DelegateHandles;

    bool UseServerCooldown;

    virtual void OnActiveGameplayEffectAddedCallback(UAbilitySystemComponent    *Target,
                                                     const FGameplayEffectSpec  &SpecApplied,
                                                     FActiveGameplayEffectHandle ActiveHandle);

    virtual void CooldownTagChanged(const FGameplayTag CooldownTag, int32 NewCount);

    bool GetCooldownRemainingForTag(FGameplayTagContainer CooldownTags, float &TimeRemaining, float &CooldownDuration);
};
