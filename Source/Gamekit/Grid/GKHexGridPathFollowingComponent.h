// BSD 3-Clause License
//
// Copyright (c) 2019, Pierre Delaunay
// All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/PathFollowingComponent.h"
#include "GKHexGridPathFollowingComponent.generated.h"

/**
 * This is necessary because acceptance radius is not radical enough to make
 * the actor stop exactly at the target location.
 * The orignal implementation of HasReachedDestination is fairly convoluted so
 * we simply replace it with something much simplier
 */
UCLASS(Blueprintable)
class GAMEKIT_API UGKHexGridPathFollowingComponent : public UPathFollowingComponent
{
	GENERATED_BODY()

	UGKHexGridPathFollowingComponent();

	bool HasReachedDestination(const FVector& CurrentLocation) const override;
};
