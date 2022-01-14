// BSD 3-Clause License Copyright (c) 2021, Pierre Delaunay All rights reserved.

#pragma once

#include "Gamekit.h"

#include "Abilities/Targeting/GKAbilityTarget_Actor.h"

#include "GKCachedAbilityTargetActorInterface.generated.h"


/**
 */
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UGKCachedAbilityTargetActorInterface : public UInterface
{
	GENERATED_BODY()
};


/** Implement it in the Controller for controller trace based targeting (RTS, Top Down)
 * Implement it in the Character/Pawn for Avatar based tracing (First Person, etc...)
 */
class GAMEKIT_API IGKCachedAbilityTargetActorInterface
{
	GENERATED_BODY()

public:
	//! Retrieve the Ability Target class
	virtual TSubclassOf<AGKAbilityTarget_Actor> GetAbilityTarget_ActorClass() = 0;

	//! Returns the cached AbilityTarget Actor
	virtual AGKAbilityTarget_Actor* GetAbilityTarget_Actor() = 0;
}; 

