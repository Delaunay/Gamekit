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

    FGKAbilityStatic const *AbilityData = Ability->GetAbilityStatic();
    if (AbilityData && AbilityData->AbilityBehavior == EGK_ActivationBehavior::Charge)
    {
        AbilityStackTag = AbilityData->StackTag;
    }

    FGameplayTagContainer AllTags(DisableTags);
    AllTags.AddTag(AbilityStackTag);

    // Debuffs
    DisableEffectTask = UGKAsyncTask_GameplayEffectChanged::ListenForGameplayEffectChange(ASC, AllTags);

    DisableEffectTask->OnGameplayEffectAdded.AddDynamic(this, &UGKAbilityWidget::OnBeginGameplayEffect_Native);
    DisableEffectTask->OnGameplayEffectStackChanged.AddDynamic(this,
                                                               &UGKAbilityWidget::OnStackChangedGameplayEffect_Native);
    DisableEffectTask->OnGameplayEffectRemoved.AddDynamic(this, &UGKAbilityWidget::OnEndGameplayEffect_Native);

    // Targeting
    Ability->TargetingStartDelegate.AddDynamic(this, &UGKAbilityWidget::OnStartTargeting);
    Ability->TargetingResultDelegate.AddDynamic(this, &UGKAbilityWidget::OnEndTargeting);

    // Level up
    Ability->OnAbilityLevelUp.AddDynamic(this, &UGKAbilityWidget::OnAbilityLevelUp);

    // Start
    // Note Activate should be useless in that case
    if (CooldownChangedTask)
    {
        CooldownChangedTask->RegisterWithGameInstance(Ability->GetWorld());
        CooldownChangedTask->Activate();
    }

    if (AttributeChangedTask)
    {
        AttributeChangedTask->RegisterWithGameInstance(Ability->GetWorld());
        AttributeChangedTask->Activate();
    }

    // Attribute
    AttributeChangedTask =
            UGKAsyncTaskAttributeChanged::ListenForAttributesChange(ASC, Ability->GetAbilityCostAttribute());

    if (AttributeChangedTask)
    {
        AttributeChangedTask->OnAttributeChanged.AddDynamic(this,
                                                            &UGKAbilityWidget::OnAbilityInsufficientResources_Native);
    }

    // TODO: Note: for abilities with charges the cooldown is not about the ability it is about
    // the task that generates the stacks
    FGameplayTagContainer const *CooldownTags = Ability->GetCooldownTags();
    if (CooldownTags != nullptr && !CooldownTags->IsEmpty())
    {
        CooldownChangedTask = UGKAsyncTaskCooldownChanged::ListenForCooldownChange(ASC, *CooldownTags, true);
        CooldownChangedTask->OnCooldownBegin.AddDynamic(this, &UGKAbilityWidget::OnAbilityCooldownBegin_Native);
        CooldownChangedTask->OnCooldownEnd.AddDynamic(this, &UGKAbilityWidget::OnAbilityCooldownEnd_Native);
    }

    bBound = true;
}

void UGKAbilityWidget::RemoveListeners()
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

    // This is not necessary, they get cleanup automatically already
    /*
    if (IsValid(CooldownChangedTask) && !CooldownChangedTask->IsDestroyed())
    {
        if (CooldownChangedTask->HasCooldownTags())
        {
            CooldownChangedTask->OnCooldownBegin.RemoveAll(this);
            CooldownChangedTask->OnCooldownEnd.RemoveAll(this);
        }
        CooldownChangedTask->EndTask();
    }

    if (IsValid(AttributeChangedTask) && !AttributeChangedTask->IsDestroyed())
    {
        // AttributeChangedTask->OnAttributeChanged.RemoveAll(this);
        AttributeChangedTask->EndTask();
    }
    */

    bBound = false;
}

void UGKAbilityWidget::NativeDestruct()
{
    RemoveListeners();

    UUserWidget::NativeDestruct();
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

void UGKAbilityWidget::OnBeginGameplayEffect_Native(FActiveGameplayEffectHandle EffectHandle,
                                                    UGameplayEffect            *Effect,
                                                    FGameplayTagContainer       Tags,
                                                    float                       Duration,
                                                    int                         Stack)
{
    if (DisableTags.HasAnyExact(Effect->InheritableOwnedTagsContainer.CombinedTags))
    {
        OnBeginDisabled_Native(EffectHandle, Effect, Tags, Duration, 0);
        return;
    }

    CurrentEffectHandle = EffectHandle;
    OnAbilityChargeChanged(1);
}

void UGKAbilityWidget::OnStackChangedGameplayEffect_Native(FActiveGameplayEffectHandle EffectHandle, int StackCount)
{

    if (CurrentEffectHandle == EffectHandle)
    {
        OnAbilityChargeChanged(StackCount);
    }
}

void UGKAbilityWidget::OnEndGameplayEffect_Native(FActiveGameplayEffectHandle EffectHandle)
{

    if (EffectHandle != CurrentEffectHandle)
    {
        OnEndDisabled_Native(EffectHandle);
        return;
    }

    if (CurrentEffectHandle == EffectHandle)
    {
        OnAbilityChargeChanged(0);
        CurrentEffectHandle = FActiveGameplayEffectHandle();
    }
}

// Disable Handling
// we track the count of all the stacked disables
void UGKAbilityWidget::OnBeginDisabled_Native(FActiveGameplayEffectHandle EffectHandle,
                                              UGameplayEffect            *Effect,
                                              FGameplayTagContainer       Tags,
                                              float                       Duration,
                                              int                         Stack)
{
    DisableCount += 1;
    OnBeginDisabled(EffectHandle, Effect, Tags, Duration);
}

void UGKAbilityWidget::OnEndDisabled_Native(FActiveGameplayEffectHandle EffectHandle)
{
    DisableCount -= 1 * (DisableCount > 0);
    OnEndDisabled(EffectHandle);
}

void UGKAbilityWidget::OnAbilityGranted_Implementation(FGKAbilityWidgetArgs const &Args)
{
    UGKGameplayAbility *AbilityCDO  = Cast<UGKGameplayAbility>(Args.AbilitySpec.Ability);
    FGKAbilityStatic   *AbilityData = AbilityCDO->GetAbilityStatic();
    FGameplayTag        StackTag    = AbilityData->StackTag;

    UAbilitySystemComponent *const ASC = Args.ActorInfo.AbilitySystemComponent.Get();

    // Initialize the Stack count
    if (StackTag != FGameplayTag::EmptyTag)
    {
        int Num = AbilityCDO->CheckChargeCost(Args.AbilitySpec.Handle, &Args.ActorInfo, nullptr);
        TArray<FGameplayEffectSpec> ActiveGameplayEffects;
        ASC->GetAllActiveGameplayEffectSpecs(ActiveGameplayEffects);

        FGameplayEffectQuery const Query =
                FGameplayEffectQuery::MakeQuery_MatchAllOwningTags(FGameplayTagContainer(StackTag));

        for (FGameplayEffectSpec &Effect: ActiveGameplayEffects)
        {
            // if (Effect.DynamicGrantedTags.MatchesQuery(Query))
            // {
            //     CurrentEffectHandle = ASC->FindActiveGameplayEffectHandle();
            //     break;
            // }
        }

        OnAbilityChargeChanged(Num);
    }
}