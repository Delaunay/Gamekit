// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Unreal Engine
#include "CoreMinimal.h"

// MOBA/Hack'n Slash like Ability binding
// How does this work with EnhancedInput ?
// Probably doesn't
UENUM(BlueprintType)
enum class EGK_MOBA_AbilityInputID : uint8
{
    // clang-format off
	None			UMETA(DisplayName = "None"),

	Skill1			UMETA(DisplayName = "Skill1"),	// Q
	Skill2			UMETA(DisplayName = "Skill2"),	// W
	Skill3			UMETA(DisplayName = "Skill3"),	// E
	Skill4			UMETA(DisplayName = "Skill4"),	// D
	Skill5			UMETA(DisplayName = "Skill5"),	// F
	Skill6			UMETA(DisplayName = "Skill6"),	// R

	Item1			UMETA(DisplayName = "Item1"),	// 1
	Item2			UMETA(DisplayName = "Item2"),	// 2
	Item3			UMETA(DisplayName = "Item3"),	// 3
	Item4			UMETA(DisplayName = "Item4"),	// 4
	Item5			UMETA(DisplayName = "Item5"),	// 5
	Item6			UMETA(DisplayName = "Item6"),	// 6

	Item7			UMETA(DisplayName = "Item7"),	// T
	Item8			UMETA(DisplayName = "Item8"),   // V

	Confirm			UMETA(DisplayName = "Confirm"),
	Cancel			UMETA(DisplayName = "Cancel"),

	// Movements
	Move			UMETA(DisplayName = "Move"),		 // Destination
	AttackMove		UMETA(DisplayName = "AttackMove"),   // Area & Target
	FollowTarget	UMETA(DisplayName = "FollowTarget"), // Follow Ally
	Patrol			UMETA(DisplayName = "Patrol"),		 // Go through multiple points

	Size
    // clang-format on
};
