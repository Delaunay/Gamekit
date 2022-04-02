// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "GKWorldSettings.h"

// Gamekit
#include "Gamekit/Gamekit.h"
#include "Gamekit/GKLog.h"


void AGKWorldSettings::BuildTeamCache() const {
    if (TeamCache.Num() == 0 && Teams) {
        TeamCache.Reserve(Teams->GetRowMap().Num());
        
        int i = 0;
        for (auto RowMapIter(Teams->GetRowMap().CreateConstIterator()); RowMapIter; ++RowMapIter)
	    {
            FGKTeamInfo* Info = reinterpret_cast<FGKTeamInfo*>(RowMapIter.Value());
            Info->TeamId = i;
            TeamCache.Add(Info);
            i += 1;
        }
    }

    if (Teams && TeamCache.Num() != Teams->GetRowMap().Num())
    {
        GK_WARNING(TEXT("Team count changed at runtime!"));
    }
}

class UDataTable const *AGKWorldSettings::GetTeams() const { 
    BuildTeamCache();
    return Teams;
}

FGKTeamInfo const *AGKWorldSettings::GetTeamInfo(int Index) const {
    BuildTeamCache();

    if (Index < TeamCache.Num())
        return TeamCache[Index]; 

    return nullptr;
}

FGKTeamInfo const *AGKWorldSettings::GetTeamInfoFromName(FName Name) const { 
    BuildTeamCache();

    for (auto const &TeamInfo: TeamCache)
    {
        if (TeamInfo->Name == Name)
        {
            return TeamInfo;
        }
    }
    return nullptr;
}
