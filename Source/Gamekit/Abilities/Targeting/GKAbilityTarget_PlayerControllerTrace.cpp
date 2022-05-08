// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Abilities/Targeting/GKAbilityTarget_PlayerControllerTrace.h"

// Gamekit
#include "Gamekit/Abilities/GKAbilityStatic.h"
#include "Gamekit/Characters/GKSelectableInterface.h"
#include "Gamekit/GKLog.h"

// Unreal Engine
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Components/DecalComponent.h"
#include "DrawDebugHelpers.h"
#include "GenericTeamAgentInterface.h"
#include "Kismet/KismetSystemLibrary.h"

AGKAbilityTarget_PlayerControllerTrace::AGKAbilityTarget_PlayerControllerTrace(
        const FObjectInitializer &ObjectInitializer):
    Super(ObjectInitializer)
{
    // NOTE: Gamekit is deprecating this
    bDestroyOnConfirmation = false;
    // ---------------------------------

    bTickEnabled                  = true;
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickGroup    = TG_PostUpdateWork;
    IsInputBound                  = false;
}

void AGKAbilityTarget_PlayerControllerTrace::InitializeFromAbilityData(FGKAbilityStatic const &AbilityData)
{
    MaxRange           = AbilityData.CastMaxRange;
    MinRange           = AbilityData.CastMinRange;
    AreaOfEffect       = AbilityData.AreaOfEffect;
    ObjectTypes        = AbilityData.TargetObjectTypes;
    ClassFilter        = AbilityData.TargetFilterClass;
    TargetActorFaction = AbilityData.TargetActorFaction;
    TargetMode         = AbilityData.AbilityBehavior;

    // Blueprint overrides
    Super::InitializeFromAbilityData(AbilityData);
}

void AGKAbilityTarget_PlayerControllerTrace::StartTargeting(UGKGameplayAbility *Ability)
{
    Super::StartTargeting(Ability);

    if (ensure(OwningAbility)){
        SourceActor = OwningAbility->GetActorInfo().AvatarActor.Get();

        // Init the targeting before the tick
        TargetValidityChanged.Broadcast(LatestHitResult, bIsTargetValid);
    }
}

void AGKAbilityTarget_PlayerControllerTrace::Deselect()
{
    for (auto &Actor: ActorsUnderCursor)
    {
        auto Selectable = Cast<IGKSelectableInterface>(Actor);
        if (Selectable)
        {
            Selectable->Deselect();
        }
    }
}

void AGKAbilityTarget_PlayerControllerTrace::Select()
{
    for (auto &Actor: ActorsUnderCursor)
    {
        auto Selectable = Cast<IGKSelectableInterface>(Actor);
        if (Selectable)
        {
            Selectable->Select();
        }
    }
}

void AGKAbilityTarget_PlayerControllerTrace::DebugDraw()
{
#if ENABLE_DRAW_DEBUG
    if (bDebug)
    {
        auto Color = IsConfirmTargetingAllowed() ? FColor::Green : FColor::Red;
        DrawDebugLine(GetWorld(), SourceActor->GetActorLocation(), TraceEndPoint, Color, false);
        DrawDebugSphere(GetWorld(), TraceEndPoint, 16, 10, Color, false);
        DrawDebugCircle(GetWorld(),
                        SourceActor->GetActorLocation() + FVector(0.f, 0.f, 1.f),
                        MaxRange,
                        32,
                        FColor::Green,
                        false,
                        -1.f,
                        0,
                        1,
                        FVector(0.f, 1.f, 0.f),
                        FVector(1.f, 0.f, 0.f));
    }
#endif // ENABLE_DRAW_DEBUG
}

void AGKAbilityTarget_PlayerControllerTrace::Tick(float DeltaSeconds)
{
    // Server and launching client only
    if (!OwningAbility)
    {
        GK_WARNING(TEXT("Owning ability is null"));
        CancelTargeting();
        return;
    }

    APlayerController *PC = OwningAbility->GetCurrentActorInfo()->PlayerController.Get();
    check(PC);

    PC->GetHitResultUnderCursorForObjects(ObjectTypes, false, LatestHitResult);

    TraceEndPoint = LatestHitResult.Component.IsValid() ? LatestHitResult.ImpactPoint : LatestHitResult.TraceEnd;

    if (TargetMode == EGK_AbilityBehavior::ActorTarget)
    {
        Deselect();

        UKismetSystemLibrary::SphereOverlapActors(
            GetWorld(), 
            TraceEndPoint, 
            AreaOfEffect, 
            ObjectTypes, 
            ClassFilter, 
            ActorsToIgnore, 
            ActorsUnderCursor
        );

        FilterActors();

        Select();
    }

    auto valid = IsConfirmTargetingAllowed();

    if (valid)
    {
        LatestValidHitResult = LatestHitResult;
    }

    DebugDraw();

    SetActorLocationAndRotation(TraceEndPoint, SourceActor->GetActorRotation());
}

bool AGKAbilityTarget_PlayerControllerTrace::IsTargetValid() const
{
    if (!SourceActor)
    {
        return false;
    }

    if (TargetMode == EGK_AbilityBehavior::ActorTarget)
    {
        return ActorsUnderCursor.Num() > 0;
    }

    if (TargetMode == EGK_AbilityBehavior::PointTarget)
    {
        float DistanceSqr = FVector::DistSquared(SourceActor->GetActorLocation(), TraceEndPoint);
        return MaxRange * MaxRange >= DistanceSqr && DistanceSqr >= MinRange * MinRange;
    }

    return false;
}

bool AGKAbilityTarget_PlayerControllerTrace::IsConfirmTargetingAllowed()
{

    bool newValidity = IsTargetValid();

    if (newValidity != bIsTargetValid)
    {
        TargetValidityChanged.Broadcast(LatestHitResult, newValidity);
        bIsTargetValid = newValidity;
    }

    return newValidity;
}

void AGKAbilityTarget_PlayerControllerTrace::CancelTargeting()
{
    CanceledDelegate.Broadcast(FGameplayAbilityTargetDataHandle());
    Deselect();
}

//! Select the current target as our final target
void AGKAbilityTarget_PlayerControllerTrace::ConfirmTargeting()
{
    if (!IsConfirmTargetingAllowed())
    {
        // Everytime confirmed input is pressed the callbacks are cleared.
        // if we have not been able to produce a valid target now we need to re-register our inputs
        if (!GenericDelegateBoundASC)
        {
            return;
        }

        AbilitySystemComponent->GenericLocalConfirmCallbacks.AddDynamic(
                this, &AGKAbilityTarget_PlayerControllerTrace::ConfirmTargeting);
        return;
    }

    ConfirmTargetingAndContinue();
}

void AGKAbilityTarget_PlayerControllerTrace::ConfirmTargetingAndContinue()
{
    check(ShouldProduceTargetData());

    if (!IsConfirmTargetingAllowed())
    {
        return;
    }

    if (TargetMode == EGK_AbilityBehavior::ActorTarget)
    {
        TArray<TWeakObjectPtr<AActor>> Actors;
        Actors.Reset(ActorsUnderCursor.Num());

        for (auto &actor: ActorsUnderCursor)
        {
            Actors.Add(actor);
        }

        // Target is ready, send the data now
        auto Handle = StartLocation.MakeTargetDataHandleFromActors(Actors, false);
        TargetDataReadyDelegate.Broadcast(Handle);
        return;
    }

    if (TargetMode == EGK_AbilityBehavior::PointTarget)
    {
        // Target is ready, send the data now
        auto Handle = StartLocation.MakeTargetDataHandleFromHitResult(OwningAbility, LatestHitResult);
        TargetDataReadyDelegate.Broadcast(Handle);
        return;
    }

    Deselect();
}

void AGKAbilityTarget_PlayerControllerTrace::FilterActors()
{
    TArray<AActor *> Actors;
    Actors.Reset(ActorsUnderCursor.Num());

    APlayerController *         PC = OwningAbility->GetCurrentActorInfo()->PlayerController.Get();
    IGenericTeamAgentInterface *Me = nullptr;

    {
        AActor *OwnerActor  = OwningAbility->GetCurrentActorInfo()->OwnerActor.Get();
        AActor *AvatarActor = OwningAbility->GetCurrentActorInfo()->AvatarActor.Get();

        Me = Cast<IGenericTeamAgentInterface>(OwnerActor);
        if (Me == nullptr)
        {
            Me = Cast<IGenericTeamAgentInterface>(AvatarActor);
        }
    }

    if (Me == nullptr)
    {
        ABILITY_LOG(Warning,
                    TEXT("AGKAbilityTarget_PlayerControllerTrace::FilterActors, Owner does not implement "
                         "TeamAgentInterface"));
        return;
    }

    for (auto &Actor: ActorsUnderCursor)
    {
        // Create the bit flag we will check against
        auto TeamAttitude = SetFlag(0, Me->GetTeamAttitudeTowards(*Actor));

        if (TeamAttitude & uint32(TargetActorFaction))
        {
            Actors.Add(Actor);
        }
    }

    ActorsUnderCursor = Actors;
}

void AGKAbilityTarget_PlayerControllerTrace::BindToConfirmCancelInputs()
{
    check(OwningAbility);

    if (!AbilitySystemComponent)
    {
        ABILITY_LOG(Warning,
                    TEXT("AGameplayAbilityTargetActor::BindToConfirmCancelInputs called with null ASC! Actor %s"),
                    *GetName())
        return;
    }

    const FGameplayAbilityActorInfo *Info = (OwningAbility ? OwningAbility->GetCurrentActorInfo() : nullptr);

    // Client
    if (Info && Info->IsLocallyControlled())
    {
        // We have to wait for the callback from the AbilitySystemComponent. Which will always be instigated locally
        AbilitySystemComponent->GenericLocalConfirmCallbacks.AddDynamic(
                this, &AGameplayAbilityTargetActor::ConfirmTargeting); // Tell me if the confirm input is pressed
        AbilitySystemComponent->GenericLocalCancelCallbacks.AddDynamic(
                this, &AGameplayAbilityTargetActor::CancelTargeting); // Tell me if the cancel input is pressed

        // Save off which ASC we bound so that we can error check that we're removing them later
        GenericDelegateBoundASC = AbilitySystemComponent;
        IsInputBound            = true;
        return;
    }

    // Server
    FGameplayAbilitySpecHandle Handle  = OwningAbility->GetCurrentAbilitySpecHandle();
    FPredictionKey             PredKey = OwningAbility->GetCurrentActivationInfo().GetActivationPredictionKey();

    // Setup replication for Confirm & Cancel
    GenericConfirmHandle =
            AbilitySystemComponent
                    ->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GenericConfirm, Handle, PredKey)
                    .AddUObject(this, &AGameplayAbilityTargetActor::ConfirmTargeting);

    GenericCancelHandle =
            AbilitySystemComponent
                    ->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GenericCancel, Handle, PredKey)
                    .AddUObject(this, &AGameplayAbilityTargetActor::CancelTargeting);

    IsInputBound = true;

    // if the input was already sent force replication now
    // Calls a given Generic Replicated Event delegate if the event has already been sent
    if (AbilitySystemComponent->CallReplicatedEventDelegateIfSet(
                EAbilityGenericReplicatedEvent::GenericConfirm, Handle, PredKey))
    {
        return;
    }

    if (AbilitySystemComponent->CallReplicatedEventDelegateIfSet(
                EAbilityGenericReplicatedEvent::GenericCancel, Handle, PredKey))
    {
        return;
    }
}

void AGKAbilityTarget_PlayerControllerTrace::StopTargeting()
{
    Super::StopTargeting();

    if (!IsInputBound)
    {
        return;
    }

    if (!AbilitySystemComponent)
    {
        ABILITY_LOG(Warning,
                    TEXT("AGameplayAbilityTargetActor::CleanupBindings called with null ASC! Actor %s"),
                    *GetName());
        return;
    }

    const FGameplayAbilityActorInfo *Info = (OwningAbility ? OwningAbility->GetCurrentActorInfo() : nullptr);
    Deselect();

    // i.e Locally Controlled
    if (GenericDelegateBoundASC && Info && Info->IsLocallyControlled())
    {
        // We must remove ourselves from GenericLocalConfirmCallbacks/GenericLocalCancelCallbacks,
        // since while these are bound they will inhibit any *other* abilities
        // that are bound to the same key.
        AbilitySystemComponent->GenericLocalConfirmCallbacks.RemoveDynamic(
                this, &AGameplayAbilityTargetActor::ConfirmTargeting);
        AbilitySystemComponent->GenericLocalCancelCallbacks.RemoveDynamic(
                this, &AGameplayAbilityTargetActor::CancelTargeting);

        // Error checking that we have removed delegates from the same ASC we bound them to
        // FIXME: how could this ever be !=
        ensure(GenericDelegateBoundASC == Info->AbilitySystemComponent.Get());
        return;
    }

    // Server
    // Remove Replicated events
    // Remove Confirm event
    AbilitySystemComponent
            ->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GenericConfirm,
                                             OwningAbility->GetCurrentAbilitySpecHandle(),
                                             OwningAbility->GetCurrentActivationInfo().GetActivationPredictionKey())
            .Remove(GenericConfirmHandle);

    // Remove Cancel event
    AbilitySystemComponent
            ->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GenericCancel,
                                             OwningAbility->GetCurrentAbilitySpecHandle(),
                                             OwningAbility->GetCurrentActivationInfo().GetActivationPredictionKey())
            .Remove(GenericCancelHandle);
}
