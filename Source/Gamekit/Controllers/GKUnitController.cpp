// BSD 3-Clause License
//
// Copyright (c) 2022, Pierre Delaunay
// All rights reserved.

#include "Gamekit/Controllers/GKUnitController.h"

#include "Gamekit/Grid/GKHexGridPathFollowingComponent.h"

AGKUnitAIController::AGKUnitAIController()
{
    auto cmp = CreateDefaultSubobject<UGKHexGridPathFollowingComponent>(TEXT("HexPathFollowingComponent"));
    cmp->OnRequestFinished.AddUObject(this, &AAIController::OnMoveCompleted);
    this->SetPathFollowingComponent(cmp);
    this->GetPathFollowingComponent()->SetAcceptanceRadius(1);
}
