// BSD 3-Clause License Copyright (c) 2021, Pierre Delaunay All rights reserved.

#include "Abilities/GKAbilityStatic.h"
#include "Abilities/Targeting/GKAbilityTarget_PlayerControllerTrace.h"

FGKAbilityStatic::FGKAbilityStatic()
	: MaxLevel(1)
	, Price(0)
	, MaxStack(1)
	, AbilityTargetActorClass(AGKAbilityTarget_PlayerControllerTrace::StaticClass())
{}