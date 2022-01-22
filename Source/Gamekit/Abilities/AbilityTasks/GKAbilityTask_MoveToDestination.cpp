// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.


#include "Gamekit/Abilities/AbilityTasks/GKAbilityTask_MoveToDestination.h"

#include "Gamekit.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"

UGKAbilityTask_MoveToDestination::UGKAbilityTask_MoveToDestination(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bTickingTask   = true;
	bSimulatedTask = true;
	bIsFinished    = false;
    DistanceTolerance = 0.001;
    AngleTolerance = 0.001;
    bDebug         = true;
}


UGKAbilityTask_MoveToDestination* UGKAbilityTask_MoveToDestination::MoveToDestination(
    UGameplayAbility* OwningAbility, 
    FName TaskInstanceName, 
    FVector Destination,
    float DistanceTolerance,
    float AngleTolerance, 
    float TurnRate, 
    float Speed, 
    bool MoveToTarget,
    bool Debug) 
{
	UGKAbilityTask_MoveToDestination* MyObj = NewAbilityTask<UGKAbilityTask_MoveToDestination>(OwningAbility, TaskInstanceName);
    MyObj->Destination = Destination;
    MyObj->Init();
    MyObj->DistanceTolerance = DistanceTolerance;
    MyObj->AngleTolerance    = AngleTolerance;
    MyObj->TurnRate          = TurnRate;
    MyObj->MaxSpeed          = Speed;
    MyObj->bDebug            = Debug;
    MyObj->MoveToTarget      = MoveToTarget;
	return MyObj;
}

void UGKAbilityTask_MoveToDestination::Init() {
    TimeMoveStarted = GetWorld()->GetTimeSeconds();

    AActor *MyActor = GetAvatarActor();
    if (MyActor == nullptr) {
        bIsFinished = true;
        return;
    }

    Character = Cast<APawn>(MyActor);
    if (Character == nullptr) {
        bIsFinished = true;
        return;
    }

    MovementComponent = Cast<UPawnMovementComponent>(Character->GetMovementComponent());
    if (MovementComponent == nullptr) {
        bIsFinished = true;
        return;
    }

    RootComponent = Character->GetRootComponent();

    if (RootComponent == nullptr) {
        bIsFinished = true;
        return;
    }
}


void UGKAbilityTask_MoveToDestination::InitSimulatedTask(UGameplayTasksComponent& InGameplayTasksComponent){
    Super::InitSimulatedTask(InGameplayTasksComponent);
}

 void UGKAbilityTask_MoveToDestination::TickTask(float DeltaTime){
    if (bIsFinished) {
		return;
	}

    auto Location  = RootComponent->GetComponentLocation();
    auto Direction = Destination - Location;
    auto Distance  = Direction.Size2D();

    if (Distance < DistanceTolerance) {
        bIsFinished = true;
        OnCompleted.Broadcast();
        EndTask();
        return;
    }

#if ENABLE_DRAW_DEBUG
    if (bDebug) {
        // Get Forward Vector
        FVector Forward = RootComponent->GetForwardVector();

        DrawDebugLine(
            GetWorld(), 
            Location,                       // Start
            Location + Forward * 5000.f, // End
            FColor::Red,                    // Color
            false,                          // Persistent
            0,                              // Lifetime
            1,                              // Depth
            2.f                             // Tickness
        );
        
        DrawDebugSphere(
            GetWorld(), 
            Destination,    // Center
            16,             // Radius
            10,             // Segements
            FColor::Red, 
            false           // Persistent
        );
    }
#endif // ENABLE_DRAW_DEBUG

    auto TargetRotator = UKismetMathLibrary::FindLookAtRotation(Location, Destination);
    auto CurrentRot    = RootComponent->GetComponentRotation();
    auto TargetYaw     = TargetRotator.Yaw - CurrentRot.Yaw;
    auto MaxTurnStep   = TurnRate * DeltaTime;
    auto TurnStep      = FMath::Clamp(TargetYaw, -MaxTurnStep, MaxTurnStep);
    Character->AddControllerYawInput(TurnStep);

    // Is our direction close enough ?
    if (FMath::Abs(TargetYaw) < AngleTolerance) {
        bRotationFinished = true;

        if (!MoveToTarget) {
            // Rotation is finished
            bIsFinished = true;
            OnCompleted.Broadcast();
            EndTask();

        } else {
            // Starts to move towards the target
            // Maybe we can replace this with some Nav Function to

            auto MaxMoveStep = MaxSpeed * DeltaTime;
            auto MoveStep    = Direction.GetClampedToSize(-MaxMoveStep, MaxMoveStep);

            Character->AddMovementInput(MoveStep, 1.f, false);
        }
    }
 }

void UGKAbilityTask_MoveToDestination::OnDestroy(bool AbilityIsEnding) {
    Super::OnDestroy(AbilityIsEnding);
}

void UGKAbilityTask_MoveToDestination::Activate() {

}

void UGKAbilityTask_MoveToDestination::ExternalCancel() { 
    bIsFinished = true; 
    OnInterrupted.Broadcast();
}

UGKAbilitySystemComponent* UGKAbilityTask_MoveToDestination::GetTargetASC()
{
	return Cast<UGKAbilitySystemComponent>(AbilitySystemComponent);
}
