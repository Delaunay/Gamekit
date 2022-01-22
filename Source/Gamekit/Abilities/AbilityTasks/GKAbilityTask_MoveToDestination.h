// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "CoreMinimal.h"
#include "GKAbilityTask_MoveToDestination.generated.h"

/** Delegate type used, EventTag and Payload may be empty if it came from the montage callbacks */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGKMoveToDestinationDelegate);

/**
 * This is different from the Gameplay Ability MoveToLocation built-in
 *
 * 	1. This will move the player to location without a timer
 *	2. This will turn the player to the movement direction before moving
 * 
 * AddControllerYawInput && AddMovementInput are used to turn the character
 * mimicking what a player would have to do to turn it.
 * Hopefully making it respect the constraitn imposed by the MovementComponent.
 * 
 * Parameters
 * ----------
 * 
 * Destination: FVector
 *  Target location to reach
 * 
 * DistanceTolerance: Float
 *  How close to the location should we be to stop (avoid too small values)
 * 
 * AngleTolerance: Float
 *  Angle Jitter we are willing to accept (avoid too small values)
 * 
 * TurnRate: Float
 *  Turn speed
 * 
 * Speed: Float
 *  Move speed
 * 
 * MoveToTarget: bool
 *  If true will start moving towards the target after rotating to face it.
 *  if false will only rotate to face the target
 * 
 * Debug: bool
 *  if true will debug draw, destination and direction vector
 * 
 */
UCLASS()
class GAMEKIT_API UGKAbilityTask_MoveToDestination: public UAbilityTask
{
    GENERATED_BODY()

    UGKAbilityTask_MoveToDestination(const FObjectInitializer &ObjectInitializer);

    UFUNCTION(BlueprintCallable,
              Category = "Ability|Tasks",
              meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
    static UGKAbilityTask_MoveToDestination *MoveToDestination(UGameplayAbility *OwningAbility,
                                                               FName             TaskInstanceName,
                                                               FVector           Destination,
                                                               float             DistanceTolerance,
                                                               float             AngleTolerance,
                                                               float             TurnRate,
                                                               float             Speed,
                                                               bool              MoveToTarget,
                                                               bool              Debug);

    /** Reached Destination */
    UPROPERTY(BlueprintAssignable)
    FGKMoveToDestinationDelegate OnCompleted;

    /** The movement was stopped by external forces */
    UPROPERTY(BlueprintAssignable)
    FGKMoveToDestinationDelegate OnInterrupted;

    /** The movement action was cancelled by the user */
    UPROPERTY(BlueprintAssignable)
    FGKMoveToDestinationDelegate OnCancelled;

    virtual void InitSimulatedTask(UGameplayTasksComponent &InGameplayTasksComponent) override;

    virtual void TickTask(float DeltaTime) override;

    virtual void OnDestroy(bool AbilityIsEnding) override;

    virtual void Activate() override;

    virtual void ExternalCancel() override;

    void Init();

    private:
    // Arguments
    float   TurnRate;
    float   MaxSpeed;
    FVector Destination;
    bool    bDebug;
    float   DistanceTolerance;
    float   AngleTolerance;
    bool    MoveToTarget;

    // States
    bool  bIsFinished;
    bool  bRotationFinished;

    // Cached Variables
    class APawn                     *Character;
    class UPawnMovementComponent    *MovementComponent;
    class USceneComponent           *RootComponent;
    class UGKAbilitySystemComponent *GetTargetASC();
};
