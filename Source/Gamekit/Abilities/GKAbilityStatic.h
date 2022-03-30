// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Animation/GKAnimationSet.h"
#include "Gamekit/GKAssetManager.h"
#include "Gamekit/Gamekit.h"
#include "Gamekit/Projectiles/GKProjectile.h"

// Unreal Engine
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Engine/DataTable.h"
#include "GameplayAbilitySpec.h"
#include "Styling/SlateBrush.h"

// Generated
#include "GKAbilityStatic.generated.h"

class UGKGameplayAbility;

// MOBA/Hack'n Slash like Ability binding
UENUM(BlueprintType)
enum class EGK_AbilityKind : uint8
{
    None  UMETA(DisplayName = "None"),
    Item  UMETA(DisplayName = "Item"),
    Skill UMETA(DisplayName = "Skill"),
};

// MOBA/Hack'n Slash like Ability binding
UENUM(BlueprintType)
enum class EGK_AbilityBehavior : uint8
{
    Hidden       UMETA(DisplayName = "Hidden"),       // Not shown in UI
    Passive      UMETA(DisplayName = "Passive"),      // Passive ability, enabled as soon as it is equiped
    NoTarget     UMETA(DisplayName = "NoTarget"),     // Ability is cast as soon as the input is pressed
    ActorTarget  UMETA(DisplayName = "ActorTarget"),  // Ability targets another actor
    PointTarget  UMETA(DisplayName = "PointTarget"),  // Ability targets the ground
    VectorTarget UMETA(DisplayName = "VectorTarget"), // Ability targets 2 points
    Toggle       UMETA(DisplayName = "Toggle"),       // Ability is Active or not
};

template <typename Enum>
bool IsSet(uint32 Value, Enum Flag)
{
    return (Value & (1 << static_cast<uint32>(Flag))) > 0;
}

template <typename Enum>
uint32 SetFlag(uint32 Value, Enum Flag)
{
    return (Value | (1 << static_cast<uint32>(Flag)));
}

template <typename Enum>
uint32 RemoveFlag(uint32 Value, Enum Flag)
{
    return (Value & ~(1 << static_cast<uint32>(Flag)));
}

// This is create because we cannot use include AGKAbilityTarget_Actor
// in AbilityStatic
UENUM(BlueprintType)
enum class EGK_TargetingMode : uint8
{
    None                  UMETA(DisplayName = "None"),
    PlayerControllerTrace UMETA(DisplayName = "PlayerControllerTrace"),
};

UENUM(BlueprintType)
enum class EGK_ItemSlot : uint8
{
    None,

    Head,
    Neck,
    Shoulders,
    Shirt,
    Chest,
    Belt,
    Legs,
    Feet,
    Wrist,
    Gloves,
    Finger1,
    Finger2,
    Trinket1,
    Trinket2,
    Back,
    RightHand,
    LeftHand,
    Tabard,

    Ranged,
    Ammo,
};

USTRUCT(BlueprintType)
struct GAMEKIT_API FGKAbilityCost
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayAttribute Attribute;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<float> Value;
};

USTRUCT(BlueprintType)
struct GAMEKIT_API FGKAbilityEffect
{
    GENERATED_USTRUCT_BODY()

    FName EffectName;

    TArray<float> Curve;
};

/*! Holds all data that is shared across all instances of a single ability
 * The data is saved in a datatable to enable gameplay designer and
 * traductions services to modify/update without opening the engine.
 */
USTRUCT(BlueprintType)
struct GAMEKIT_API FGKAbilityStatic: public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    public:
    FGKAbilityStatic();

    //! Primary key not visible
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Common)
    FName Name;

    //! An Ability can be both a Skill or an Item
    //! An Item is a skill you can buy while a skill is an ability that is
    //! innate to the character/class
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Common)
    EGK_AbilityKind AbilityKind;

    //! Localized name for display
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Common)
    FText LocalName;

    //! Localized description for display
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Common)
    FText LocalDescription;

    //! Icon to display
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Common)
    class UTexture2D *Icon;

    //! Maximum level this skill/item can be
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    int32 MaxLevel;

    // Should be a TArray | Ability Level can change this
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    float Duration;

    //! Customize the ability effect
    //! This is ability specific so be aware of the keys!
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    TMap<FName, FGKAbilityEffect> AbilityEffects;

    //! Specify the cost of this ability (mana, energy, etc...)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    FGKAbilityCost Cost;

    //! Cooldown curve
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    TArray<float> Cooldown;

    //! Price in game
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
    int32 Price;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
    EGK_ItemSlot Slot;

    // TODO: make a token tag if we have different kind of money

    //! If AbilityKind == Item, specifies the max count of the given
    //! item in a single stack
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
    int32 MaxStack;

    //! If true, the target can be selected while out of range
    //! the players will start moving toward the enemy to reach the
    //! minimal rang
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Target)
    bool bIgnoreRange;

    // Should be a TArray | Ability Level can change this
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Target)
    float CastMaxRange;

    // Should be a TArray | Ability Level can change this
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Target)
    float CastMinRange;

    //! Specifies how the ability is activated
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Target)
    EGK_AbilityBehavior AbilityBehavior;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Target, meta = (Bitmask, BitmaskEnum = "ETeamAttitude"))
    int32 TargetActorFaction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Target)
    TSubclassOf<AActor> TargetFilterClass;

    //! The AbilityTarget Actor this ability will use to select a target
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Target)
    TSubclassOf<AGameplayAbilityTargetActor> AbilityTargetActorClass;

    //! Used to know when a valid target is detected
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Target)
    TArray<TEnumAsByte<EObjectTypeQuery>> TargetObjectTypes;

    //! Time this ability needs before it is cast
    //! Player can be interrupted during that time
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
    float CastTime;

    //! Only usefull when AbilityBehavior is set to channel
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
    float ChannelTime;

    //! When using animation set we get refer to a specific animation kind
    //! this ability should be playing
    //! this is used in the case where the ability can be cast by different
    //! character/skeleton with their own specific set of animation
    //! we do not want to tie the ability to the character.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
    EGK_AbilityAnimation AbilityAnimation;

    //! Projectile to spawn
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Projectile);
    TSubclassOf<AGKProjectile> ProjectileActorClass;

    //! Projectile customization data
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Projectile)
    FGKProjectileStatic ProjectileData;

    //!
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AreaOfEffect);
    TSubclassOf<AActor> AOEActorClass;

    // Should be a TArray | Ability Level can change this
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AreaOfEffect)
    float AreaOfEffect;
};
