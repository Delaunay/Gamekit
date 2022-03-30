// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Grid/GKHexGridPathFollowingComponent.h"

UGKHexGridPathFollowingComponent::UGKHexGridPathFollowingComponent()
{
    MyDefaultAcceptanceRadius = 1.f;
    AcceptanceRadius          = 1.f;
    CurrentAcceptanceRadius   = 1.f;
}

bool UGKHexGridPathFollowingComponent::HasReachedDestination(const FVector &CurrentLocation) const
{
    FVector GoalLocation = *Path->GetPathPointLocation(Path->GetPathPoints().Num() - 1);
    auto    d            = (GoalLocation - CurrentLocation).SizeSquared2D();
    return d < AcceptanceRadius * AcceptanceRadius;
}
