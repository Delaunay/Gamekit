// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Subsystems/GKEngineSubsystem.h"

// Gamekit
#include "Gamekit/Abilities/GKAbilitySystemGlobals.h"

void UGKEngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UGKAbilitySystemGlobals::Get().InitGlobalData();
}
