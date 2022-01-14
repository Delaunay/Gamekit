// BSD 3-Clause License Copyright (c) 2021, Pierre Delaunay All rights reserved.

#pragma once

#include "Gamekit.h"
#include "GameFramework/GameStateBase.h"
#include "GKGameState.generated.h"

/** Base class for GameMode, should be blueprinted */
UCLASS()
class GAMEKIT_API AGKGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	/** Constructor */
	AGKGameStateBase() {}
};

