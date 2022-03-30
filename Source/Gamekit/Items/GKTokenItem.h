// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Items/GKItem.h"

// Generated
#include "GKTokenItem.generated.h"

/** Native base class for tokens/currency, should be blueprinted */
UCLASS(Blueprintable)
class GAMEKIT_API UGKTokenItem : public UGKItem
{
	GENERATED_BODY()

public:
	/** Constructor */
	UGKTokenItem()
	{

		MaxCount = 0; // Infinite
	}
};