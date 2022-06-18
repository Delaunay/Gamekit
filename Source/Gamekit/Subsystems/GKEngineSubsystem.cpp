// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Subsystems/GKEngineSubsystem.h"

// Gamekit
#include "Gamekit/Abilities/GKAbilitySystemGlobals.h"
#include "Gamekit/GKGamekitSettings.h"

void UGKEngineSubsystem::Initialize(FSubsystemCollectionBase &Collection)
{
    Super::Initialize(Collection);

    UGKGamekitSettings::Get()->InitGlobalData();
    UGKAbilitySystemGlobals::Get().InitGlobalData();
}
