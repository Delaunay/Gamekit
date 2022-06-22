// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Gamekit.h"
#include "Gamekit/Abilities/GKGameplayAbility.h"
#include "Gamekit/Abilities/GKAbilityInputs.h"

// Generated
#include "GKAbilityInterface.generated.h"


/** Struct representing a slot for an item, shown in the UI
 * The ARPG used a static FName Item Type as well, I think it makes interaction with the code
 * more complex for little gains.
 */
USTRUCT(BlueprintType)
struct GAMEKIT_API FGKAbilitySlot
{
    GENERATED_BODY()

    /** Constructor, -1 means an invalid slot */
    FGKAbilitySlot() : SlotNumber(-1) {}

    FGKAbilitySlot(int32 InSlotNumber) : SlotNumber(InSlotNumber) {}

    FGKAbilitySlot(EGK_MOBA_AbilityInputID AbilityInput) : SlotNumber(int32(AbilityInput)) {}

    /** The number of this slot, 0 indexed */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
    int32 SlotNumber;

    /** Equality operators */
    bool operator==(const FGKAbilitySlot& Other) const { return SlotNumber == Other.SlotNumber; }
    bool operator!=(const FGKAbilitySlot& Other) const { return !(*this == Other); }

    /** Implemented so it can be used in Maps/Sets */
    friend inline uint32 GetTypeHash(const FGKAbilitySlot& Key) { return GetTypeHash(Key.SlotNumber); }

    /** Returns true if slot is valid */
    bool IsValid() const { return SlotNumber >= 0; }
};

/**
 * Interface for actors that provide a set of Items bound to ItemSlots
 * This exists CharacterBase can query inventory without doing hacky player controller casts
 * It is designed only for use by native classes
 */
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UGKAbilityInterface: public UInterface
{
    GENERATED_BODY()
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAbilityRemoved, int, SlotID, UGKGameplayAbility*, Ability);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAbilityAdded, int, SlotID, UGKGameplayAbility*, Ability);


class GAMEKIT_API IGKAbilityInterface
{
    GENERATED_BODY()

    public:

    virtual void GrantAbility(TSubclassOf<UGKGameplayAbility> AbilityClass, int SlotID, int InputID, int Level = 0) = 0;

    virtual bool ActivateAbility(int SlotID) = 0;

    virtual void RemoveAbility(int SlotID) = 0;

    virtual UGKGameplayAbility const* GetAbilityInstance(int SlotID) const = 0;

    virtual FGameplayAbilitySpecHandle GetAbilityHandle(int SlotID) const = 0;

    virtual FOnAbilityRemoved& GetOnAbilityRemoved() = 0;

    virtual FOnAbilityAdded& GetOnAbilityAdded() = 0;
};


