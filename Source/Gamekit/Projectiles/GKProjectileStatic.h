// BSD 3-Clause License Copyright (c) 2021, Pierre Delaunay All rights reserved.

#pragma once

#include "Gamekit.h"
#include "Engine/DataTable.h"
#include "Styling/SlateBrush.h"

#include "GKProjectileStatic.generated.h"


UENUM(BlueprintType)
enum class EGK_ProjectileBehavior : uint8
{
    UnitTarget  UMETA(DisplayName = "UnitTarget"),  // Target seeking Projectile
    Directional UMETA(DisplayName = "Directional"), // Projectile goes in straight line
};


/*! Holds customization data for projectiles
 */
USTRUCT(BlueprintType)
struct GAMEKIT_API FGKProjectileStatic: public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    public:
    FGKProjectileStatic();

    //! Base projectile speed
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Projectile);
    float BaseSpeed;

    //! Base projectile speed
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Projectile);
    float InitialSpeed;

    //! Homing Acceleration affects how quickly the projectile can turn
    //! towards the target this needs to be at least 2x the max speed
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Projectile);
    float HomingAcceleration;

    //! How the projectlie behave
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Projectile);
    EGK_ProjectileBehavior Behavior;

    //! Max length before the actor destroy itself
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Projectile);
    float Range;
};
