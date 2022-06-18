// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.


#include "Abilities/Abilities/GKItemAbility.h"

// Gamekit
#include "Gamekit/Abilities/GKAbilitySystemGlobals.h"


UGKItemAbility::UGKItemAbility() {
	// The exclusiveness is meant to avoid cancelling previously casting ability
	AbilityTags.AddTag(AbilityExclusive);
	BlockAbilitiesWithTag.AddTag(AbilityExclusive);

	// Is there instances where `ActivationBlockedTags` is not the same as CancelledByTags
	ActivationBlockedTags.AddTag(DisableStun);
	ActivationBlockedTags.AddTag(DisableDead);
	ActivationBlockedTags.AddTag(DisableMute);

	CancelledByTags.AddTag(DisableStun);
	CancelledByTags.AddTag(DisableDead);
	CancelledByTags.AddTag(DisableMute);
}