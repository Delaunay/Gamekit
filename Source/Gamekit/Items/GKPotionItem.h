// BSD 3-Clause License Copyright (c) 2021, Pierre Delaunay All rights reserved.

#pragma once

#include "Items/GKItem.h"
#include "GKPotionItem.generated.h"

/** Native base class for potions, should be blueprinted */
UCLASS(Blueprintable)
class GAMEKIT_API UGKPotionItem : public UGKItem
{
	GENERATED_BODY()

public:
	/** Constructor */
	UGKPotionItem()
	{
	}
};