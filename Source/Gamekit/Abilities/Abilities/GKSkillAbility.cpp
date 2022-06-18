// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

// Include
#include "Abilities/Abilities/GKSkillAbility.h"

// Gamekit
#include "Gamekit/Abilities/GKAbilitySystemGlobals.h"


UGKSkillAbility::UGKSkillAbility() {
	// The exclusiveness is meant to avoid cancelling previously casting ability

	// this ability is blocked by other exclusive abilities
	// and block other exclusive abilities
	AbilityTags.AddTag(AbilityExclusive);
	BlockAbilitiesWithTag.AddTag(AbilityExclusive);

	// Is there instances where `ActivationBlockedTags` is not the same as CancelledByTags
	ActivationBlockedTags.AddTag(DisableStun);
	ActivationBlockedTags.AddTag(DisableDead);
	ActivationBlockedTags.AddTag(DisableSilence);

	CancelledByTags.AddTag(DisableStun);
	CancelledByTags.AddTag(DisableDead);
	CancelledByTags.AddTag(DisableSilence);
}