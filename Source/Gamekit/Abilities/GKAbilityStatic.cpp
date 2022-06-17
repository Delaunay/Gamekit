// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

// Gamekit
#include "Gamekit/Abilities/GKAbilityStatic.h"
#include "Gamekit/Abilities/Targeting/GKAbilityTarget_PlayerControllerTrace.h"

// Unreal Engine
#include "GenericTeamAgentInterface.h"

FGKAbilityStatic::FGKAbilityStatic():
    MaxLevel(1), Price(0), MaxStack(1), AbilityTargetActorClass(AGKAbilityTarget_PlayerControllerTrace::StaticClass())
{

    AbilityKind        = EGK_AbilityKind::None;
    Icon               = nullptr;
    CastMaxRange       = 1800;
    CastMinRange       = 0;
    MaxStack           = 1;
    Price              = 0;
    Duration           = 1;
    AbilityBehavior    = EGK_ActivationBehavior::NoTarget;
    TargetActorFaction = SetFlag(0, ETeamAttitude::Hostile);
    CastTime           = 1;
    ChannelTime        = 0;
    AbilityAnimation   = EGK_AbilityAnimation::Attack;
    AreaOfEffect       = 16.f;
    Slot               = EGK_ItemSlot::None;
}