// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "GKTeamState.generated.h"

/**
 * Team State holds private information about a given team.
 * The information is only replicated to teammates.
 * Server is in full control of its state and it gets replicated
 * conditionally to clients.
 * 
 * In Gamekit this is used by the fog of war to store
 * components participating in the fog.
 * So the fog of war is unaware of enemies (cheat prevention).
 * We want to minimize the information clients receive about enemies.
 * So even in the event of a cheat, the impact stays limited.
 * 
 * The Team state is created by the game mode at the same time as the team
 * is assigned to the player
 * 
 * TODO: how would this work with the replication graph
 */
UCLASS()
class GAMEKIT_API AGKTeamState : public AInfo
{
	GENERATED_BODY()
};
