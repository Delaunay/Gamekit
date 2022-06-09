#include "Gamekit/Widget/GKAbilityWidget.h"

// Gamekit
#include "Gamekit/Abilities/Blueprint/GKAsyncTaskAttributeChanged.h"
#include "Gamekit/Abilities/Blueprint/GKAsyncTaskCooldownChanged.h"
#include "Gamekit/Abilities/Blueprint/GKAsyncTask_GameplayEffectChanged.h"
#include "Gamekit/Abilities/GKGameplayAbility.h"

void UGKAbilityWidget::SetupListeners(class UGKGameplayAbility *InAbility)
{
    DisableCount = 0;

    if (!InAbility)
    {
        return;
    }

    if (bBound)
    {
        UE_LOG(LogGamekit, Warning, TEXT("Calling SetupListeners twice"));
        return;
    }

    Ability  = InAbility;
    auto ASC = Ability->GetAbilitySystemComponentFromActorInfo();

    // Attribute
    AttributeChangedTask = UGKAsyncTaskAttributeChanged::ListenForAttributesChange(ASC, Ability->GetAbilityCostAttribute());

    if (AttributeChangedTask) {
         AttributeChangedTask->OnAttributeChanged.AddDynamic(this, &UGKAbilityWidget::OnAbilityInsufficientResources_Native);
    }

    FGameplayTagContainer const* CooldownTags = Ability->GetCooldownTags();
    if (CooldownTags != nullptr && !CooldownTags->IsEmpty()){
        CooldownChangedTask = UGKAsyncTaskCooldownChanged::ListenForCooldownChange(ASC, *CooldownTags, true);
        CooldownChangedTask->OnCooldownBegin.AddDynamic(this, &UGKAbilityWidget::OnAbilityCooldownBegin_Native);
        CooldownChangedTask->OnCooldownEnd.AddDynamic(this, &UGKAbilityWidget::OnAbilityCooldownEnd_Native);
    }

    // Debuffs
    DisableEffectTask = UGKAsyncTask_GameplayEffectChanged::ListenForGameplayEffectChange(
        ASC,
        DisableTags
    );

    DisableEffectTask->OnGameplayEffectAdded.AddDynamic(this, &UGKAbilityWidget::OnBeginDisabled_Native);
    DisableEffectTask->OnGameplayEffectRemoved.AddDynamic(this, &UGKAbilityWidget::OnEndDisabled_Native);

    // Targeting
    Ability->TargetingStartDelegate.AddDynamic(this, &UGKAbilityWidget::OnStartTargeting);
    Ability->TargetingResultDelegate.AddDynamic(this, &UGKAbilityWidget::OnEndTargeting);

    // Level up
    Ability->OnAbilityLevelUp.AddDynamic(this, &UGKAbilityWidget::OnAbilityLevelUp);

    // Start
    // Note Activate should be useless in that case
   
    if (CooldownChangedTask){
        CooldownChangedTask->RegisterWithGameInstance(Ability->GetWorld());
        CooldownChangedTask->Activate();
    }

    if (AttributeChangedTask){
        AttributeChangedTask->RegisterWithGameInstance(Ability->GetWorld());
        AttributeChangedTask->Activate();
    }

    bBound = true;
}

void UGKAbilityWidget::RemoveListeners() {
    if (!bBound)
    {
        return;
    }

    if (Ability)
    {
        Ability->TargetingStartDelegate.RemoveAll(this);
        Ability->TargetingResultDelegate.RemoveAll(this);
        Ability->OnAbilityLevelUp.RemoveAll(this);
    }

    if (CooldownChangedTask)
    {
        CooldownChangedTask->OnCooldownBegin.RemoveAll(this);
        CooldownChangedTask->OnCooldownEnd.RemoveAll(this);
        CooldownChangedTask->EndTask();
        CooldownChangedTask->SetReadyToDestroy();
    }

    if (AttributeChangedTask)
    {
        AttributeChangedTask->OnAttributeChanged.RemoveAll(this);
        AttributeChangedTask->EndTask();
        AttributeChangedTask->SetReadyToDestroy();
    }

    bBound = false;
}

void UGKAbilityWidget::NativeDestruct()
{
    RemoveListeners(); 
}

void UGKAbilityWidget::OnAbilityInsufficientResources_Native(FGameplayAttribute Attribute,
                                                             float              NewValue,
                                                             float              OldValue)
{
    return OnAbilityInsufficientResources(Ability->K2_CheckAbilityCost());
}

void UGKAbilityWidget::OnAbilityCooldownBegin_Native(FGameplayTag CooldownTag, float TimeRemaining, float Duration)
{
    return OnAbilityCooldownBegin(TimeRemaining, Duration);
}

void UGKAbilityWidget::OnAbilityCooldownEnd_Native(FGameplayTag CooldownTag, float TimeRemaining, float Duration)
{
    return OnAbilityCooldownEnd(TimeRemaining, Duration);
}
