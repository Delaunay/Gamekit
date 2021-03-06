// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Abilities/GKAbilityStatic.h"

// Unreal Engine
#include "Abilities/Tasks/AbilityTask.h"
#include "CoreMinimal.h"

// Generated
#include "GKAbilityTask_MoveToDestination.generated.h"

/** Delegate type used, EventTag and Payload may be empty if it came from the montage callbacks */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGKMoveToDestinationDelegate,
                                            const FGameplayAbilityTargetDataHandle &,
                                            TargetData);

/**
 * This is different from the Gameplay Ability MoveToLocation built-in
 *
 * 	1. This will move the player to location without a timer
 *	2. This will turn the player to the movement direction before moving
 *  3. This can be cancelled by the user, or by opponents
 *
 * AddControllerYawInput && AddMovementInput are used to turn the character
 * mimicking what a player would have to do to turn it.
 * Hopefully making it respect the constraitn imposed by the MovementComponent.
 *
 * Turn rate is an important game mechanic that give time for the opponents to react.
 * It also give an advantage to the character that is ready (i.e facing).
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
 * AngleTolerance: Float (In Degrees, Default: 15)
 *  Angle Jitter we are willing to accept (avoid too small values)
 *  15 which will give a 30 degrees of leeway where the
 *  character can move right away. This will make the movement more natural
 *  by making the transition between the turn and movement less obvious
 *
 * TurnRate: Float (In Degrees)
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

    public:
    UGKAbilityTask_MoveToDestination(const FObjectInitializer &ObjectInitializer);

    UFUNCTION(BlueprintCallable,
              Category = "Ability|Tasks",
              meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
    static UGKAbilityTask_MoveToDestination *MoveToDestination(
            UGameplayAbility *  OwningAbility,
            FName               TaskInstanceName,
            FVector             Destination,
            float               DistanceTolerance = 10.f,
            float               AngleTolerance    = 15.f,
            float               TurnRate          = 45.f,
            float               Speed             = 600.f,
            bool                MoveToTarget      = true,
            bool                bFollowActor      = false,
            FGameplayTagContainer CancelTag       = FGameplayTagContainer(),
            bool                bUsePathfinding   = false,
            bool                Debug             = false);

    /**
     *  TargetData       : Target information, used to derive the final destination
     *  DistanceTolerance: When can we consider the target reached
     *  AngleTolerance   : When do we consider ourself facing the target
     *  TurnRate         : Rotation speed
     *  Speed            : Walk Speed
     *  MoveToTarget     : Move towards the target after the rotation
     *  bUseMovementComp : Use the movement component to set Rotation Speed & walk speed
     *  TargetKind       : PointTarget or ActorTarget
     */
    UFUNCTION(BlueprintCallable,
              Category = "Ability|Tasks",
              meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
    static UGKAbilityTask_MoveToDestination *MoveToTarget(
            UGameplayAbility *                      OwningAbility,
            FName                                   TaskInstanceName,
            const FGameplayAbilityTargetDataHandle &TargetData,
            float                                   DistanceTolerance     = 10.f,
            float                                   AngleTolerance        = 15.f,
            float                                   TurnRate              = 45.f,
            float                                   Speed                 = 600.f,
            bool                                    MoveToTarget          = true,
            bool                                    bUseMovementComponent = true,
            bool                                    bFollowActor          = false,
            FGameplayTagContainer                   CancelTag             = FGameplayTagContainer(),
            bool                                    bUsePathfinding       = false,
            bool                                    Debug                 = false);

    /** Reached Destination */
    UPROPERTY(BlueprintAssignable)
    FGKMoveToDestinationDelegate OnCompleted;

    /** The movement action was cancelled by the user */
    UPROPERTY(BlueprintAssignable)
    FGKMoveToDestinationDelegate OnCancelled;

    UPROPERTY(BlueprintAssignable)
    FGKMoveToDestinationDelegate OnTurnDone;

    virtual void InitSimulatedTask(UGameplayTasksComponent &InGameplayTasksComponent) override;

    void InitFromTargetData();

    virtual void TickTask(float DeltaTime) override;

    virtual void OnDestroy(bool AbilityIsEnding) override;

    virtual void Activate() override;

    UFUNCTION()
    virtual void ExternalCancel() override;

    UFUNCTION()
    void OnReceiveGameplayTag(const FGameplayTag Tag, int32 Count);

    // void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const;

    void Init();

    void DebugDraw();

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY()
    bool bFollowActor;

    // Arguments
    UPROPERTY()
    float TurnRate;

    UPROPERTY()
    float MaxSpeed;

private:
    UPROPERTY(replicated)
    FVector Destination;
public:

    void UpdateDestination(FVector Dest) {
        Destination = Dest;

        if (bUsePathfinding){
            FindPath();
        }
    }

    UPROPERTY()
    bool bDebug;

    UPROPERTY()
    float DistanceTolerance;

    UPROPERTY()
    float AngleTolerance;

    UPROPERTY()
    bool bTurnOnly;

    UPROPERTY()
    FGameplayAbilityTargetDataHandle TargetData;

    UPROPERTY()
    bool bUsePathfinding;

    // States
    UPROPERTY()
    TWeakObjectPtr<AActor> TargetActor;

    UPROPERTY()
    bool bIsFinished;

    UPROPERTY()
    bool bRotationFinished;

    UPROPERTY()
    FGameplayTagContainer CancelTag;

    FDelegateHandle DelegateHandle;

    // Cached Variables
    class APawn *                    Character;
    class UPawnMovementComponent *   MovementComponent;
    class USceneComponent *          RootComponent;
    class UGKAbilitySystemComponent *GetTargetASC();


    // Path finding logic

    // Find a path to the goal using the navigation system
    void FindPath();

    FNavPathSharedPtr Path;

    // Called when the path was modified
    void OnPathEvent(FNavigationPath* InPath, ENavPathEvent::Type Event);

    int32 CurrentSegment;
    int32 EndSegment;
    NavNodeRef Start;
    NavNodeRef End;

    void FindingObjectivePoint();

    // Return the current goal we should be waking toward
    FVector GetCurrentGoal();

    FVector GetEndGoal();

    void DrawPath(FNavigationPath* InPath);

    void NextSegment();
};
