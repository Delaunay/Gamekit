#include "Gamekit/Widget/GKAbilityWidget.h"

// Gamekit
#include "Gamekit/Abilities/Blueprint/GKAsyncTaskAttributeChanged.h"
#include "Gamekit/Abilities/Blueprint/GKAsyncTaskCooldownChanged.h"
#include "Gamekit/Abilities/GKGameplayAbility.h"

void UGKAbilityWidget::SetupListeners(class UGKGameplayAbility *InAbility)
{
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
    AttributeChangedTask =
            UGKAsyncTaskAttributeChanged::ListenForAttributesChange(ASC, Ability->GetAbilityCostAttribute());

    AttributeChangedTask->OnAttributeChanged.AddDynamic(this, &UGKAbilityWidget::OnAbilityInsufficientResources_Native);

    CooldownChangedTask = UGKAsyncTaskCooldownChanged::ListenForCooldownChange(ASC, *Ability->GetCooldownTags(), true);

    CooldownChangedTask->OnCooldownBegin.AddDynamic(this, &UGKAbilityWidget::OnAbilityCooldownBegin_Native);
    CooldownChangedTask->OnCooldownEnd.AddDynamic(this, &UGKAbilityWidget::OnAbilityCooldownEnd_Native);

    // Targeting
    Ability->TargetingStartDelegate.AddDynamic(this, &UGKAbilityWidget::OnStartTargeting);
    Ability->TargetingResultDelegate.AddDynamic(this, &UGKAbilityWidget::OnEndTargeting);

    // Level up
    Ability->OnAbilityLevelUp.AddDynamic(this, &UGKAbilityWidget::OnAbilityLevelUp);

    // Listen to Gameplay effect
    // TODO: Issue [#6]

    // Start
    // Note Activate should be useless in that case
    CooldownChangedTask->RegisterWithGameInstance(Ability->GetWorld());
    AttributeChangedTask->RegisterWithGameInstance(Ability->GetWorld());

    CooldownChangedTask->Activate();
    AttributeChangedTask->Activate();

    bBound = true;
}

void UGKAbilityWidget::NativeDestruct()
{
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