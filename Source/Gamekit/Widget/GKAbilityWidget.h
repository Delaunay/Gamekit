#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayAbilitySpec.h"

#include "Items/GKInventoryInterface.h"
#include "Abilities/GKGameplayAbility.h"

#include "GKAbilityWidget.generated.h"

/* Simple Widget that receives ability state update through a top level UGKAbilityWidgetContainer
 */
UCLASS(BlueprintType, Blueprintable, meta = (DisableNativeTick))
class GAMEKIT_API UGKAbilityWidget : public UUserWidget
{  
    // I tried to implement Delegate binding here but I get a LOT of issues
    // from the ability not bein instantiated to some event tag simply not firing
    GENERATED_BODY()

public:
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

    //! Disabled because of a debuf
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Ability)
    void OnAbilityDisabled(bool Enabled);

    //! Mana is lacking
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Ability)
    void OnAbilityInsufficientResources(bool CostMet);

    virtual void OnAbilityCooldownBegin_Implementation(float TimeRemaining, float Duration) {}

    virtual void OnAbilityCooldownEnd_Implementation(float TimeRemaining, float Duration) {}

    virtual void OnAbilityLevelUp_Implementation(int Level) {}

    virtual void OnStartTargeting_Implementation() {}

    virtual void OnEndTargeting_Implementation(bool Cancelled) {}

    virtual void OnAbilityDisabled_Implementation(bool Enabled) {}

    virtual void OnAbilityInsufficientResources_Implementation(bool CostMet) {}

    UFUNCTION(BlueprintCallable)
    virtual void SetupListeners(class UGKGameplayAbility* AbilityIn);

    void NativeDestruct() override;

protected:
    UPROPERTY()
    bool                      bBound;

    class UGKGameplayAbility* Ability;

    class UGKAsyncTaskAttributeChanged* AttributeChangedTask;

    class UGKAsyncTaskCooldownChanged* CooldownChangedTask;

    UFUNCTION()
    void OnAbilityInsufficientResources_Native(FGameplayAttribute Attribute, float NewValue, float OldValue);

    UFUNCTION()
    void OnAbilityCooldownBegin_Native(FGameplayTag CooldownTag, float TimeRemaining, float Duration);

    UFUNCTION()
    void OnAbilityCooldownEnd_Native(FGameplayTag CooldownTag, float TimeRemaining, float Duration);
};
