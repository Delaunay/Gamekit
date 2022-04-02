// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/FogOfWar/GKFogOfWarActorTeam.h"


AGKFogOfWarActorTeam::AGKFogOfWarActorTeam() {
    Buffer         = nullptr;
    Exploration    = nullptr;
    Vision         = nullptr;
    PreviousFrameVision = nullptr;
    UpScaledVision = nullptr;
    VisibleEnemies.Reserve(128);
    Allies.Reserve(128);
}

void AGKFogOfWarActorTeam::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AGKFogOfWarActorTeam, Allies);

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