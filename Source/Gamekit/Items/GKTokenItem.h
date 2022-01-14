// BSD 3-Clause License Copyright (c) 2021, Pierre Delaunay All rights reserved.

#pragma once

#include "Items/GKItem.h"
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