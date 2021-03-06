// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Unreal Engine
#include "AbilitySystemGlobals.h"
#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

// Generated
#include "GKAbilitySystemGlobals.generated.h"

// Those are defined as native gameplay tag so they will always be available when gamekit is used
// 
// The gameplay ability plugin makes those configurable, but in our case we rely on quite a bit of them
// and we do not want user to spend too much time figuring out how to set this up
// So it is better experience to the user to just define them and allow them to build on top of them later on
// 
// clang-format off
#define GKTAGS(GKTAG)\
    GKTAG(AbilityExclusive, "Ability.Exclusive",        TEXT("Used to tag abilities that cannot be casted during the activation of another")) \
    GKTAG(CooldownParent,   "Ability.Cooldown",         TEXT("Used for setbycaller GE")) \
    GKTAG(CostParent,       "Ability.Cost",             TEXT("Used for setbycaller GE")) \
    GKTAG(AbilityMove,      "Ability.Move",             TEXT("Tag movement abilities")) \
    GKTAG(AnimationCastPoint,   "Animation.CastPoint",  TEXT("Used to tag the moment inside an animation sequence when the ability should apply its effects")) \
    GKTAG(Dispel,           "Dispel",                   TEXT("Removes malus effect")) \
    GKTAG(DispelDebuff,     "Dispel.Debuff",            TEXT("Remove all debuffs with level equal or lower")) \
    GKTAG(DispelDisable,    "Dispel.Disable",           TEXT("Remove all disables with level equal or lower")) \
    GKTAG(DispelMinor,      "Dispel.Level.Minor",       TEXT("Dispel level")) \
    GKTAG(DispelMedium,     "Dispel.Level.Medium",      TEXT("Dispel level")) \
    GKTAG(DispelStrong,     "Dispel.Level.Strong",      TEXT("Dispel level")) \
    GKTAG(Buff,                 "Buff",                 TEXT("")) \
    GKTAG(BufffMinor,           "Buff.Level.Minor",     TEXT("Buff level")) \
    GKTAG(BuffMedium,           "Buff.Level.Medium",    TEXT("Buff level")) \
    GKTAG(BuffStrong,           "Buff.Level.Strong",    TEXT("Buff level")) \
    GKTAG(BuffImmunity,         "Buff.Immunity",        TEXT("Grant immunity to debuffs and disables")) \
    GKTAG(BuffDebuffImmunity,   "Buff.Immunity.Debuff", TEXT("Grant immunity to debuffs")) \
    GKTAG(BuffDisableImmunity,  "Buff.Immunity.Disable",TEXT("Grant immunity to disables")) \
    GKTAG(Disable,          "Disable",                  TEXT("")) \
    GKTAG(DisableMinor,     "Disable.Level.Minor",      TEXT("Disable level")) \
    GKTAG(DisableMedium,    "Disable.Level.Medium",     TEXT("Disable level")) \
    GKTAG(DisableStrong,    "Disable.Level.Strong",     TEXT("Disable level")) \
    GKTAG(DisableStun,      "Disable.Stun",             TEXT("Disable the character completly (no spells, no items, no movements)")) \
    GKTAG(DisableSilence,   "Disable.Silence",          TEXT("Stops current spells and prevent future spell casting")) \
    GKTAG(DisableRoot,      "Disable.Root",             TEXT("Stop all movements and prevent future movements")) \
    GKTAG(DisableBreak,     "Disable.Break",            TEXT("Stop all passive abilities")) \
    GKTAG(DisableMute,      "Disable.Mute",             TEXT("Stop activating items")) \
    GKTAG(DisableDisarmed,  "Disable.Disarmed",         TEXT("Cannot perform physical attacks")) \
    GKTAG(DisableEtheral,   "Disable.Etheral",          TEXT("Immune to physical attacks but cannot do physical attacks")) \
    GKTAG(DisableDead,      "Disable.Dead",             TEXT("")) \
    GKTAG(Debuff,               "Debuff",               TEXT("")) \
    GKTAG(DebuffMinor,          "Debuff.Level.Minor",   TEXT("Debuff level")) \
    GKTAG(DebuffMedium,         "Debuff.Level.Medium",  TEXT("Debuff level")) \
    GKTAG(DebuffStrong,         "Debuff.Level.Strong",  TEXT("Debuff level")) \
    GKTAG(FailureBlocked,   "Failure.Blocked",          TEXT("Ability is blocked by a Tag")) \
    GKTAG(FailureCooldown,  "Failure.Cooldown",         TEXT("Ability is not ready yet")) \
    GKTAG(FailureDead,      "Failure.Dead",             TEXT("Character is dead")) \
    GKTAG(FailureCost,      "Failure.Cost",             TEXT("Ability cost is not met")) \
    GKTAG(FailureCharge,    "Failure.Charge",           TEXT("Ability has no charge available")) \
    GKTAG(FailureMissing,   "Failure.Missing",          TEXT("Ability requirements are not met")) \
    GKTAG(FailureNetwork,   "Failure.Network",          TEXT("Internal error")) \
    GKTAG(FailureNotLearned,"Failure.NotLearned",       TEXT("Ability was not learned"))
    
    
// clang-format on

#define GKTAG(Name, Tag, Comment) extern FNativeGameplayTag Name;
    GKTAGS(GKTAG)
#undef GKTAG

// #define GKTAG(Name, _) FGameplayTag Get ## Name();
// GKTAGS(GKTAG);
// #undef GKTAG

/**
 *
 */
UCLASS(config = Game)
class GAMEKIT_API UGKAbilitySystemGlobals: public UAbilitySystemGlobals
{
    GENERATED_UCLASS_BODY()
    public:

    static UGKAbilitySystemGlobals& GetGlobals();

    virtual void InitGlobalTags() override;

    //
    // THINK: We could a secondary global curve here for our curve generation system
    //

    // UPROPERTY(config)
    // FSoftClassPath AbilitySystemGlobalsClassName;
};


