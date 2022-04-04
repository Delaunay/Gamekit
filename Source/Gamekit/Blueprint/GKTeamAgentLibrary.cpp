// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.


#include "Gamekit/Blueprint/GKTeamAgentLibrary.h"


// Gamekit
#include "Gamekit/GKWorldSettings.h"



FGKTeamInfo const* UGKTeamAgentLibrary::GetTeamInfo(class UWorld *World, FGenericTeamId TeamId) {
    auto Settings = Cast<AGKWorldSettings const>(World->GetWorldSettings());

    return GKGETATTR(Settings, GetTeamInfo(TeamId), nullptr);
}

FName UGKTeamAgentLibrary::GetTeamName(class UObject* WorldCtx, FGenericTeamId TeamId) { 
    UWorld *World = GEngine->GetWorldFromContextObject(WorldCtx, EGetWorldErrorMode::LogAndReturnNull);

    auto Result = GetTeamInfo(World, TeamId);

    return GKGETATTR(Result, Name, NAME_None);
}

FText UGKTeamAgentLibrary::GetTeamDisplayName(class UObject *WorldCtx, FGenericTeamId TeamId) {
    UWorld *World = GEngine->GetWorldFromContextObject(WorldCtx, EGetWorldErrorMode::LogAndReturnNull);

    auto Result = GetTeamInfo(World, TeamId);

    return GKGETATTR(Result, DisplayName, FText());
}