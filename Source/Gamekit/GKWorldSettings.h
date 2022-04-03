// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Unreal Engine
#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "Engine/DataTable.h"
#include "GenericTeamAgentInterface.h"

// Generated
#include "GKWorldSettings.generated.h"

USTRUCT(BlueprintType)
struct GAMEKIT_API FGKTeamInfo: public FTableRowBase
{
    GENERATED_BODY()
    public:

    FGenericTeamId TeamId;

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

    //! Name of the default team to assign actors if not specified
    //! This will also be used to set NoTeam
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Teams")
    FName DefaultTeam;

    //! Fetch Team info using TeamID
    FGKTeamInfo const* GetTeamInfo(int Index) const;

    //! Fetch Team info using 
    FGKTeamInfo const* GetTeamInfoFromName(FName) const;

    //! Build an array of TeamInfo from the DataTable
    void BuildTeamCache() const;

    TArray<FGKTeamInfo *> const &GetTeams() const { 
        BuildTeamCache();
        return TeamCache;
}

private:
    //! List of all the teams in this game
    UPROPERTY(EditDefaultsOnly, Category = "Teams")
    class UDataTable *Teams;

public:
    mutable TArray<FGKTeamInfo*> TeamCache;
};
