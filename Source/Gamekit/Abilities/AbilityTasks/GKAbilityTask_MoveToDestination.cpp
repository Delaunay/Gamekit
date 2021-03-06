// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Abilities/AbilityTasks/GKAbilityTask_MoveToDestination.h"

// Gamekit
#include "Gamekit/Abilities/GKGameplayAbility.h"
#include "Gamekit/Blueprint/GKUtilityLibrary.h"
#include "Gamekit/GKLog.h"
#include "Gamekit/Gamekit.h"
#include "Gamekit/Abilities/GKAbilitySystemGlobals.h"
#include "Gamekit/Blueprint/GKNavigationInterface.h"
#include "Gamekit/Blueprint/GKUtilityLibrary.h"

// Unreal Engine
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// #define DEBUG_MOVEMENT

#ifdef DEBUG_MOVEMENT
#    define DEBUG_MVT(...) GK_WARNING(__VA_ARGS__)
#else
#    define DEBUG_MVT(...)
#endif

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

void UGKAbilityTask_MoveToDestination::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UGKAbilityTask_MoveToDestination, Destination);
}

UGKAbilityTask_MoveToDestination *UGKAbilityTask_MoveToDestination::MoveToTarget(
        UGameplayAbility *                      OwningAbility,
        FName                                   TaskInstanceName,
        const FGameplayAbilityTargetDataHandle &TargetData,
        float                                   DistanceTolerance,
        float                                   AngleTolerance,
        float                                   TurnRate,
        float                                   Speed,
        bool                                    MoveToTarget,
        bool                                    bUseMovementComponent,
        bool                                    bFollowActor,
        FGameplayTagContainer                   CancelTag,
        bool                                    bUsePathfinding,
        bool                                    Debug)
{
    UGKAbilityTask_MoveToDestination *MyObj =
            NewAbilityTask<UGKAbilityTask_MoveToDestination>(OwningAbility, TaskInstanceName);

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
    MyObj->bFollowActor      = bFollowActor;
    MyObj->CancelTag         = CancelTag;
    MyObj->bUsePathfinding = bUsePathfinding;

    if (bUseMovementComponent)
    {
        auto Avatar   = OwningAbility->GetAvatarActorFromActorInfo();
        auto Movement = Cast<UCharacterMovementComponent>(
                Avatar->GetComponentByClass(UCharacterMovementComponent::StaticClass()));

        if (Movement)
        {
            MyObj->TurnRate = Movement->RotationRate.Yaw;
            MyObj->MaxSpeed = Movement->MaxWalkSpeed;
        }
    }

    MyObj->InitFromTargetData();
    MyObj->Init();
    return MyObj;
}

void UGKAbilityTask_MoveToDestination::InitFromTargetData()
{
    if (TargetData.IsValid(0))
    {
        auto FirstTarget = TargetData.Get(0);

        if (!bFollowActor)
        {
            if (FirstTarget->HasHitResult())
            {
                Destination = FirstTarget->GetHitResult()->ImpactPoint;
                return;
            }
        }

        auto Actors = FirstTarget->GetActors();
        if (Actors.Num() > 0)
        {
            auto Actor = Actors[0];

            if (TargetActor.IsValid())
            {
                Destination = Actor->GetActorLocation();
            }

            if (bFollowActor)
            {
                TargetActor = Actors[0];
            } else {
                Destination = Actors[0]->GetActorLocation();
            }
        }
    }
    else
    {
        GK_WARNING(TEXT("No valid target!"));
    }
}

UGKAbilityTask_MoveToDestination *UGKAbilityTask_MoveToDestination::MoveToDestination(UGameplayAbility *OwningAbility,
                                                                                      FName   TaskInstanceName,
                                                                                      FVector Destination,
                                                                                      float   DistanceTolerance,
                                                                                      float   AngleTolerance,
                                                                                      float   TurnRate,
                                                                                      float   Speed,
                                                                                      bool    MoveToTarget,
                                                                                      bool    bFollowActor,
                                                                                      FGameplayTagContainer CancelTag,
                                                                                      bool                  bUsePathfinding,
                                                                                      bool                Debug)
{
    UGKAbilityTask_MoveToDestination *MyObj =
            NewAbilityTask<UGKAbilityTask_MoveToDestination>(OwningAbility, TaskInstanceName);

    // TODO: Move this to some generic GKAbilityTask
    auto GKAbility = Cast<UGKGameplayAbility>(OwningAbility);
    if (GKAbility)
    {
        GKAbility->CurrentTask = MyObj;
    }

    MyObj->Destination       = Destination;
    MyObj->DistanceTolerance = DistanceTolerance;
    MyObj->AngleTolerance    = AngleTolerance;
    MyObj->TurnRate          = TurnRate;
    MyObj->MaxSpeed          = Speed;
    MyObj->bDebug            = Debug;
    MyObj->bTurnOnly         = !MoveToTarget;
    MyObj->bFollowActor = bFollowActor;
    MyObj->CancelTag = CancelTag;
    MyObj->bUsePathfinding = bUsePathfinding;

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

   if (CancelTag.IsValid()){
        auto Delegate = FOnGameplayEffectTagCountChanged::FDelegate::CreateUObject(
            this,
            &UGKAbilityTask_MoveToDestination::OnReceiveGameplayTag
        );

        DelegateHandle = Delegate.GetHandle();

        AbilitySystemComponent->RegisterGenericGameplayTagEvent().Add(Delegate);
   }


   if (bUsePathfinding) {
       FindPath();
   }

    /* This is not needed, it is already handled
     *
    Ability->OnGameplayAbilityCancelled.Add(FOnGameplayAbilityCancelled::FDelegate::CreateUObject(
            this, &UGKAbilityTask_MoveToDestination::ExternalCancel));
    */
}

void UGKAbilityTask_MoveToDestination::OnReceiveGameplayTag(const FGameplayTag Tag, int32 Count) 
{
    if (CancelTag.HasTag(Tag)){
        // Ability cancel is going to cancel this task as well
        Ability->K2_CancelAbility();
        AbilitySystemComponent->RegisterGenericGameplayTagEvent().Remove(DelegateHandle);
    }
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

    if (bUsePathfinding && Path.IsValid()){
        DrawPath(Path.Get());
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
    DEBUG_MVT(TEXT("TickTask"));

    if (bIsFinished)
    {
        EndTask();
        return;
    }

    Super::TickTask(DeltaTime);

    DebugDraw();

    // If the movement input was not consumed yet; we cannot run
    // to avoid overshooting our target
    // NB: no need to use NearlyZero here because the pending
    // vector will be exactly Zero after input consumption
    if (!Character->GetPendingMovementInputVector().IsZero())
    {
        GK_WARNING(TEXT("Movement Input was not consumed yet"));
        return;
    }

    FVector   Goal = GetCurrentGoal();
    auto Location  = RootComponent->GetComponentLocation();
    auto Direction = Goal - Location;
    auto Distance  = Direction.Size2D();

    if (bRotationFinished && Distance < DistanceTolerance)
    {
        DEBUG_MVT(TEXT("Reached current goal"));

        if (CurrentSegment == EndSegment){
            DEBUG_MVT(TEXT("End of Path"));

            bIsFinished = true;
            OnCompleted.Broadcast(TargetData);
            EndTask();
            return;
        } 
        else 
        {
            DEBUG_MVT(TEXT("Go to next goal"));
            // Got a new goal
            NextSegment();
            Goal = GetCurrentGoal();
            bRotationFinished = false;
        }
    }

    // Look at method
    // ---------------
    // auto TargetRotator = UGKUtilityLibrary::BetterLookAtRotation(Location, Destination);
    auto TargetRotator = UKismetMathLibrary::FindLookAtRotation(Location, Goal);

    auto CurrentRot    = RootComponent->GetComponentRotation();
    auto TargetYaw     = TargetRotator.Yaw - CurrentRot.Yaw;
    auto Before        = TargetYaw;

    // was positive, becomes negative, i.e we will turn the other way
    if (TargetYaw > 180) 
    {
        TargetYaw = TargetYaw - 360;
    }

    if (TargetYaw < -180)
    {
        TargetYaw = TargetYaw + 360;
    }

    // Limit turn speed
    auto MaxTurnStep = TurnRate * DeltaTime;
    auto TurnStep    = FMath::Clamp(TargetYaw, -MaxTurnStep, MaxTurnStep);

    // We continue tweaking the Yaw throughout the movement
    // that might not be a good idea for some edge cases
    Character->AddControllerYawInput(TurnStep);
    DEBUG_MVT(TEXT("Turning toward target %f (%f, %f)"), TurnStep, Before, TargetYaw);

    // Is our direction close enough ?
    if (FMath::Abs(TargetYaw) < AngleTolerance)
    {
        if (!bRotationFinished)
        {
            OnTurnDone.Broadcast(TargetData);
            DEBUG_MVT(TEXT("Finished turning"));
        }
        bRotationFinished = true;

        // if rooted we cannot move
        bool bRooted = AbilitySystemComponent->HasMatchingGameplayTag(DisableRoot);

        if (bTurnOnly || bRooted)
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

            DEBUG_MVT(TEXT("Moving toward target %s"), *MoveStep.ToString());
            Character->AddMovementInput(MoveStep, 1.f, false);
        }
    }
}

void UGKAbilityTask_MoveToDestination::OnDestroy(bool AbilityIsEnding)
{
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
    Super::ExternalCancel();

    bIsFinished = true;
    OnCancelled.Broadcast(TargetData);
    EndTask();
}

UGKAbilitySystemComponent *UGKAbilityTask_MoveToDestination::GetTargetASC()
{
    return Cast<UGKAbilitySystemComponent>(AbilitySystemComponent);
}


void UGKAbilityTask_MoveToDestination::FindPath() {
    Path.Reset();

    FGKNavQueryResult Result = UGKNavigationInterface::FindPath(
        GetWorld(),
        Ability->GetAvatarActorFromActorInfo(),
        Destination,
        TargetActor.Get(),
        UNavigationQueryFilter::StaticClass(),
        true,
        TargetActor != nullptr,
        true,
        true,
        0.0,
        true,
        false,
        bDebug
    );

    if (Result.NavPath.IsValid() && Result.NavPath->IsValid()) {
        Path = Result.NavPath;
        Path->AddObserver(FNavigationPath::FPathObserverDelegate::FDelegate::CreateUObject(this, &UGKAbilityTask_MoveToDestination::OnPathEvent));

        CurrentSegment = 0;
        Start = Path->GetPathPoints()[CurrentSegment].NodeRef;
        End   = Path->GetPathPoints()[CurrentSegment + 1].NodeRef;
        EndSegment = Path->GetPathPoints().Num() - 2;

        GK_DISPLAY(TEXT("Found path with %d points %s"), EndSegment + 2, *UGKUtilityLibrary::GetNetConfig(Ability->GetAvatarActorFromActorInfo()));
    } else {
        GK_WARNING(TEXT("No path was found to destination"));
    }
}

void UGKAbilityTask_MoveToDestination::NextSegment() {
    CurrentSegment += 1;
    Start = Path->GetPathPoints()[CurrentSegment].NodeRef;
    End = Path->GetPathPoints()[CurrentSegment + 1].NodeRef;
}

FVector UGKAbilityTask_MoveToDestination::GetCurrentGoal() {
    if (bUsePathfinding && Path.IsValid() && Path->IsValid()) {
        TArray<FNavPathPoint>& Points = Path->GetPathPoints();
        return Path->GetPathPoints()[CurrentSegment + 1].Location;
    }
    else if (TargetActor.IsValid()) 
    {
        return TargetActor->GetActorLocation();
    } 
    else 
    {
        return Destination;
    }
}

void UGKAbilityTask_MoveToDestination::OnPathEvent(FNavigationPath* InPath, ENavPathEvent::Type Event)
{
    const static UEnum* NavPathEventEnum = StaticEnum<ENavPathEvent::Type>();
    GK_LOG(TEXT("Received Event: %s"), *NavPathEventEnum->GetNameStringByValue(Event));


    if (InPath && Path.Get() == InPath) {
        switch (Event)
        {
        // Reset the segment
        case ENavPathEvent::NewPath: 
        case ENavPathEvent::UpdatedDueToGoalMoved:
        case ENavPathEvent::UpdatedDueToNavigationChanged:
        {
            // The path is not updated fully so some old points can still be there
            // we need to find the new CurrentSegment that is our previous goal
            //
            // but sometimes we endup with less points
            // so just start from the beginning, if we cannot resume
            CurrentSegment = 0;
            EndSegment = Path->GetPathPoints().Num() - 2;

            // Try to resume where we used tobe if we have Node reference
            if (Start != INVALID_NAVNODEREF && End != INVALID_NAVNODEREF && InPath->GetNavigationDataUsed() != nullptr){
                for (int32 PathPoint = 0; PathPoint < InPath->GetPathPoints().Num() - 1; ++PathPoint)
                {
                    if (InPath->GetPathPoints()[PathPoint].NodeRef == Start && InPath->GetPathPoints()[PathPoint + 1].NodeRef == End)
                    {
                        CurrentSegment = PathPoint;
                        GK_LOG(TEXT("Resumed path following"));
                        break;
                    }
                }
            }
        }
        break;

        // Ignore this
        case ENavPathEvent::MetaPathUpdate:
            break;

        // Stop movement
        case ENavPathEvent::Cleared:
        case ENavPathEvent::Invalidated:
        case ENavPathEvent::RePathFailed:
        default: {
            ExternalCancel();
        }
            break;
        }
    }
}

void UGKAbilityTask_MoveToDestination::DrawPath(FNavigationPath* InPath) {
    if (bDebug) {
        TArray<FNavPathPoint> Points = InPath->GetPathPoints();
        FVector LineStart = Points[0].Location;

        for (int i = 1; i < Points.Num(); i++) {
            auto& Point = Points[i];

            UKismetSystemLibrary::DrawDebugLine(
                GetWorld(),
                LineStart,
                Point.Location,
                FLinearColor::Red,
                0.f,
                1.f
            );
            LineStart = Point.Location;
        }
    }
}
