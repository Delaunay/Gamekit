// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/GKLog.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGKNav, Log, All);

// Unreal Engine
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AITypes.h"

// Generated
#include "GKNavigationInterface.generated.h"



UCLASS(BlueprintType)
class  UGKMovementRequestContext: public UObject {

    GENERATED_BODY()

public:
    // Cached variables
    UObject* WorldCtx;

    // This component is kind of the task we want to create 
    TObjectPtr<class UPathFollowingComponent> PathFollowingComponent;

    TObjectPtr<class UNavigationSystemV1> NavSys;

    FAIMoveRequest MovementRequest;

    FPathFollowingRequestResult Result;

    bool bAllowStrafe;

    TSubclassOf<UNavigationQueryFilter> DefaultNavigationFilterClass;

    // Helper functions
    FPathFollowingRequestResult MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath);

    bool BuildPathfindingQuery(const FAIMoveRequest& MoveRequest, FPathFindingQuery& Query) const;

    FAIRequestID RequestMove(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr Path);

    void FindPathForMoveRequest(const FAIMoveRequest& MoveRequest, FPathFindingQuery& Query, FNavPathSharedPtr& OutPath) const;

    APawn* GetPawn() const;

    APawn* GetPawn();

    const FNavAgentProperties& GetNavAgentPropertiesRef() const;

    FVector GetNavAgentLocation() const;
};


/**
 * Move some of the AIController logic interfacing with the navigation system to a blueprint library
 * so the logic can be reused without AIControllers
 */
UCLASS()
class GAMEKIT_API UGKNavigationInterface : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:

    UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldCtx"))
    UGKMovementRequestContext* NewMovementContext(UObject* WorldCtx, class UPathFollowingComponent* PathFollowingComponent);

    UFUNCTION(BlueprintPure)
    void MoveToLocation(UGKMovementRequestContext* Mvt,
                        const FVector& Dest,
                        float AcceptanceRadius,
                        bool bStopOnOverlap,
                        bool bUsePathfinding,
                        bool bProjectDestinationToNavigation,
                        bool bCanStrafe,
                        TSubclassOf<UNavigationQueryFilter> FilterClass,
                        bool bAllowPartialPaths);

    UFUNCTION(BlueprintCallable)
    void MoveToActor(UGKMovementRequestContext* Movement,
                     AActor* Goal,
                     float AcceptanceRadius,
                     bool bStopOnOverlap,
                     bool bUsePathfinding,
                     bool bCanStrafe,
                     TSubclassOf<UNavigationQueryFilter> FilterClass,
                     bool bAllowPartialPaths);


    UFUNCTION(BlueprintCallable)
    bool PauseMove(UGKMovementRequestContext* Mvt);

    UFUNCTION(BlueprintCallable)
    bool ResumeMove(UGKMovementRequestContext* Mvt);

    UFUNCTION(BlueprintCallable)
    void StopMovement(UGKMovementRequestContext* Mvt);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldCtx"))
    void SimplifiedMove(UObject* WorldCtx,
                        AActor* Actor,
                        FVector const& Dest,
                        AActor* GoalActor,
                        TSubclassOf<UNavigationQueryFilter> FilterClass,
                        bool bUsePathFinding,
                        bool bMovetoActor,
                        bool bAllowPartialPaths,
                        bool bProjectDestinationToNavigation,
                        float AcceptanceRadius,
                        bool bStopOnOverlap,
                        bool bCanStrafe);
};
 