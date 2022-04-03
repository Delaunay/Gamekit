// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once


// Unreal Engine
#include "GenericTeamAgentInterface.h"
 

/** Retrieved owner attitude toward given Other object */
ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& A, const AActor& B)


#define DEFINE_TEAM_AGENT()\
	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;\
	virtual FGenericTeamId GetGenericTeamId() const override;\
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;\
	UPROPERTY(Replicated)\
	FGenericTeamId TeamId;

#define DECLARE_TEAM_AGENT(ClassName)\
	void ClassName::SetGenericTeamId(const FGenericTeamId &TeamID) { TeamId = TeamID; }\
	FGenericTeamId ClassName::GetGenericTeamId() const { return TeamId; }\
	ETeamAttitude::Type ClassName::GetTeamAttitudeTowards(const AActor &Other) const {\
		return ::GetTeamAttitudeTowards(*this, Other);\
	}
