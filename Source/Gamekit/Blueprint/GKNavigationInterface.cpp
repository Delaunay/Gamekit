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


FVector GetGoalLocation(AActor* Actor, FVector const& Dest, AActor* GoalActor, bool bMovetoActor) {
	FVector GoalLocation = Dest;
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

	return GoalLocation;
}

FGKNavPathHandle UGKNavigationInterface::SimplifiedMove(
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
	bool bCanStrafe
) {
	// Fetch global resources
	UWorld* World = GEngine->GetWorldFromContextObject(WorldCtx, EGetWorldErrorMode::LogAndReturnNull);
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);

	// Resolve some Actor resources
	APawn* Pawn = Cast<APawn>(Actor);
	const FNavAgentProperties& NavAgentProps = Pawn? Pawn->GetNavAgentPropertiesRef() : FNavAgentProperties::DefaultProperties;

	UPathFollowingComponent* PathFollowingComponent = Pawn? Cast<UPathFollowingComponent>(Pawn->GetComponentByClass(UPathFollowingComponent::StaticClass())): nullptr;

	FGKNavPathHandle OutPath;

	// Get the navigation data for our query
	const ANavigationData* NavData = bUsePathFinding ?
		NavSys->GetNavDataForProps(NavAgentProps, Actor->GetActorLocation()) :
		NavSys->GetAbstractNavData();

	// Builds the Filter
	FSharedConstNavQueryFilter NavFilter = UNavigationQueryFilter::GetQueryFilter(*NavData, Actor, FilterClass);
	
	// Builds the query
	FPathFindingQuery Query = FPathFindingQuery(
		Actor, 
		*NavData, 
		Actor->GetActorLocation(), 
		GetGoalLocation(Actor, Dest, GoalActor, bMovetoActor), 
		NavFilter
	);
	Query.SetAllowPartialPaths(bAllowPartialPaths);


	// Fetch the points to reach the goal
	// this is probably what executes the A* algo
	FPathFindingResult PathResult = NavSys->FindPathSync(Query);

	// Check for results
	if (PathResult.Result != ENavigationQueryResult::Error)
	{
		if (PathResult.IsSuccessful() && PathResult.Path.IsValid())
		{
			if (bMovetoActor)
			{
				PathResult.Path->SetGoalActorObservation(*GoalActor, 100.0f);
			}

			PathResult.Path->EnableRecalculationOnInvalidation(true);
			OutPath.NavPath = PathResult.Path;
		}
	}

	// Now we can execute a move 
	if (PathFollowingComponent)
	{
		PathFollowingComponent->OnPathfindingQuery(Query);

		FAIMoveRequest MoveReq;

		if (bMovetoActor) {
			MoveReq.SetGoalActor(GoalActor);
		} else {
			MoveReq.SetGoalLocation(Dest);
		}
		MoveReq.SetGoalLocation(Dest);
		MoveReq.SetUsePathfinding(bUsePathFinding);
		MoveReq.SetAllowPartialPath(bAllowPartialPaths);
		MoveReq.SetProjectGoalLocation(bProjectDestinationToNavigation);
		MoveReq.SetNavigationFilter(*FilterClass ? FilterClass : UNavigationQueryFilter::StaticClass());
		MoveReq.SetAcceptanceRadius(AcceptanceRadius);
		MoveReq.SetReachTestIncludesAgentRadius(bStopOnOverlap);
		MoveReq.SetCanStrafe(bCanStrafe);

		OutPath.RequestID = PathFollowingComponent->RequestMove(MoveReq, OutPath.NavPath);
	}

	return OutPath;
}