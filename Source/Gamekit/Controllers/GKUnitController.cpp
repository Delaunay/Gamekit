// BSD 3-Clause License
//
// Copyright (c) 2019, Pierre Delaunay
// All rights reserved.

#include "GKUnitController.h"

#include "Grid/GKHexGridPathFollowingComponent.h"

AGKUnitAIController::AGKUnitAIController(){
	auto cmp = CreateDefaultSubobject<UGKHexGridPathFollowingComponent>(TEXT("HexPathFollowingComponent"));
	cmp->OnRequestFinished.AddUObject(this, &AAIController::OnMoveCompleted);
	this->SetPathFollowingComponent(cmp);
	this->GetPathFollowingComponent()->SetAcceptanceRadius(1);
}

