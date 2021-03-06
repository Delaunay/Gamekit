// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Animation/GKAnimationSet.h"
#include "Gamekit/GKAssetManager.h"
#include "Gamekit/Gamekit.h"
#include "Gamekit/Projectiles/GKProjectile.h"
#include "Gamekit/Utilities/GKBitFlag.h"

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
    None        UMETA(DisplayName = "None"),
    Item        UMETA(DisplayName = "Item"),
    Skill       UMETA(DisplayName = "Skill"),
    Ultimate    UMETA(DisplayName = "Ultimate"),
    // Note: Perk/Talent Can be Gameplay Effect directly
};

// MOBA/Hack'n Slash like Ability binding
UENUM(BlueprintType)
enum class EGK_ActivationBehavior : uint8
{
    Hidden       UMETA(DisplayName = "Hidden"),       // Not shown in UI
    Passive      UMETA(DisplayName = "Passive"),      // Passive ability, enabled as soon as it is equiped
    NoTarget     UMETA(DisplayName = "NoTarget"),     // Ability is cast as soon as the input is pressed
    Targeted     UMETA(DisplayName = "Targeted"),     // Ability targets something
    Charge       UMETA(DisplayName = "Charge"),       // Ability consume charges on activation
    Channel      UMETA(DisplayName = "Channel"),      // Plays animation until the ends and cast spells at the end
    Toggle       UMETA(DisplayName = "Toggle"),       // Ability is Active or not
};

UENUM(BlueprintType)
enum class EGK_TargetMode : uint8 {
    None            UMETA(DisplayName = "None"),       
    ActorTarget     UMETA(DisplayName = "ActorTarget"),      
    PointTarget     UMETA(DisplayName = "PointTarget"),     
    VectorTarget    UMETA(DisplayName = "VectorTarget"),
};

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

//! Specify the effect to apply under specific conditions
UENUM(BlueprintType)
enum class EGK_EffectSlot : uint8 
{
    // Default Slot, for easier edit in editor
    None,

    TargetEffect,   // Effects applied to the target
                    // In the case of a projectile, the projectile will apply the effects
                    // to the hit

    CasterEffect,   // Effects applied to the Caster 


    PassiveEffect,  // Effects that are applied once the ability is learnt
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

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName EffectName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<float> Curve;

    // Gameplay effect have their own curve data
    // but we want it to use our curve, but the GA system does not like that much 
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UGameplayEffect> GameplayEffectClass;
};


USTRUCT(BlueprintType)
struct GAMEKIT_API FGKAbilityEffects
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FGKAbilityEffect> Effects;
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

    //! use to disable auto generation on a per ability basis
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Common)
    bool bAutoGenerate;

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

    //! The ability activate itself after the end of the animation
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    bool bLoop;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    TArray<int32> MaxStackCount;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    FGameplayTag StackTag;

    //! Specifies how the ability is activated
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    EGK_ActivationBehavior AbilityBehavior;

    //! Duration the ability linger after being cast
    //! used for damage overtime abilities or active buffs
    //! Should be a TArray | Ability Level can change this
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    float Duration;

    //! Customize the ability effect
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    TMap<EGK_EffectSlot, FGKAbilityEffects> AbilityEffects;

    //! Specify the cost of this ability (mana, energy, etc...)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    FGKAbilityCost Cost;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    FGameplayTagContainer CooldownTags;

    //! Cooldown curve
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability)
    TArray<float> Cooldown;

    //! Price in game
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
    int32 Price;

    //! Slot ID, this is used to define equipment slots
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
    EGK_ItemSlot Slot;

    //! If AbilityKind == Item, specifies the max count of the given
    //! item in a single stack
    //! Should be a TArray | Ability Level can change this
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
    int32 MaxStack;

    //! If true, the target can be selected while out of range
    //! the players will start moving toward the enemy to reach the
    //! minimal rang, if false the target will not be able to be selected
    //! and the user will have to get closer to the target to cast the spell
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Target)
    bool bIgnoreRange;

    // Should be a TArray | Ability Level can change this
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Target)
    float CastMaxRange;

    // Should be a TArray | Ability Level can change this
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Target)
    float CastMinRange;

    //! Required targets attitue to the spell caster
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Target, meta = (Bitmask, BitmaskEnum = "ETeamAttitude"))
    int32 TargetActorFaction;

    //! Class of actors that is an acceptable target for this ability
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Target)
    TSubclassOf<AActor> TargetFilterClass;

    //! The AbilityTarget Actor this ability will use to select a target
    //! The actor spawn is cached inside the Ability System Component
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Target)
    TSubclassOf<AGameplayAbilityTargetActor> AbilityTargetActorClass;

    //! Used to know when a valid target is detected
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Target)
    TArray<TEnumAsByte<EObjectTypeQuery>> TargetObjectTypes;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Target)
    EGK_TargetMode TargetMode;

    //! Time this ability needs before it is cast
    //! Player can be interrupted during that time
    //! Should be a TArray | Ability Level can change this
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
    float CastTime;

    //! Only usefull when AbilityBehavior is set to channel
    //! Should be a TArray | Ability Level can change this
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

    //! Actor that is spawn on impact
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AreaOfEffect);
    TSubclassOf<AActor> ImpactActorClass;

    //! Area of effect used by the ImpactActor
    //! Should be a TArray | Ability Level can change this
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AreaOfEffect)
    float AreaOfEffect;
};
