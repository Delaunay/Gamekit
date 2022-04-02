// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Unreal Engine
#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"

// Generated
#include "GKWorldSettings.generated.h"

USTRUCT(BlueprintType)
struct FGKTeamInfo
{
    GENERATED_BODY()
    public:

    //! Internal Name
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName        Name;

    //! Display name (localized)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText        DisplayName;

    //! Description (localized)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText        Description;

    //! Display color
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor Color;
};


/** Holds all the level specific constants
 *
 */
UCLASS()
class GAMEKIT_API AGKWorldSettings: public AWorldSettings
{
    GENERATED_BODY()

    public:

    // Size of the Map in total in mm (1e3)
    // The map MUST be centered on (0, 0)
    // You can avoid using this if you are using a Fog of War Volume
    // The Volume knows its size and can be fetched using ``GetFogOfWarMapSize``
    // Which simplify your life as you do not have to set the size of everything
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level|Constants")
    FVector2D MapSize;

    //! Enumeration of all the teams
    //! This could be a map or an array
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Teams")
    TArray<FGKTeamInfo> Teams;

    //! Name of the default team to assign actors if not specified
    //! This will also be used to set NoTeam
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Teams")
    FName DefaultTeam;
};
