// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Abilities/Targeting/GKAbilityTask_WaitTargetData.h"

// Gamekit
#include "Gamekit/Abilities/Targeting/GKAbilityTarget_Actor.h"
#include "Gamekit/GKLog.h"

// Unreal Engine
#include "AbilitySystemComponent.h"
#include "Engine/Engine.h"
#include "EngineGlobals.h"

UGKAbilityTask_WaitForTargetData::UGKAbilityTask_WaitForTargetData(const FObjectInitializer &ObjectInitializer):
    Super(ObjectInitializer)
{
}

UGKAbilityTask_WaitForTargetData *UGKAbilityTask_WaitForTargetData::WaitForTargetDataUsingActor(
        UGameplayAbility *                                OwningAbility,
        FName                                             TaskInstanceName,
        TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType,
        AGKAbilityTarget_Actor *                          InTargetActor)
{
    UGKAbilityTask_WaitForTargetData *MyObj = NewAbilityTask<UGKAbilityTask_WaitForTargetData>(
            OwningAbility, TaskInstanceName); // Register for task list here, providing a given FName as a key
    MyObj->TargetActor      = InTargetActor;
    MyObj->ConfirmationType = ConfirmationType;
    return MyObj;
}

void UGKAbilityTask_WaitForTargetData::Activate()
{
    // Need to handle case where target actor was passed into task
    if (!Ability)
    {
        return;
    }

    if (!IsValid(TargetActor))
    {
        EndTask();
        return;
    }

    RegisterTargetDataCallbacks();

    if (!IsValid(this))
    {
        return;
    }

    if (ShouldProduceTargetData())
    {
        InitializeTargetActor();
        FinalizeTargetActor();
        // Note that the call to FinalizeTargetActor, this task could finish and our owning ability may be ended.
    }
}

bool UGKAbilityTask_WaitForTargetData::ShouldProduceTargetData() const
{
    check(Ability);

    // Spawn the actor if this is a locally controlled ability (always) or if this is a replicating targeting mode.
    // (E.g., server will spawn this target actor to replicate to all non owning clients)

    const AGameplayAbilityTargetActor *CDO =
            CastChecked<AGameplayAbilityTargetActor>(TargetActor->GetClass()->GetDefaultObject());

    const bool bReplicates                      = CDO->GetIsReplicated();
    const bool bIsLocallyControlled             = Ability->GetCurrentActorInfo()->IsLocallyControlled();
    const bool bShouldProduceTargetDataOnServer = CDO->ShouldProduceTargetDataOnServer;

    return (bReplicates || bIsLocallyControlled || bShouldProduceTargetDataOnServer);
}

void UGKAbilityTask_WaitForTargetData::InitializeTargetActor() const
{
    check(TargetActor);
    check(Ability);

    TargetActor->MasterPC = Ability->GetCurrentActorInfo()->PlayerController.Get();

    // If we spawned the target actor, always register the callbacks for when the data is ready.
    TargetActor->TargetDataReadyDelegate.AddUObject(const_cast<UGKAbilityTask_WaitForTargetData *>(this),
                                                    &UGKAbilityTask_WaitForTargetData::OnTargetDataReadyCallback);

    TargetActor->CanceledDelegate.AddUObject(const_cast<UGKAbilityTask_WaitForTargetData *>(this),
                                             &UGKAbilityTask_WaitForTargetData::OnTargetDataCancelledCallback);
}

void UGKAbilityTask_WaitForTargetData::FinalizeTargetActor() const
{
    check(TargetActor);
    check(Ability);

    // User ability activation is inhibited while this is active
    AbilitySystemComponent->SpawnedTargetActors.Push(TargetActor);

    TargetActor->AbilitySystemComponent = AbilitySystemComponent;
    TargetActor->StartTargeting(Ability);

    if (TargetActor->ShouldProduceTargetData())
    {
        // If instant confirm, then stop targeting immediately.
        // Note this is kind of bad: we should be able to just call a static func on the CDO to do this.
        // But then we wouldn't get to set ExposeOnSpawnParameters.
        if (ConfirmationType == EGameplayTargetingConfirmation::Instant)
        {
            TargetActor->ConfirmTargeting();
        }
        else if (ConfirmationType == EGameplayTargetingConfirmation::UserConfirmed)
        {
            // Enable our target actor that does act as a reticle as well
            TargetActor->EnableUserInput();

            // Bind to the Cancel/Confirm Delegates (called from local confirm or from repped confirm)
            TargetActor->BindToConfirmCancelInputs();
        }
    }
}

void UGKAbilityTask_WaitForTargetData::RegisterTargetDataCallbacks()
{
    if (!ensure(IsValid(this)))
    {
        GK_WARNING(TEXT("UGKAbilityTask_WaitForTargetData is not valid"));
        return;
    }

    check(Ability);

    const AGameplayAbilityTargetActor *CDO = CastChecked<AGameplayAbilityTargetActor>(
        TargetActor->GetClass()->GetDefaultObject()
    );

    const bool bIsLocallyControlled             = Ability->GetCurrentActorInfo()->IsLocallyControlled();
    const bool bShouldProduceTargetDataOnServer = CDO->ShouldProduceTargetDataOnServer;

    // If not locally controlled (server for remote client), see if TargetData was already sent
    // else register callback for when it does get here.
    if (bIsLocallyControlled)
    {
        return;
    }

    if (bShouldProduceTargetDataOnServer)
    {
        return;
    }

    // We are expecting client to send the data
    // Note: using data from client means you will have to make sure it is valid before using it
    // (cheats)
    // Register with the TargetData callbacks if we are expecting client to send them
    FGameplayAbilitySpecHandle SpecHandle              = GetAbilitySpecHandle();
    FPredictionKey             ActivationPredictionKey = GetActivationPredictionKey();

    // Since multifire is supported, we still need to hook up the callbacks
    AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey)
            .AddUObject(this, &UGKAbilityTask_WaitForTargetData::OnTargetDataReplicatedCallback);

    AbilitySystemComponent->AbilityTargetDataCancelledDelegate(SpecHandle, ActivationPredictionKey)
            .AddUObject(this, &UGKAbilityTask_WaitForTargetData::OnTargetDataReplicatedCancelledCallback);

    AbilitySystemComponent->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);

    // Wait for the client data
    SetWaitingOnRemotePlayerData();
}

/** Valid TargetData was replicated to use (we are server, was sent from client) */
void UGKAbilityTask_WaitForTargetData::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle &Data,
                                                                      FGameplayTag ActivationTag)
{
    check(AbilitySystemComponent);

    FGameplayAbilityTargetDataHandle MutableData = Data;
    AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());

    /**
     *  Call into the TargetActor to sanitize/verify the data. If this returns false, we are rejecting
     *	the replicated target data and will treat this as a cancel.
     *
     *	This can also be used for bandwidth optimizations. OnReplicatedTargetDataReceived could do an actual
     *	trace/check/whatever server side and use that data. So rather than having the client send that data
     *	explicitly, the client is basically just sending a 'confirm' and the server is now going to do the work
     *	in OnReplicatedTargetDataReceived.
     */
    if (TargetActor && !TargetActor->OnReplicatedTargetDataReceived(MutableData))
    {
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            Cancelled.Broadcast(MutableData);
        }
    }
    else
    {
        if (ShouldBroadcastAbilityTaskDelegates())
        {
            ValidData.Broadcast(MutableData);
        }
    }

    if (ConfirmationType != EGameplayTargetingConfirmation::CustomMulti)
    {
        EndTask();
    }
}

/** Client canceled this Targeting Task (we are the server) */
void UGKAbilityTask_WaitForTargetData::OnTargetDataReplicatedCancelledCallback()
{
    check(AbilitySystemComponent);
    if (ShouldBroadcastAbilityTaskDelegates())
    {
        Cancelled.Broadcast(FGameplayAbilityTargetDataHandle());
    }
    EndTask();
}

/** The TargetActor we spawned locally has called back with valid target data */
void UGKAbilityTask_WaitForTargetData::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle &Data)
{
    check(AbilitySystemComponent);
    if (!Ability)
    {
        return;
    }

    FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent, ShouldReplicateDataToServer());

    if (IsPredictingClient())
    {
        if (!TargetActor->ShouldProduceTargetDataOnServer)
        {
            // Fixme: where would this be useful?
            FGameplayTag ApplicationTag;

            // Send Target Data to server
            AbilitySystemComponent->CallServerSetReplicatedTargetData(GetAbilitySpecHandle(),
                                                                      GetActivationPredictionKey(),
                                                                      Data,
                                                                      ApplicationTag,
                                                                      AbilitySystemComponent->ScopedPredictionKey);
        }
        else if (ConfirmationType == EGameplayTargetingConfirmation::UserConfirmed)
        {
            // We aren't going to send the target data, but we will send a generic confirmed message.
            AbilitySystemComponent->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GenericConfirm,
                                                             GetAbilitySpecHandle(),
                                                             GetActivationPredictionKey(),
                                                             AbilitySystemComponent->ScopedPredictionKey);
        }
    }

    // Ability is going to get enabled
    if (ShouldBroadcastAbilityTaskDelegates())
    {
        ValidData.Broadcast(Data);
    }

    if (ConfirmationType != EGameplayTargetingConfirmation::CustomMulti)
    {
        EndTask();
    }
}

/** The TargetActor we spawned locally has called back with a cancel event (they still include the 'last/best'
 * targetdata but the consumer of this may want to discard it) */
void UGKAbilityTask_WaitForTargetData::OnTargetDataCancelledCallback(const FGameplayAbilityTargetDataHandle &Data)
{
    check(AbilitySystemComponent);

    FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent, IsPredictingClient());

    if (IsPredictingClient())
    {
        if (!TargetActor->ShouldProduceTargetDataOnServer)
        {
            AbilitySystemComponent->ServerSetReplicatedTargetDataCancelled(
                    GetAbilitySpecHandle(), GetActivationPredictionKey(), AbilitySystemComponent->ScopedPredictionKey);
        }
        else
        {
            // We aren't going to send the target data, but we will send a generic confirmed message.
            AbilitySystemComponent->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GenericCancel,
                                                             GetAbilitySpecHandle(),
                                                             GetActivationPredictionKey(),
                                                             AbilitySystemComponent->ScopedPredictionKey);
        }
    }
    Cancelled.Broadcast(Data);
    EndTask();
}

/** Called when the ability is asked to confirm from an outside node. What this means depends on the individual task. By
 * default, this does nothing other than ending if bEndTask is true. */
void UGKAbilityTask_WaitForTargetData::ExternalConfirm(bool bEndTask)
{
    check(AbilitySystemComponent);
    if (TargetActor)
    {
        if (TargetActor->ShouldProduceTargetData())
        {
            TargetActor->ConfirmTargetingAndContinue();
        }
    }
    Super::ExternalConfirm(bEndTask);
}

/** Called when the ability is asked to confirm from an outside node. What this means depends on the individual task. By
 * default, this does nothing other than ending if bEndTask is true. */
void UGKAbilityTask_WaitForTargetData::ExternalCancel()
{
    check(AbilitySystemComponent);
    if (ShouldBroadcastAbilityTaskDelegates())
    {
        Cancelled.Broadcast(FGameplayAbilityTargetDataHandle());
    }

    Super::ExternalCancel();
    TargetActor->StopTargeting();
}

bool UGKAbilityTask_WaitForTargetData::ShouldReplicateDataToServer() const
{
    if (!Ability || !TargetActor)
    {
        return false;
    }

    // Send TargetData to the server IFF we are the client and this isn't a GameplayTargetActor that can produce data on
    // the server
    const FGameplayAbilityActorInfo *Info = Ability->GetCurrentActorInfo();
    if (!Info->IsNetAuthority() && !TargetActor->ShouldProduceTargetDataOnServer)
    {
        return true;
    }

    return false;
}

void UGKAbilityTask_WaitForTargetData::OnDestroy(bool AbilityEnded)
{
    TargetActor->StopTargeting();
    Super::OnDestroy(AbilityEnded);
}
// --------------------------------------------------------------------------------------
