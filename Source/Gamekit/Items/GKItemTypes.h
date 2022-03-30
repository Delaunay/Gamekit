// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// ----------------------------------------------------------------------------------------------------------------
// This header is for enums and structs used by classes and blueprints accross the game
// Collecting these in a single header helps avoid problems with recursive header includes
// It's also a good place to put things like data table row structs
// ----------------------------------------------------------------------------------------------------------------

// Gamekit
#include "Gamekit/Abilities/GKAbilityInputs.h"

// Unreal Engine
#include "UObject/PrimaryAssetId.h"

// Generated
#include "GKItemTypes.generated.h"

class UGKItem;
class UGKSaveGame;

/** Struct representing a slot for an item, shown in the UI
 * The ARPG used a static FName Item Type as well, I think it makes interaction with the code
 * more complex for little gains.
 */
USTRUCT(BlueprintType)
struct GAMEKIT_API FGKAbilitySlot
{
    GENERATED_BODY()

    /** Constructor, -1 means an invalid slot */
    FGKAbilitySlot(): SlotNumber(-1) {}

    FGKAbilitySlot(int32 InSlotNumber): SlotNumber(InSlotNumber) {}

    FGKAbilitySlot(EGK_MOBA_AbilityInputID AbilityInput): SlotNumber(int32(AbilityInput)) {}

    /** The number of this slot, 0 indexed */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
    int32 SlotNumber;

    /** Equality operators */
    bool operator==(const FGKAbilitySlot &Other) const { return SlotNumber == Other.SlotNumber; }
    bool operator!=(const FGKAbilitySlot &Other) const { return !(*this == Other); }

    /** Implemented so it can be used in Maps/Sets */
    friend inline uint32 GetTypeHash(const FGKAbilitySlot &Key) { return GetTypeHash(Key.SlotNumber); }

    /** Returns true if slot is valid */
    bool IsValid() const { return SlotNumber >= 0; }
};

/** Extra information about a UGKItem that is in a player's inventory */
USTRUCT(BlueprintType)
struct GAMEKIT_API FGKItemData
{
    GENERATED_BODY()

    /** Constructor, default to count/level 1 so declaring them in blueprints gives you the expected behavior */
    FGKItemData(): ItemCount(1), ItemLevel(1) {}

    FGKItemData(int32 InItemCount, int32 InItemLevel): ItemCount(InItemCount), ItemLevel(InItemLevel) {}

    /** The number of instances of this item in the inventory, can never be below 1 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
    int32 ItemCount;

    /** The level of this item. This level is shared for all instances, can never be below 1 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
    int32 ItemLevel;

    /** Equality operators */
    bool operator==(const FGKItemData &Other) const
    {
        return ItemCount == Other.ItemCount && ItemLevel == Other.ItemLevel;
    }
    bool operator!=(const FGKItemData &Other) const { return !(*this == Other); }

    /** Returns true if count is greater than 0 */
    bool IsValid() const { return ItemCount > 0; }

    /** Append an item data, this adds the count and overrides everything else */
    void UpdateItemData(const FGKItemData &Other, int32 MaxCount, int32 MaxLevel)
    {
        if (MaxCount <= 0)
        {
            MaxCount = MAX_int32;
        }

        if (MaxLevel <= 0)
        {
            MaxLevel = MAX_int32;
        }

        ItemCount = FMath::Clamp(ItemCount + Other.ItemCount, 1, MaxCount);
        ItemLevel = FMath::Clamp(Other.ItemLevel, 1, MaxLevel);
    }
};

/** Delegate called when an inventory item changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemChanged, bool, bAdded, UGKItem *, Item);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemChangedNative, bool, UGKItem *);

/** Delegate called when the contents of an inventory slot change */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlottedItemChanged, FGKAbilitySlot, ItemSlot, UGKItem *, Item);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSlottedItemChangedNative, FGKAbilitySlot, UGKItem *);

/** Delegate called when the entire inventory has been loaded, all items may have been replaced */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryLoaded);
DECLARE_MULTICAST_DELEGATE(FOnInventoryLoadedNative);

/** Delegate called when the save game has been loaded/reset */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveGameLoaded, UGKSaveGame *, SaveGame);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSaveGameLoadedNative, UGKSaveGame *);
