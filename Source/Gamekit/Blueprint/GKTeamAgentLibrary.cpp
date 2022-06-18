// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.


#include "Gamekit/Blueprint/GKTeamAgentLibrary.h"

// Gamekit
#include "Gamekit/GKGamekitSettings.h"



FGKTeamInfo const* UGKTeamAgentLibrary::GetTeamInfo(FGenericTeamId TeamId) {
    UGKGamekitSettings* Settings = UGKGamekitSettings::Get();

    return GKGETATTR(Settings, GetTeamInfo(TeamId), nullptr);
}

FName UGKTeamAgentLibrary::GetTeamName( FGenericTeamId TeamId) { 

    auto Result = GetTeamInfo(TeamId);
    return GKGETATTR(Result, Name, NAME_None);
}

FText UGKTeamAgentLibrary::GetTeamDisplayName(FGenericTeamId TeamId) {
    auto Result = GetTeamInfo(TeamId);

    return GKGETATTR(Result, DisplayName, FText());
}

void UGKTeamAgentLibrary::GetTeamInfoFromName(FName Name, FGKTeamInfo& TeamInfo, bool& bValid) {
    UGKGamekitSettings* Settings = UGKGamekitSettings::Get();
    
    auto Result = Settings->GetTeamInfoFromName(Name);
    bValid = false;

    if (Result != nullptr)
    {
        TeamInfo = *Result;
        bValid = true;
    }
}

void UGKTeamAgentLibrary::GetTeamInfo(FGenericTeamId Team, FGKTeamInfo& TeamInfo, bool& bValid) {
    auto Result = GetTeamInfo(Team);
    bValid = false;

    if (Result != nullptr)
    {
        TeamInfo = *Result;
        bValid = true;
    }
}