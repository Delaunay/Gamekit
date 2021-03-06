// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.


#include "Gamekit/FogOfWar/GKFogOfWarTeam.h"

// Gamekit
#include "Gamekit/FogOfWar/GKFogOfWar.h"
#include "Gamekit/FogOfWar/GKFogOfWarComponent.h"

// Unreal Engine
#include "GenericTeamAgentInterface.h"
#include "Net/UnrealNetwork.h"


AGKFogOfWarTeam::AGKFogOfWarTeam() {
    bReplicates = true;
    bAlwaysRelevant = false;

    Buffer              = nullptr;
    Exploration         = nullptr;
    Vision              = nullptr;
    PreviousFrameVision = nullptr;
    UpScaledVision      = nullptr;
    Visible.Reserve(128);
    Allies.Reserve(128);
}

bool AGKFogOfWarTeam::IsNetRelevantFor(const AActor *RealViewer, const AActor *ViewTarget, const FVector &SrcLocation) const
{
    bool bRelevant = false;
    auto Attitude  = FGenericTeamId::GetAttitude(TeamId, FGenericTeamId::GetTeamIdentifier(RealViewer));

    if (Attitude == ETeamAttitude::Friendly)
    {
        bRelevant = true;
    }

    return bRelevant;
}

void AGKFogOfWarTeam::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AGKFogOfWarTeam, Name);
    DOREPLIFETIME(AGKFogOfWarTeam, TeamId);
    DOREPLIFETIME(AGKFogOfWarTeam, Allies);
}


TArray<class UGKFogOfWarComponent *> const &AGKFogOfWarTeam::GetBlocking()
{
    Blocking.Reset();

    for (auto Comp: Allies)
    {
        if (Comp->BlocksVision)
        {
            Blocking.Add(Comp);
        }
    }

    for (auto Comp: Visible)
    {
        if (Comp->BlocksVision)
        {
            Blocking.Add(Comp);
        }
    }

    return Blocking;
}