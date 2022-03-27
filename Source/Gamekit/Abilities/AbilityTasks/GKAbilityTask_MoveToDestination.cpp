// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#include "Gamekit/Abilities/AbilityTasks/GKAbilityTask_MoveToDestination.h"

#include "Gamekit/Abilities/GKGameplayAbility.h"
#include "Gamekit/Blueprint/GKUtilityLibrary.h"
#include "Gamekit/GKLog.h"

#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "Gamekit.h"
#include "Kismet/KismetMathLibrary.h"


UGKAbilityTask_MoveToDestination::UGKAbilityTask_MoveToDestination(const FObjectInitializer &ObjectInitializer):
    Super(ObjectInitializer)
{
    bTickingTask      = true;
    bSimulatedTask    = true;
    bIsFinished       = false;
    DistanceTolerance = 0.001;
    AngleTolerance    = 0.001;
    bDebug            = false;
}


UGKAbilityTask_MoveToDestination *UGKAbilityTask_MoveToDestination::MoveToTarget(
    UGameplayAbility *OwningAbility,
    FName             TaskInstanceName,
    const FGameplayAbilityTargetDataHandle& TargetData,
    float             DistanceTolerance,
    float             AngleTolerance,
    float             TurnRate ,
    float             Speed ,
    bool              MoveToTarget,
    bool              Debug)
{
    UGKAbilityTask_MoveToDestination *MyObj = NewAbilityTask<UGKAbilityTask_MoveToDestination>(
        OwningAbility, 
        TaskInstanceName
    );

    // TODO: Move this to some generic GKAbilityTask
    auto GKAbility = Cast<UGKGameplayAbility>(OwningAbility);
    if (GKAbility)
    {
        GKAbility->CurrentTask = MyObj;   
    }

    MyObj->TargetData        = TargetData; 
    MyObj->DistanceTolerance = DistanceTolerance;
    MyObj->AngleTolerance    = AngleTolerance;
    MyObj->TurnRate          = TurnRate;
    MyObj->MaxSpeed          = Speed;
    MyObj->bDebug            = Debug;
    MyObj->bTurnOnly         = !MoveToTarget;

    MyObj->InitFromTargetData();
    MyObj->Init();

    GK_WARNING(TEXT("Moving to target"));
    return MyObj;
}


void UGKAbilityTask_MoveToDestination::InitFromTargetData() { 
    if (TargetData.IsValid(0)) {
        auto FirstTarget = TargetData.Get(0);
        
        if (FirstTarget->HasHitResult())
        {
            Destination = FirstTarget->GetHitResult()->ImpactPoint;
        }

        auto Actors = FirstTarget->GetActors();
        if (Actors.Num() > 0)
        {
            TargetActor = Actors[0];
        }
    } 
}

UGKAbilityTask_MoveToDestination *UGKAbilityTask_MoveToDestination::MoveToDestination(
    UGameplayAbility *OwningAbility,
    FName   TaskInstanceName,
    FVector Destination,
    float   DistanceTolerance,
    float   AngleTolerance,
    float   TurnRate,
    float   Speed,
    bool    MoveToTarget,
    bool    Debug)
{
    UGKAbilityTask_MoveToDestination *MyObj = NewAbilityTask<UGKAbilityTask_MoveToDestination>(
        OwningAbility, 
        TaskInstanceName
    );

    // TODO: Move this to some generic GKAbilityTask
    auto GKAbility = Cast<UGKGameplayAbility>(OwningAbility);
    if (GKAbility)
    {
        GKAbility->CurrentTask = MyObj;   
    }

    MyObj->Destination = Destination;
    MyObj->DistanceTolerance = DistanceTolerance;
    MyObj->AngleTolerance    = AngleTolerance;
    MyObj->TurnRate          = TurnRate;
    MyObj->MaxSpeed          = Speed;
    MyObj->bDebug            = Debug;
    MyObj->bTurnOnly         = !MoveToTarget;

    MyObj->Init();
    return MyObj;
}

void UGKAbilityTask_MoveToDestination::Init()
{
    AActor *MyActor = GetAvatarActor();
    if (MyActor == nullptr)
    {
        bIsFinished = true;
        return;
    }

    Character = Cast<APawn>(MyActor);
    if (Character == nullptr)
    {
        bIsFinished = true;
        return;
    }

    MovementComponent = Cast<UPawnMovementComponent>(Character->GetMovementComponent());
    if (MovementComponent == nullptr)
    {
        bIsFinished = true;
        return;
    }

    RootComponent = Character->GetRootComponent();

    if (RootComponent == nullptr)
    {
        bIsFinished = true;
        return;
    }

    Ability->OnGameplayAbilityCancelled.Add(
        FOnGameplayAbilityCancelled::FDelegate::CreateUObject(this, &UGKAbilityTask_MoveToDestination::ExternalCancel)
    );
}

void UGKAbilityTask_MoveToDestination::InitSimulatedTask(UGameplayTasksComponent &InGameReplayTasksComponent)
{
    Init();
    if (TargetData.IsValid(0))
    {
        InitFromTargetData();
    }
    Super::InitSimulatedTask(InGameReplayTasksComponent);
}

void UGKAbilityTask_MoveToDestination::DebugDraw()
{
#if ENABLE_DRAW_DEBUG
    if (bDebug && RootComponent)
    {
        // Get Forward Vector
        FVector Forward  = RootComponent->GetForwardVector();
        FVector Location = RootComponent->GetComponentLocation();

        DrawDebugLine(GetWorld(),
                      Location,                    // Start
                      Location + Forward * 5000.f, // End
                      FColor::Red,                 // Color
                      false,                       // Persistent
                      0,                           // Lifetime
                      1,                           // Depth
                      2.f                          // Tickness
        );

        DrawDebugSphere(GetWorld(),
                        Destination, // Center
                        16,          // Radius
                        10,          // Segements
                        FColor::Red,
                        false // Persistent
        );
    }
#endif // ENABLE_DRAW_DEBUG
}


/* void UGKAbilityTask_MoveToDestination::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    DOREPLIFETIME(UGKAbilityTask_MoveToDestination, Destination);
}
// */

void UGKAbilityTask_MoveToDestination::TickTask(float DeltaTime)
{
    GK_WARNING(TEXT("TickTask"));

    if (bIsFinished)
    {
        EndTask();
        return;
    }

    Super::TickTask(DeltaTime);

    // If our target is an actor update the target location to his
    if (TargetActor.IsValid())
    {
        Destination = TargetActor->GetActorLocation();
    }

    DebugDraw();

    // If the movement input was not consumed yet; we cannot run
    // to avoid overshooting our target
    // NB: no need to use NearlyZero here because the pending
    // vector will be exactly Zero after input consumption
    if (!Character->GetPendingMovementInputVector().IsZero())
    {
        UE_LOG(LogGamekit, Log, TEXT("Movement Input was not consumed yet"));
        return;
    }

    auto Location  = RootComponent->GetComponentLocation();
    auto Direction = Destination - Location;
    auto Distance  = Direction.Size2D();

    if (bRotationFinished && Distance < DistanceTolerance)
    {
        GK_WARNING(TEXT("Reached destination"));
        bIsFinished = true;
        OnCompleted.Broadcast(TargetData);
        EndTask();
        return;
    }

    // Look at method
    // ---------------
    // auto TargetRotator = UGKUtilityLibrary::BetterLookAtRotation(Location, Destination);
    auto TargetRotator = UKismetMathLibrary::FindLookAtRotation(Location, Destination);
    auto CurrentRot    = RootComponent->GetComponentRotation();
    auto TargetYaw     = (TargetRotator.Yaw - CurrentRot.Yaw);

    // This is the most reliable way of getting the smallest angle
    auto Before = TargetYaw;

    // I think there is a bug when the angle is close to 180
    if (FMath::Abs(TargetYaw) > 180)
    {
        TargetYaw = FMath::RadiansToDegrees(FMath::Asin(FMath::Sin(FMath::DegreesToRadians(TargetYaw))));
    }
    // -----------------

    // Limit turn speed
    auto MaxTurnStep = TurnRate * DeltaTime;
    auto TurnStep    = FMath::Clamp(TargetYaw, -MaxTurnStep, MaxTurnStep);

    // We continue tweaking the Yaw throughout the movement
    // that might not be a good idea for some edge cases
    Character->AddControllerYawInput(TurnStep);

    GK_WARNING(TEXT("Turning toward target %f"), TurnStep);

    // Is our direction close enough ?
    if (FMath::Abs(TargetYaw) < AngleTolerance)
    {
        if (!bRotationFinished)
        {
            OnTurnDone.Broadcast(TargetData);
            GK_WARNING(TEXT("Finished turning"));
        }
        bRotationFinished = true;


        if (bTurnOnly)
        {
            // Rotation is finished
            bIsFinished = true;
            OnCompleted.Broadcast(TargetData);
            EndTask();
        }
        else
        {
            // Starts to move towards the target
            // Maybe we can replace this with some Nav Function
            // if so we might have to stop our turn logic afterwards
            // so they dont conflicts
            auto MaxMoveStep = MaxSpeed * DeltaTime;
            auto MoveStep    = Direction.GetClampedToSize(-MaxMoveStep, MaxMoveStep);

            GK_WARNING(TEXT("Moving toward target %s"), *MoveStep.ToString());
            Character->AddMovementInput(MoveStep, 1.f, false);
        }
    } 
}

void UGKAbilityTask_MoveToDestination::OnDestroy(bool AbilityIsEnding) { 
    Super::OnDestroy(AbilityIsEnding); 

    if (!IsSimulatedTask())
    {
        Ability->OnGameplayAbilityCancelled.RemoveAll(this);

        // TODO: Move this to some generic GKAbilityTask
        auto GKAbility = Cast<UGKGameplayAbility>(Ability);
        if (GKAbility)
        {
            GKAbility->CurrentTask = nullptr;   
        }
    }
}

void UGKAbilityTask_MoveToDestination::Activate() {}

void UGKAbilityTask_MoveToDestination::ExternalCancel()
{
    bIsFinished = true;
    OnCancelled.Broadcast(TargetData);
    EndTask();
}

UGKAbilitySystemComponent *UGKAbilityTask_MoveToDestination::GetTargetASC()
{
    return Cast<UGKAbilitySystemComponent>(AbilitySystemComponent);
}
