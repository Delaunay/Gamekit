// BSD 3-Clause License Copyright (c) 2021, Pierre Delaunay All rights reserved.

#include "GKEngineSubsystem.h"

#include "Abilities/GKAbilitySystemGlobals.h"

void UGKEngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UGKAbilitySystemGlobals::Get().InitGlobalData();
}
