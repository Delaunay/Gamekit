// BSD 3-Clause License Copyright (c) 2021, Pierre Delaunay All rights reserved.

#pragma once

#include "Gamekit.h"

#include "Items/GKItem.h"
#include "Items/GKItemTypes.h"

#include "GKInventoryInterface.generated.h"

/**
 * Interface for actors that provide a set of Items bound to ItemSlots
 * This exists CharacterBase can query inventory without doing hacky player controller casts
 * It is designed only for use by native classes
 */
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UGKInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

class GAMEKIT_API IGKInventoryInterface
{
	GENERATED_BODY()

public:
	/** Returns the map of items to data */
	virtual const TMap<UGKItem*, FGKItemData>& GetInventoryDataMap() const = 0;

	/** Returns the map of slots to items */
	virtual const TMap<FGKAbilitySlot, UGKItem*>& GetSlottedItemMap() const = 0;

	/** Gets the delegate for inventory item changes */
	virtual FOnInventoryItemChangedNative& GetInventoryItemChangedDelegate() = 0;

	/** Gets the delegate for inventory slot changes */
	virtual FOnSlottedItemChangedNative& GetSlottedItemChangedDelegate() = 0;

	/** Gets the delegate for when the inventory loads */
	virtual FOnInventoryLoadedNative& GetInventoryLoadedDelegate() = 0;
};

