// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Abilities/GKGameplayAbility.h"
#include "Gamekit/Items/GKInventoryInterface.h"

// Unreal Engine
#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"

// Generated
#include "GKAbilityWidget.generated.h"

/* Simple Widget that receives ability state update through a top level UGKAbilityWidgetContainer
 */
UCLASS(BlueprintType, Blueprintable, meta = (DisableNativeTick))
class GAMEKIT_API UGKAbilityWidget: public UUserWidget
{
    // I tried to implement Delegate binding here but I get a LOT of issues
    // from the ability not bein instantiated to some event tag simply not firing
    GENERATED_BODY()

    public:
    //! Query that match any gameplay effect that disable ability casting
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTagContainer DisableTags;

    UFUNCTION(BlueprintCallable)
    bool IsDisabled() const { return DisableCount != 0; }

    //! This ability cooldown started
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Ability)
    void OnAbilityCooldownBegin(float TimeRemaining, float Duration);

    //! This ability cooldown ended
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Ability)
    void OnAbilityCooldownEnd(float TimeRemaining, float Duration);

    //! This ability leveled up
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Ability)
    void OnAbilityLevelUp(int Level);

    //! User is currently selecting a target for this ability
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Ability)
    void OnStartTargeting();

    //! User finished selecting a target for this ability
    //! either the target was acquired or the targeting got cancelled
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Ability)
    void OnEndTargeting(bool Cancelled);

    //! Disabled because of a debuff
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Ability)
    void OnAbilityDisabled(bool Enabled);

    //! Mana is lacking
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Ability)
    void OnAbilityInsufficientResources(bool CostMet);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Ability)
    void OnBeginDisabled(FActiveGameplayEffectHandle EffectHandle, UGameplayEffect* Effect, FGameplayTagContainer Tags, float Duration);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Ability)
    void OnEndDisabled(FActiveGameplayEffectHandle EffectHandle);

    void OnBeginDisabled_Implementation(FActiveGameplayEffectHandle EffectHandle, UGameplayEffect* Effect, FGameplayTagContainer Tags, float Duration) {}

    void OnEndDisabled_Implementation(FActiveGameplayEffectHandle EffectHandle) {}

    virtual void OnAbilityCooldownBegin_Implementation(float TimeRemaining, float Duration) {}

    virtual void OnAbilityCooldownEnd_Implementation(float TimeRemaining, float Duration) {}

    virtual void OnAbilityLevelUp_Implementation(int Level) {}

    virtual void OnStartTargeting_Implementation() {}

    virtual void OnEndTargeting_Implementation(bool Cancelled) {}

    virtual void OnAbilityDisabled_Implementation(bool Enabled) {}

    virtual void OnAbilityInsufficientResources_Implementation(bool CostMet) {}

    UFUNCTION(BlueprintCallable, BlueprintCosmetic)
    virtual void SetupListeners(class UGKGameplayAbility *AbilityIn);

    UFUNCTION(BlueprintCallable, BlueprintCosmetic)
    void RemoveListeners();

    void NativeDestruct() override;

    protected:
    UPROPERTY()
    bool bBound;

    class UGKGameplayAbility *Ability;

    class UGKAsyncTaskAttributeChanged *AttributeChangedTask;

    class UGKAsyncTaskCooldownChanged *CooldownChangedTask;

    class UGKAsyncTask_GameplayEffectChanged* DisableEffectTask;

    UFUNCTION()
    void OnAbilityInsufficientResources_Native(FGameplayAttribute Attribute, float NewValue, float OldValue);

    UFUNCTION()
    void OnAbilityCooldownBegin_Native(FGameplayTag CooldownTag, float TimeRemaining, float Duration);

    UFUNCTION()
    void OnAbilityCooldownEnd_Native(FGameplayTag CooldownTag, float TimeRemaining, float Duration);


    // Disable Handling
    // we track the count of all the stacked disables
    UFUNCTION()
    void OnBeginDisabled_Native(FActiveGameplayEffectHandle EffectHandle, UGameplayEffect* Effect, FGameplayTagContainer Tags, float Duration, int Stack) {
        DisableCount += 1;
        OnBeginDisabled(EffectHandle, Effect, Tags, Duration);
    }

    UFUNCTION()
    void OnEndDisabled_Native(FActiveGameplayEffectHandle EffectHandle) {
        DisableCount -= 1 * (DisableCount > 0);
        OnEndDisabled(EffectHandle);
    }

    UPROPERTY()
    int DisableCount;
};
