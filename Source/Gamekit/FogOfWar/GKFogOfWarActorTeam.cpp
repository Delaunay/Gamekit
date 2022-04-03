// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/FogOfWar/GKFogOfWarActorTeam.h"

// Gamekit
#include "Gamekit/GKLog.h"


DECLARE_TEAM_AGENT(AGKFogOfWarActorTeam)


AGKFogOfWarActorTeam::AGKFogOfWarActorTeam() {
    Buffer         = nullptr;
    Exploration    = nullptr;
    Vision         = nullptr;
    PreviousFrameVision = nullptr;
    UpScaledVision = nullptr;
    VisibleEnemies.Reserve(128);
    Allies.Reserve(128);

    NetUpdateFrequency = 10.;
    bOnlyRelevantToOwner  = false;
    bReplicates = true;
}

void AGKFogOfWarActorTeam::OnRep_Allies() { 
    GK_WARNING(TEXT("Replicated to clients %d"), Allies.Num());

}

void AGKFogOfWarActorTeam::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AGKFogOfWarActorTeam, Allies);
    DOREPLIFETIME(AGKFogOfWarActorTeam, TeamId);
}

bool AGKFogOfWarActorTeam::IsNetRelevantFor(
    const AActor * RealViewer,  // Network connection (Player Controller / UPlayer)
    const AActor * ViewTarget,  // Actor which the player is viewing through
    const FVector & SrcLocation) const
{
    if (GetTeamAttitudeTowards(*RealViewer) == ETeamAttitude::Friendly)
    {
        return true;
    }

    return Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
}
