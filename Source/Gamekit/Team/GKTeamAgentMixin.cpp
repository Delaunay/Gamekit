// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.


#include "Gamekit/Team/GKTeamAgentMixin.h"


ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& A, const AActor& B)
{ 
	const IGenericTeamAgentInterface* AgentA = Cast<const IGenericTeamAgentInterface>(&A);
	const IGenericTeamAgentInterface* AgentB = Cast<const IGenericTeamAgentInterface>(&B);

	if (!AgentA || !AgentB)
    {
		return ETeamAttitude::Neutral;
    }

	// People with noteam are neutral between eachother
	// not considered to be in the same team
	auto TeamA = AgentA->GetGenericTeamId();
	auto TeamB = AgentB->GetGenericTeamId();

	// Could be implemented with the attitude solver aswell
	// but then I need to initialize it somewhere
	if (TeamA == FGenericTeamId::NoTeam || TeamB == FGenericTeamId::NoTeam)
	{
		return ETeamAttitude::Neutral;
	}
		 
	return FGenericTeamId::GetAttitude(TeamA, TeamB);
}