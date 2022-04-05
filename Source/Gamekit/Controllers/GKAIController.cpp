// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.


#include "Gamekit/Controllers/GKAIController.h"


// IGenericTeamAgentInterface
// --------------------------

void AGKAIController::OnTeamChange() {}

void AGKAIController::SetGenericTeamId(const FGenericTeamId &TID)
{
    // Only authority can change TeamID
    if (GetNetMode() == ENetMode::NM_Client)
    {
        return;
    }

    Super::SetGenericTeamId(TID);

    // Change Possessed pawn to the new TeamId
    auto Agent = Cast<IGenericTeamAgentInterface>(GetPawn());
    if (Agent)
    {
        Agent->SetGenericTeamId(GetGenericTeamId());
    }

    //
    OnTeamChange();
}
