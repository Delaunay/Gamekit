// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Gamekit.h"

// Unreal Engine
#include "GameFramework/GameStateBase.h"

// Generated
#include "GKGameState.generated.h"

/** Base class for GameMode, should be blueprinted */
UCLASS()
class GAMEKIT_API AGKGameStateBase: public AGameStateBase
{
    GENERATED_BODY()

    public:
    /** Constructor */
    AGKGameStateBase() {}
};
