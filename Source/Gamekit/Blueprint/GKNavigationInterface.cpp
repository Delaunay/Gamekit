// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

// Include
#include "Gamekit/Blueprint/GKNavigationInterface.h"

// Gamekit
#define GKNAV_WARNING(Format, ...) GAMEKIT_LOG(LogGKNav, Warning, Format, __VA_ARGS__)
#define GKNAV_DISPLAY(Format, ...) GAMEKIT_LOG(LogGKNav, Display, Format, __VA_ARGS__)
#define GKNAV_LOG(Format, ...)     GAMEKIT_LOG(LogGKNav, Log, Format, __VA_ARGS__)
#define GKNAV_ERROR(Format, ...)   GAMEKIT_LOG(LogGKNav, Error, Format, __VA_ARGS__)

DEFINE_LOG_CATEGORY(LogGKNav)

// Unreal Engine
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AIController.h"


struct FGKMoveToArguments {
    // Inputs
    UWorld* World;
    AActor* Actor;
    FVector Dest;
    AActor* GoalActor;
    AActor* Controller;
    TSubclassOf<UNavigationQueryFilter> FilterClass;
    bool    bUsePathFinding;
    bool    bMovetoActor;
    bool    bAllowPartialPaths;
    bool    bProjectDestinationToNavigation;
    float   AcceptanceRadius;
    bool    bStopOnOverlap;
    bool    bCanStrafe;
    bool    bDebug;

    // Cached variables
    APawn*                      Pawn;
    FVector                     GoalLocation;
    FNavAgentProperties const*  NavAgentProps;
    UPathFollowingComponent*    PathFollowingComponent;
    UNavigationSystemV1*        NavSys;
    FPathFindingQuery           Query;

    // Results
    FGKNavQueryResult Results;

    static FGKMoveToArguments NewMoveTo(
        UObject* WorldCtx,
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
        bool bCanStrafe,
        bool bDebug);

    // Resolve Goal location; checks for Nans & validity of the vector
    // project the goal
    // resolve actor goal location
    bool ResolveGoalLocation();

    // Builds a NavQuery and find the nav path to follow to reach destination
    bool FindPathToGoalSync();

    // Use the PathFollowing component to request a move if available
    void RequestPathFollowingMove(FAIMoveRequest const& MoveReq);

    // Draw the path segments
    void DebugDrawPath();

    bool HasReachedLocation(FAIMoveRequest const& MoveReq) {
        return PathFollowingComponent && PathFollowingComponent->HasReached(MoveReq);
    }

    FAIMoveRequest BuildsMoveRequest();
};


FAIMoveRequest FGKMoveToArguments::BuildsMoveRequest() {
    FAIMoveRequest MoveReq;

    if (bMovetoActor) {
        MoveReq.SetGoalActor(GoalActor);
    }
    else {
        MoveReq.SetGoalLocation(GoalLocation);
    }

    MoveReq.SetUsePathfinding(bUsePathFinding);
    MoveReq.SetAllowPartialPath(bAllowPartialPaths);
    MoveReq.SetProjectGoalLocation(bProjectDestinationToNavigation);
    MoveReq.SetNavigationFilter(*FilterClass ? FilterClass : UNavigationQueryFilter::StaticClass());
    MoveReq.SetAcceptanceRadius(AcceptanceRadius);
    MoveReq.SetReachTestIncludesAgentRadius(bStopOnOverlap);
    MoveReq.SetCanStrafe(bCanStrafe);

    return MoveReq;
}


FGKMoveToArguments FGKMoveToArguments::NewMoveTo(UObject* WorldCtx,
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
    bool bCanStrafe,
    bool bDebug)
{
    FGKMoveToArguments Args;

    Args.World = GEngine->GetWorldFromContextObject(WorldCtx, EGetWorldErrorMode::LogAndReturnNull);
    Args.Actor                           = Actor;
    Args.Dest                            = Dest;
    Args.GoalActor                       = GoalActor;
    Args.FilterClass                     = FilterClass;
    Args.bUsePathFinding                 = bUsePathFinding;
    Args.bMovetoActor                    = bMovetoActor;
    Args.bAllowPartialPaths              = bAllowPartialPaths;
    Args.bProjectDestinationToNavigation = bProjectDestinationToNavigation;
    Args.AcceptanceRadius                = AcceptanceRadius;
    Args.bStopOnOverlap                  = bStopOnOverlap;
    Args.bCanStrafe                      = bCanStrafe;
    Args.bDebug                          = bDebug;

    Args.NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(Args.World);

    // Actor can both be an AIController or an Actor
    // we put just Controller because more than just the AI controller could have the comp
    AController* Controller = Cast<AController>(Actor);
    APawn* Pawn = Cast<APawn>(Actor);

    if (Controller){
        Args.Controller = Controller;
        Args.Pawn = Controller->GetPawn();
        Args.PathFollowingComponent = Cast<UPathFollowingComponent>(Controller->GetComponentByClass(UPathFollowingComponent::StaticClass()));
    }

    if (Pawn) {
        Args.Pawn = Pawn;
        Args.Controller = Pawn;
        Args.PathFollowingComponent = Cast<UPathFollowingComponent>(Args.Pawn->GetComponentByClass(UPathFollowingComponent::StaticClass()));
    }
        
    return Args;
}


void FGKMoveToArguments::RequestPathFollowingMove(FAIMoveRequest const& MoveReq) {
    if (PathFollowingComponent)
    {
        if (PathFollowingComponent->GetStatus() != EPathFollowingStatus::Idle) {
            PathFollowingComponent->AbortMove(
                *Controller,
                FPathFollowingResultFlags::ForcedScript | FPathFollowingResultFlags::NewRequest,
                FAIRequestID::CurrentRequest,
                EPathFollowingVelocityMode::Keep
            );
        }

        PathFollowingComponent->OnPathfindingQuery(Query);

        // This happens earlier inside the AI controller
        // because the FAIMoveRequest is created much earlier
        if (!PathFollowingComponent->HasReached(MoveReq)) {
            Results.RequestID = PathFollowingComponent->RequestMove(MoveReq, Results.NavPath);
            Results.Code = EPathFollowingRequestResult::RequestSuccessful;
        }
        else {
            Results.RequestID = PathFollowingComponent->RequestMoveWithImmediateFinish(EPathFollowingResult::Success);
            Results.Code = EPathFollowingRequestResult::AlreadyAtGoal;
        }
    }
}


void FGKMoveToArguments::DebugDrawPath() {
    if (bDebug) {
        TArray<FNavPathPoint> Points = Results.NavPath->GetPathPoints();
        FVector LineStart = Points[0].Location;

        for (int i = 1; i < Points.Num(); i++) {
            auto& Point = Points[i];

            UKismetSystemLibrary::DrawDebugLine(
                World,
                LineStart,
                Point.Location,
                FLinearColor::Red,
                10.f,
                1.f
            );
            LineStart = Point.Location;
        }
    }
}

bool FGKMoveToArguments::ResolveGoalLocation() {
    GoalLocation = Dest;
    bool bCanRequestMove = true;

    if (bMovetoActor)
    {
        const INavAgentInterface* NavGoal = Cast<const INavAgentInterface>(GoalActor);
        if (NavGoal)
        {
            const FVector Offset = NavGoal->GetMoveGoalOffset(Actor);
            GoalLocation = FQuatRotationTranslationMatrix(GoalActor->GetActorQuat(), NavGoal->GetNavAgentLocation()).TransformPosition(Offset);
        }
        else
        {
            GoalLocation = GoalActor->GetActorLocation();
        }
    }
    else {
        // Move to a destination
        if (GoalLocation.ContainsNaN() || FAISystem::IsValidLocation(GoalLocation) == false)
        {
            bCanRequestMove = false;
        }

        if (bCanRequestMove && bProjectDestinationToNavigation) {

            FNavLocation ProjectedLocation;
            if (!NavSys->ProjectPointToNavigation(GoalLocation, ProjectedLocation, INVALID_NAVEXTENT, NavAgentProps))
            {
                if (bUsePathFinding)
                {
                    GKNAV_ERROR(TEXT("FGKMoveToArguments::ResolveGoalLocation failed to project destination location to navmesh"));
                }
                else
                {
                    GKNAV_ERROR(TEXT("FGKMoveToArguments::ResolveGoalLocation failed to project destination location to navmesh, path finding is disabled perhaps disable goal projection ?"));
                }

                bCanRequestMove = false;
            }

            GoalLocation = ProjectedLocation.Location;
        }

    }

    return bCanRequestMove;
}

bool FGKMoveToArguments::FindPathToGoalSync() {
    // Get the navigation data for our query
    const ANavigationData* NavData = bUsePathFinding ?
        NavSys->GetNavDataForProps(*NavAgentProps, Pawn->GetActorLocation()) :
        NavSys->GetAbstractNavData();

    // Builds the Filter
    FSharedConstNavQueryFilter NavFilter = UNavigationQueryFilter::GetQueryFilter(*NavData, Actor, FilterClass);

    // Builds the query
    Query = FPathFindingQuery(
        Actor,          
        *NavData,
        Pawn->GetActorLocation(),
        GoalLocation,
        NavFilter
    );
    Query.SetAllowPartialPaths(bAllowPartialPaths);

    // Fetch the points to reach the goal
    // this is probably what executes the A* algo
    // E:\UnrealEngine\Engine\Source\Runtime\Navmesh\Private\Detour\DetourNavMeshQuery.cpp
    FPathFindingResult PathResult = NavSys->FindPathSync(Query);

    ensure(PathResult.Path->IsValid());

    // Check for results
    if (PathResult.Result != ENavigationQueryResult::Error && PathResult.IsSuccessful() && PathResult.Path.IsValid())
    {
        // Put the ActorGoal here so the path updates when the goal moves
        if (bMovetoActor)
        {
            PathResult.Path->SetGoalActorObservation(*GoalActor, 100.0f);
        }

        PathResult.Path->EnableRecalculationOnInvalidation(true);

        Results.NavPath = PathResult.Path;
        Results.Code = EPathFollowingRequestResult::RequestSuccessful;
        return true;
    }

    return false;
}

FGKNavQueryResult UGKNavigationInterface::SimplifiedMove(
    UObject* WorldCtx,
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
    bool bCanStrafe,
    bool bDebug
) {

    FGKMoveToArguments Args = FGKMoveToArguments::NewMoveTo(
        WorldCtx,
        Actor,
        Dest,
        GoalActor,
        FilterClass,
        bUsePathFinding,
        bMovetoActor,
        bAllowPartialPaths,
        bProjectDestinationToNavigation,
        AcceptanceRadius,
        bStopOnOverlap,
        bCanStrafe,
        bDebug
    );

    if (!Args.NavSys) {
        GKNAV_ERROR(TEXT("No NavSystem found"));
        Args.Results.Code = EPathFollowingRequestResult::Failed;
        return Args.Results;
    }

    if (!Args.PathFollowingComponent) {
        GKNAV_ERROR(TEXT("No PathFollowingComponent found for %s"), *AActor::GetDebugName(Args.Actor));
        Args.Results.Code = EPathFollowingRequestResult::Failed;
        return Args.Results;
    }

    if (!Args.ResolveGoalLocation()) {
        GKNAV_ERROR(TEXT("Cannot request move"));
        Args.Results.Code = EPathFollowingRequestResult::Failed;
        return Args.Results;
    }

    FAIMoveRequest MoveRequest = Args.BuildsMoveRequest();

    if (Args.HasReachedLocation(MoveRequest)){
        Args.Results.Code = EPathFollowingRequestResult::AlreadyAtGoal;
        Args.Results.RequestID = Args.PathFollowingComponent->RequestMoveWithImmediateFinish(EPathFollowingResult::Success);
        return Args.Results;
    }

    if (!Args.FindPathToGoalSync()) {
        GKNAV_ERROR(TEXT("Could not find path to location"));
        Args.Results.Code = EPathFollowingRequestResult::Failed;
        return Args.Results;
    }

    Args.DebugDrawPath();

    Args.RequestPathFollowingMove(MoveRequest);

    GKNAV_DISPLAY(TEXT("Request Status: %d"), Args.Results.Code == EPathFollowingRequestResult::RequestSuccessful);
    return Args.Results;
}

FGKNavQueryResult UGKNavigationInterface::FindPath(
    UObject* WorldCtx,
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
    bool bCanStrafe,
    bool bDebug
) {

    FGKMoveToArguments Args = FGKMoveToArguments::NewMoveTo(
        WorldCtx,
        Actor,
        Dest,
        GoalActor,
        FilterClass,
        bUsePathFinding,
        bMovetoActor,
        bAllowPartialPaths,
        bProjectDestinationToNavigation,
        AcceptanceRadius,
        bStopOnOverlap,
        bCanStrafe,
        bDebug
    );

    if (!Args.NavSys) {
        GKNAV_ERROR(TEXT("No NavSystem found"));
        Args.Results.Code = EPathFollowingRequestResult::Failed;
        return Args.Results;
    }

    if (!Args.ResolveGoalLocation()) {
        GKNAV_ERROR(TEXT("Cannot request move"));
        Args.Results.Code = EPathFollowingRequestResult::Failed;
        return Args.Results;
    }

    if (!Args.FindPathToGoalSync()) {
        GKNAV_ERROR(TEXT("Could not find path to location"));
        Args.Results.Code = EPathFollowingRequestResult::Failed;
        return Args.Results;
    }

    return Args.Results;
}