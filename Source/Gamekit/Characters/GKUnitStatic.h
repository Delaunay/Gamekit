// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.
#pragma once

// Gamekit
#include "Gamekit/Abilities/GKAbilityInputs.h"
#include "Gamekit/Abilities/GKGameplayAbility.h"
#include "Gamekit/Animation/GKAnimationSet.h"
#include "Gamekit/Gamekit.h"

// Unreal Engine
#include "Engine/DataTable.h"

// Generated
#include "GKUnitStatic.generated.h"

/**
 * Character/class specific data
 */
USTRUCT(BlueprintType)
struct GAMEKIT_API FGKUnitStatic: public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    public:
    FGKUnitStatic();

    //! User-visible short name
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName Name;

    //! Localized name for display
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText LocalName;

    //! Localized description for display
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText LocalDescription;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<TSubclassOf<UGKGameplayAbility>> Abilities;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BaseAttributes)
    float AttackRate;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BaseAttributes)
    float AttackAnimationPoint;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BaseAttributes)
    float AttackRange;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BaseAttributes)
    float Damage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BaseAttributes)
    float MovementSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BaseAttributes)
    float Health;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BaseAttributes)
    float HealthRegen;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BaseAttributes)
    float Mana;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BaseAttributes)
    float ManaRegen;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
    FGKAnimationSet AnimationSet;

    //! Icon to display
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Common)
    class UTexture2D *Icon;
};

USTRUCT(BlueprintType)
struct GAMEKIT_API FGKInputAbilityPair
{
    GENERATED_USTRUCT_BODY()

    public:
    FGKInputAbilityPair(): InputSlot(EGK_MOBA_AbilityInputID::None) {}

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EGK_MOBA_AbilityInputID InputSlot;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UGKGameplayAbility> AbilityClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int DefaultLevel;
};
