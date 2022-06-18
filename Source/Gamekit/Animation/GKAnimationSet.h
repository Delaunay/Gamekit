// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Gamekit.h"

// Unreal Engine
#include "Engine/DataTable.h"
#include "Kismet/KismetMathLibrary.h"

// Generated
#include "GKAnimationSet.generated.h"

UENUM(BlueprintType)
enum class EGK_AbilityAnimation : uint8
{
    Hidden  UMETA(DisplayName = "Hidden"), 
    Channel UMETA(DisplayName = "Channel"),
    Attack  UMETA(DisplayName = "Attack"),
    Cast    UMETA(DisplayName = "Cast"),
    Death   UMETA(DisplayName = "Death"),
};

USTRUCT(BlueprintType)
struct GAMEKIT_API FGKAnimationArray
{
    GENERATED_USTRUCT_BODY()

    public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<class UAnimMontage *> Animations;

    class UAnimMontage *Sample() const
    {
        if (Animations.Num() <= 0)
        {
            return nullptr;
        }

        return Animations[UKismetMathLibrary::RandomInteger(Animations.Num())];
    }
};

USTRUCT(BlueprintType)
struct GAMEKIT_API FGKAnimationSet: public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    public:
    //! Safe way to query the Animation set, should always return a valid animation
    FGKAnimationArray const &GetAnimations(EGK_AbilityAnimation AnimKind) const
    {
        auto Result = Animations.Find(AnimKind);

        if (Result)
        {
            return *Result;
        }

        return DefaultAnimations;
    }

    class UAnimMontage *Sample(EGK_AbilityAnimation AnimKind) const
    {
        //
        return GetAnimations(AnimKind).Sample();
    }

    public:
    //! Set of animation that this character/skeleton supports
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TMap<EGK_AbilityAnimation, FGKAnimationArray> Animations;

    //! Fall back animation if the character/skeleton does not support the requested animations
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGKAnimationArray DefaultAnimations;
};
