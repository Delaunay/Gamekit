// BSD 3-Clause License
//
// Copyright (c) 2022, Pierre Delaunay
// All rights reserved.

#pragma once

// Unreal Engine
#include "CoreMinimal.h"
#include "AIController.h"

// Generated
#include "GKUnitController.generated.h"

/**
 * Simple AI controller for unit that are not controlled by a the player
 * but could be
 */
UCLASS(Blueprintable)
class GAMEKIT_API AGKUnitAIController : public AAIController
{
	GENERATED_BODY()

public:
	AGKUnitAIController();

};
