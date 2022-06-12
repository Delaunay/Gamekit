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


UGKMovementRequestContext* UGKNavigationInterface::NewMovementContext(UObject* WorldCtx, class UPathFollowingComponent* PathFollowingComponent)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldCtx, EGetWorldErrorMode::LogAndReturnNull);

	UGKMovementRequestContext* Ctx = NewObject<UGKMovementRequestContext>();
	Ctx->WorldCtx = WorldCtx;
	Ctx->PathFollowingComponent = PathFollowingComponent;
	Ctx->NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	return Ctx;
}

void UGKNavigationInterface::MoveToLocation(UGKMovementRequestContext*Mvt,
	const FVector& Dest,
	float AcceptanceRadius,
	bool bStopOnOverlap,
	bool bUsePathfinding,
	bool bProjectDestinationToNavigation,
	bool bCanStrafe,
	TSubclassOf<UNavigationQueryFilter> FilterClass,
	bool bAllowPartialPaths)
{
	if (Mvt->PathFollowingComponent && Mvt->PathFollowingComponent->GetStatus() != EPathFollowingStatus::Idle)
	{
		Mvt->PathFollowingComponent->AbortMove(*this,
			FPathFollowingResultFlags::ForcedScript | FPathFollowingResultFlags::NewRequest,
			FAIRequestID::CurrentRequest,
			EPathFollowingVelocityMode::Keep);
	}

	FAIMoveRequest& MoveReq = Mvt->MovementRequest;
	MoveReq.SetGoalLocation(Dest);
	MoveReq.SetUsePathfinding(bUsePathfinding);
	MoveReq.SetAllowPartialPath(bAllowPartialPaths);
	MoveReq.SetProjectGoalLocation(bProjectDestinationToNavigation);
	MoveReq.SetNavigationFilter(*FilterClass ? FilterClass : Mvt->DefaultNavigationFilterClass);
	MoveReq.SetAcceptanceRadius(AcceptanceRadius);
	MoveReq.SetReachTestIncludesAgentRadius(bStopOnOverlap);
	MoveReq.SetCanStrafe(bCanStrafe);

	Mvt->Result = Mvt->MoveTo(Mvt->MovementRequest, nullptr);
}

void UGKNavigationInterface::MoveToActor(UGKMovementRequestContext*Mvt,
	AActor* Goal,
	float AcceptanceRadius,
	bool bStopOnOverlap,
	bool bUsePathfinding,
	bool bCanStrafe,
	TSubclassOf<UNavigationQueryFilter> FilterClass,
	bool bAllowPartialPaths)
{
	if (Mvt->PathFollowingComponent && Mvt->PathFollowingComponent->GetStatus() != EPathFollowingStatus::Idle)
	{
		Mvt->PathFollowingComponent->AbortMove(*this,
			FPathFollowingResultFlags::ForcedScript | FPathFollowingResultFlags::NewRequest,
			FAIRequestID::CurrentRequest,
			EPathFollowingVelocityMode::Keep);
	}

	FAIMoveRequest& MoveReq = Mvt->MovementRequest;
	MoveReq.SetGoalActor(Goal);
	MoveReq.SetUsePathfinding(bUsePathfinding);
	MoveReq.SetAllowPartialPath(bAllowPartialPaths);
	MoveReq.SetNavigationFilter(*FilterClass ? FilterClass : Mvt->DefaultNavigationFilterClass);
	MoveReq.SetAcceptanceRadius(AcceptanceRadius);
	MoveReq.SetReachTestIncludesAgentRadius(bStopOnOverlap);
	MoveReq.SetCanStrafe(bCanStrafe);

	Mvt->Result = Mvt->MoveTo(Mvt->MovementRequest, nullptr);
}

bool UGKNavigationInterface::PauseMove(UGKMovementRequestContext*Mvt) {
	if (Mvt->PathFollowingComponent != NULL && Mvt->Result.MoveId.IsEquivalent(Mvt->PathFollowingComponent->GetCurrentRequestId()))
	{
		Mvt->PathFollowingComponent->PauseMove(Mvt->Result.MoveId, EPathFollowingVelocityMode::Reset);
		return true;
	}
	return false;
}

bool UGKNavigationInterface::ResumeMove(UGKMovementRequestContext*Mvt) {
	if (Mvt->PathFollowingComponent != NULL && Mvt->Result.MoveId.IsEquivalent(Mvt->PathFollowingComponent->GetCurrentRequestId()))
	{
		Mvt->PathFollowingComponent->ResumeMove(Mvt->Result.MoveId);
		return true;
	}
	return false;
}

void UGKNavigationInterface::StopMovement(UGKMovementRequestContext*Mvt) {
	Mvt->PathFollowingComponent->AbortMove(*Mvt->GetPawn(), FPathFollowingResultFlags::MovementStop | FPathFollowingResultFlags::ForcedScript);
}

FPathFollowingRequestResult UGKMovementRequestContext::MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath) {
	// both MoveToActor and MoveToLocation can be called from blueprints/script and should keep only single movement request at the same time.
	// this function is entry point of all movement mechanics - do NOT abort in here, since movement may be handled by AITasks, which support stacking 

	// SCOPE_CYCLE_COUNTER(STAT_MoveTo);
	GKNAV_ERROR(TEXT("MoveTo: %s"), *MoveRequest.ToString());

	FPathFollowingRequestResult ResultData;
	ResultData.Code = EPathFollowingRequestResult::Failed;

	if (MoveRequest.IsValid() == false)
	{
		GKNAV_ERROR(TEXT("MoveTo request failed due MoveRequest not being valid. Most probably desired Goal Actor not longer exists. MoveRequest: '%s'"), *MoveRequest.ToString());
		return ResultData;
	}

	if (PathFollowingComponent == nullptr)
	{
		GKNAV_ERROR(TEXT("MoveTo request failed due missing PathFollowingComponent"));
		return ResultData;
	}

	ensure(MoveRequest.GetNavigationFilter() || !DefaultNavigationFilterClass);

	bool bCanRequestMove = true;
	bool bAlreadyAtGoal = false;

	if (!MoveRequest.IsMoveToActorRequest())
	{
		if (MoveRequest.GetGoalLocation().ContainsNaN() || FAISystem::IsValidLocation(MoveRequest.GetGoalLocation()) == false)
		{
			GKNAV_ERROR(TEXT("AAIController::MoveTo: Destination is not valid! Goal(%s)"), TEXT_AI_LOCATION(MoveRequest.GetGoalLocation()));
			bCanRequestMove = false;
		}

		// fail if projection to navigation is required but it failed
		if (bCanRequestMove && MoveRequest.IsProjectingGoal())
		{
			const FNavAgentProperties& AgentProps = GetNavAgentPropertiesRef();
			FNavLocation ProjectedLocation;

			if (NavSys && !NavSys->ProjectPointToNavigation(MoveRequest.GetGoalLocation(), ProjectedLocation, INVALID_NAVEXTENT, &AgentProps))
			{
				if (MoveRequest.IsUsingPathfinding())
				{
					GKNAV_ERROR(TEXT("AAIController::MoveTo failed to project destination location to navmesh"));
				}
				else
				{
					GKNAV_ERROR(TEXT("AAIController::MoveTo failed to project destination location to navmesh, path finding is disabled perhaps disable goal projection ?"));
				}

				bCanRequestMove = false;
			}

			MoveRequest.UpdateGoalLocation(ProjectedLocation.Location);
		}

		bAlreadyAtGoal = bCanRequestMove && PathFollowingComponent->HasReached(MoveRequest);
	}
	else
	{
		bAlreadyAtGoal = bCanRequestMove && PathFollowingComponent->HasReached(MoveRequest);
	}

	if (bAlreadyAtGoal)
	{
		GKNAV_LOG(TEXT("MoveTo: already at goal!"));
		ResultData.MoveId = PathFollowingComponent->RequestMoveWithImmediateFinish(EPathFollowingResult::Success);
		ResultData.Code = EPathFollowingRequestResult::AlreadyAtGoal;
	}
	else if (bCanRequestMove)
	{
		FPathFindingQuery PFQuery;

		const bool bValidQuery = BuildPathfindingQuery(MoveRequest, PFQuery);
		if (bValidQuery)
		{
			FNavPathSharedPtr Path;
			FindPathForMoveRequest(MoveRequest, PFQuery, Path);

			const FAIRequestID RequestID = Path.IsValid() ? RequestMove(MoveRequest, Path) : FAIRequestID::InvalidRequest;
			if (RequestID.IsValid())
			{
				bAllowStrafe = MoveRequest.CanStrafe();
				ResultData.MoveId = RequestID;
				ResultData.Code = EPathFollowingRequestResult::RequestSuccessful;

				if (OutPath)
				{
					*OutPath = Path;
				}
			}
		}
	}

	if (ResultData.Code == EPathFollowingRequestResult::Failed)
	{
		ResultData.MoveId = PathFollowingComponent->RequestMoveWithImmediateFinish(EPathFollowingResult::Invalid);
	}

	return ResultData;
}


bool UGKMovementRequestContext::BuildPathfindingQuery(const FAIMoveRequest& MoveRequest, FPathFindingQuery& Query) const
{
	bool bResult = false;

	const ANavigationData* NavData = (NavSys == nullptr) ? nullptr :
		MoveRequest.IsUsingPathfinding() ? NavSys->GetNavDataForProps(GetNavAgentPropertiesRef(), GetNavAgentLocation()) :
		NavSys->GetAbstractNavData();

	if (NavData)
	{
		FVector GoalLocation = MoveRequest.GetGoalLocation();
		if (MoveRequest.IsMoveToActorRequest())
		{
			const INavAgentInterface* NavGoal = Cast<const INavAgentInterface>(MoveRequest.GetGoalActor());
			if (NavGoal)
			{
				const FVector Offset = NavGoal->GetMoveGoalOffset(GetPawn());
				GoalLocation = FQuatRotationTranslationMatrix(MoveRequest.GetGoalActor()->GetActorQuat(), NavGoal->GetNavAgentLocation()).TransformPosition(Offset);
			}
			else
			{
				GoalLocation = MoveRequest.GetGoalActor()->GetActorLocation();
			}
		}

		FSharedConstNavQueryFilter NavFilter = UNavigationQueryFilter::GetQueryFilter(*NavData, GetPawn(), MoveRequest.GetNavigationFilter());
		Query = FPathFindingQuery(GetPawn(), *NavData, GetNavAgentLocation(), GoalLocation, NavFilter);
		Query.SetAllowPartialPaths(MoveRequest.IsUsingPartialPaths());

		if (PathFollowingComponent)
		{
			PathFollowingComponent->OnPathfindingQuery(Query);
		}

		bResult = true;
	}
	else
	{
		if (NavSys == nullptr)
		{
			GKNAV_WARNING(TEXT("Unable AAIController::BuildPathfindingQuery due to no NavigationSystem present. Note that even pathfinding-less movement requires presence of NavigationSystem."));
		}
		else
		{
			GKNAV_WARNING(TEXT("Unable to find NavigationData instance while calling AAIController::BuildPathfindingQuery"));
		}
	}

	return bResult;
}

FAIRequestID UGKMovementRequestContext::RequestMove(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr Path)
{
	uint32 RequestID = FAIRequestID::InvalidRequest;
	if (PathFollowingComponent)
	{
		RequestID = PathFollowingComponent->RequestMove(MoveRequest, Path);
	}

	return RequestID;
}


void UGKMovementRequestContext::FindPathForMoveRequest(const FAIMoveRequest& MoveRequest, FPathFindingQuery& Query, FNavPathSharedPtr& OutPath) const
{
	SCOPE_CYCLE_COUNTER(STAT_AI_Overall);

	if (NavSys)
	{
		FPathFindingResult PathResult = NavSys->FindPathSync(Query);
		if (PathResult.Result != ENavigationQueryResult::Error)
		{
			if (PathResult.IsSuccessful() && PathResult.Path.IsValid())
			{
				if (MoveRequest.IsMoveToActorRequest())
				{
					PathResult.Path->SetGoalActorObservation(*MoveRequest.GetGoalActor(), 100.0f);
				}

				PathResult.Path->EnableRecalculationOnInvalidation(true);
				OutPath = PathResult.Path;
			}
		}
		else
		{
			GKNAV_ERROR(
				TEXT("Trying to find path to %s resulted in Error"), 
				MoveRequest.IsMoveToActorRequest() ? *GetNameSafe(MoveRequest.GetGoalActor()) : *MoveRequest.GetGoalLocation().ToString()
			);
			GKNAV_ERROR(TEXT("Failed move to %s"), *GetNameSafe(MoveRequest.GetGoalActor()));
		}
	}
}


const FNavAgentProperties& UGKMovementRequestContext::GetNavAgentPropertiesRef() const {
	APawn* Pawn = GetPawn();

	if (Pawn)
		return Pawn->GetNavAgentPropertiesRef();

	return FNavAgentProperties::DefaultProperties;
}


FVector UGKMovementRequestContext::GetNavAgentLocation() const {
	APawn* Pawn = GetPawn();

	if (Pawn)
		return Pawn->GetNavAgentLocation();

	return FVector::ZeroVector;
}

APawn* UGKMovementRequestContext::GetPawn() const {
	APawn* Pawn = Cast<APawn>(PathFollowingComponent->GetOwner());

	if (Pawn) {
		return Pawn;
	}

	AController* Controller = Cast<AController>(PathFollowingComponent->GetOwner());

	if (Controller) {
		return Controller->GetPawn();
	}

	return nullptr;
}

APawn* UGKMovementRequestContext::GetPawn() {
	APawn* Pawn = Cast<APawn>(PathFollowingComponent->GetOwner());

	if (Pawn) {
		return Pawn;
	}

	AController* Controller = Cast<AController>(PathFollowingComponent->GetOwner());

	if (Controller) {
		return Controller->GetPawn();
	}

	return nullptr;
}

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

void UGKNavigationInterface::SimplifiedMove(
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

	// Basic Sanitation
	bMovetoActor = GoalActor != nullptr ? true : false;

	// Resolve some Actor resources
	APawn* Pawn = Cast<APawn>(Actor);
	const FNavAgentProperties& NavAgentProps = Pawn? Pawn->GetNavAgentPropertiesRef() : FNavAgentProperties::DefaultProperties;

	UPathFollowingComponent* PathFollowingComponent = Pawn? Cast<UPathFollowingComponent>(Pawn->GetComponentByClass(UPathFollowingComponent::StaticClass())): nullptr;

	FNavPathSharedPtr OutPath;

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
			OutPath = PathResult.Path;
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

		PathFollowingComponent->RequestMove(MoveReq, OutPath);
	}
}