// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.
#pragma once

// Unreal Engine
#include "CoreMinimal.h"
#include "GameFramework/GameState.h"

// Generated
#include "GKTurnGameState.generated.h"

/**
 *
 */
UCLASS(Blueprintable)
class GAMEKIT_API AGKTurnGameState: public AGameStateBase
{
    GENERATED_BODY()

    AGKTurnGameState();

    public:
    // This is used to keep track of Pathing and share it to the shader
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = UnitMap, meta = (AllowPrivateAccess = "true"))
    class UGKSharedTexture *CollisionMap;
};
