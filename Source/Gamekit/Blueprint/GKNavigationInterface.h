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

USTRUCT(BlueprintType)
struct FGKNavPathHandle {
    GENERATED_USTRUCT_BODY()

    FNavPathSharedPtr NavPath;
    FAIRequestID RequestID;
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
    FGKNavPathHandle SimplifiedMove(UObject* WorldCtx,
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

    UFUNCTION(BlueprintPure)
    void GetPathPoints(FGKNavPathHandle const& Path) {
        return Path.NavPath->GetPathPoints();
    }
};
 