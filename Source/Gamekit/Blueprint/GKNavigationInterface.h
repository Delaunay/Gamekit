// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/GKLog.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGKNav, Log, All);

// Unreal Engine
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AITypes.h"
#include "Navigation/PathFollowingComponent.h"

// Generated
#include "GKNavigationInterface.generated.h"

USTRUCT(BlueprintType)
struct FGKNavQueryResult {
    GENERATED_USTRUCT_BODY()

    FNavPathSharedPtr NavPath;
    FAIRequestID RequestID;
    TEnumAsByte<EPathFollowingRequestResult::Type> Code;
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

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldCtx"))
    static FGKNavQueryResult SimplifiedMove(UObject* WorldCtx,
        AActor* Actor,
        FVector const& Dest,
        AActor* GoalActor,
        TSubclassOf<UNavigationQueryFilter> FilterClass,
        bool bUsePathFinding,
        bool bMoveToActor,
        bool bAllowPartialPaths,
        bool bProjectDestinationToNavigation,
        float AcceptanceRadius,
        bool bStopOnOverlap,
        bool bCanStrafe,
        bool bDebug
    );

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldCtx"))
    static FGKNavQueryResult FindPath(UObject* WorldCtx,
        AActor* Actor,
        FVector const& Dest,
        AActor* GoalActor,
        TSubclassOf<UNavigationQueryFilter> FilterClass,
        bool bUsePathFinding,
        bool bMoveToActor,
        bool bAllowPartialPaths,
        bool bProjectDestinationToNavigation,
        float AcceptanceRadius,
        bool bStopOnOverlap,
        bool bCanStrafe,
        bool bDebug
    );

    UFUNCTION(BlueprintPure)
    static TArray<FVector> GetPathPoints(FGKNavQueryResult const& Path) {
        TArray<FVector> Points;

        if (Path.Code == EPathFollowingRequestResult ::RequestSuccessful && Path.NavPath){
            Points.Reserve(Path.NavPath->GetPathPoints().Num());

            for(FNavPathPoint& Point: Path.NavPath->GetPathPoints()){
                Points.Add(Point.Location);
            }
        }
        return Points;
    }
};
 