// BSD 3-Clause License Copyright (c) 2021, Pierre Delaunay All rights reserved.

#pragma once

#include "Items/GKItem.h"
#include "GKWeaponItem.generated.h"

/** Native base class for weapons, should be blueprinted */
UCLASS(Blueprintable)
class GAMEKIT_API UGKWeaponItem : public UGKItem
{
	GENERATED_BODY()

public:
	/** Constructor */
	UGKWeaponItem()
	{
		
	}

	/** Weapon actor to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	TSubclassOf<AActor> WeaponActor;
};