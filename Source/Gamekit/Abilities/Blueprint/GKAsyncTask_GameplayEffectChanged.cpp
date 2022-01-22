// BSD 3-Clause License Copyright (c) 2021, Pierre Delaunay All rights reserved.

#include "Abilities/Blueprint/GKAsyncTask_GameplayEffectChanged.h"

#include "TimerManager.h"

UGKAsyncTask_GameplayEffectChanged *UGKAsyncTask_GameplayEffectChanged::ListenForGameplayEffectChange(
        UAbilitySystemComponent *AbilitySystemComponent)
{
    UGKAsyncTask_GameplayEffectChanged *ListenForGameplayEffectStackChange =
            NewObject<UGKAsyncTask_GameplayEffectChanged>();
    ListenForGameplayEffectStackChange->AbilitySystemComponent = AbilitySystemComponent;

    if (!IsValid(AbilitySystemComponent))
    {
        ListenForGameplayEffectStackChange->EndTask();
        return nullptr;
    }

    // Other Delegate candidate we could be listening to
    // AbilitySystemComponent->OnGameplayEffectAppliedDelegateToTarget
    //
    // NOTE: Does not work I guess the delegates are hooked only later and miss all those calls

    /*/ Add already active effect Now
    TArray<FActiveGameplayEffectHandle> ActiveHandles =
    AbilitySystemComponent->GetActiveEffects(FGameplayEffectQuery()); for (auto ActiveHandle : ActiveHandles) {
            UGameplayEffect const* Effect = AbilitySystemComponent->GetGameplayEffectDefForHandle(ActiveHandle);

            // The Spec is not used yet so I guess this is not a big deal
            FGameplayEffectSpec FakeSpec;
            FakeSpec.Def = Effect;

            ListenForGameplayEffectStackChange->OnGameplayEffectAdded_Native(
                    AbilitySystemComponent,
                    FakeSpec,
                    ActiveHandle
            );
    }
    //----------------------------------------------------*/

    AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(
            ListenForGameplayEffectStackChange, &UGKAsyncTask_GameplayEffectChanged::OnGameplayEffectAdded_Native);

    AbilitySystemComponent->OnAnyGameplayEffectRemovedDelegate().AddUObject(
            ListenForGameplayEffectStackChange, &UGKAsyncTask_GameplayEffectChanged::OnGameplayEffectRemoved_Native);

    return ListenForGameplayEffectStackChange;
}

void UGKAsyncTask_GameplayEffectChanged::EndTask()
{
    if (IsValid(AbilitySystemComponent))
    {
        AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.RemoveAll(this);
        AbilitySystemComponent->OnAnyGameplayEffectRemovedDelegate().RemoveAll(this);
    }

    SetReadyToDestroy();
    MarkPendingKill();
}

void UGKAsyncTask_GameplayEffectChanged::OnGameplayEffectAdded_Native(UAbilitySystemComponent *   Target,
                                                                      const FGameplayEffectSpec & SpecApplied,
                                                                      FActiveGameplayEffectHandle ActiveHandle)
{
    // Other Delegate candidate we could be listening to
    // AbilitySystemComponent->OnGameplayEffectDurationChange
    // AbilitySystemComponent->OnGameplayEffectTimeChangeDelegate

    AbilitySystemComponent->OnGameplayEffectStackChangeDelegate(ActiveHandle)
            ->AddUObject(this, &UGKAsyncTask_GameplayEffectChanged::OnGameplayEffectStackChange_Native);

    UWorld *World       = AbilitySystemComponent->GetOwner()->GetWorld();
    float   CurrentTime = World->GetTimeSeconds();

    float Start;
    float Duration;
    AbilitySystemComponent->GetGameplayEffectStartTimeAndDuration(ActiveHandle, Start, Duration);

    UGameplayEffect const *EffectDef = AbilitySystemComponent->GetGameplayEffectDefForHandle(ActiveHandle);
    FGameplayTagContainer  Tags      = *AbilitySystemComponent->GetGameplayEffectTargetTagsFromHandle(ActiveHandle);

    OnGameplayEffectAdded.Broadcast(
            ActiveHandle, const_cast<UGameplayEffect *>(EffectDef), Tags, Duration - (CurrentTime - Start));
}

void UGKAsyncTask_GameplayEffectChanged::OnGameplayEffectRemoved_Native(const FActiveGameplayEffect &EffectRemoved)
{
    OnGameplayEffectRemoved.Broadcast(EffectRemoved.Handle);
}

void UGKAsyncTask_GameplayEffectChanged::OnGameplayEffectStackChange_Native(FActiveGameplayEffectHandle EffectHandle,
                                                                            int32                       NewStackCount,
                                                                            int32 PreviousStackCount)
{
    OnGameplayEffectStackChanged.Broadcast(EffectHandle, NewStackCount);
}
