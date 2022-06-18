// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

// Generated
#include "GKGamekitSettings.generated.h"



USTRUCT(BlueprintType)
struct GAMEKIT_API FGKTeamInfo : public FTableRowBase
{
    GENERATED_BODY()
public:

    uint8_t      TeamId;

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


template<typename T>
void ReadDataTable(class UDataTable* Table, TArray<T*>& Rows) {
    Rows.Reserve(Table->GetRowMap().Num());

    for (auto RowMapIter(Table->GetRowMap().CreateConstIterator()); RowMapIter; ++RowMapIter)
    {
        T* Info = reinterpret_cast<T*>(RowMapIter.Value());
        Rows.Add(Info);
    }
}

template<typename T>
T* GetRow(class UDataTable* Table, FName Row) {
    static FString Context;
    return Table->FindRow<T>(Row, Context, false);
}


USTRUCT(BlueprintType)
struct GAMEKIT_API FGKExperience : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly);
    int Level;

    UPROPERTY(EditAnywhere, BlueprintReadOnly);
    int TotalExperience;

    UPROPERTY(EditAnywhere, BlueprintReadOnly);
    int AdditionalExperience;

    UPROPERTY(EditAnywhere, BlueprintReadOnly);
    int MaxAbilityLevel;

    UPROPERTY(EditAnywhere, BlueprintReadOnly);
    int MaxUtlimateLevel;
};

/**
 * 
 */
UCLASS(config = Game)
class GAMEKIT_API UGKGamekitSettings : public UObject
{
    GENERATED_UCLASS_BODY()
	
public:

	static UGKGamekitSettings* Get();

	UPROPERTY(EditAnywhere, Config, Category = GameplayAbilitySystem)
	FSoftClassPath AbilitySystemGlobalsClassName;


    // ==================================================
    // Team
    // --------------------------------------------------

    //! Name of the default team to assign actors if not specified
    //! This will also be used to set NoTeam
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Teams")
    FName DefaultTeam;

    //! List of all the teams in this game
    UPROPERTY(EditAnywhere, Config, Category = "Teams")
    FSoftObjectPath TeamDataTable;

    class UDataTable* GetTeamTable();

protected:
    class UDataTable* Teams;

private:
    mutable TArray<struct FGKTeamInfo*> TeamCache;

public:
    //! Fetch Team info using TeamID
    FGKTeamInfo const* GetTeamInfo(int Index) const;

    //! Fetch Team info using
    FGKTeamInfo const* GetTeamInfoFromName(FName Name) const;

    //! Build an array of TeamInfo from the DataTable
    void BuildTeamCache() const;

    TArray<FGKTeamInfo*> const& GetTeams() {
        BuildTeamCache();
        return TeamCache;
    }

public:
    // ==================================================
    // Experience
    // --------------------------------------------------
    //! Table specifying the amount of experience required to reach a given level
    //! Can also be used to add per level contrains
    UPROPERTY(EditAnywhere, Config, Category = "Experience")
    FSoftObjectPath ExperienceDataTable;

    class UDataTable* GetExperienceLevel();

protected:

    class UDataTable* ExperienceLevels;

public:
    // This is used to cache the table locally for fast lookup
    // to avoid the Int -> String -> FName lookup
    // The asc should probably cache this
    // maybe we could move this to the asc global
    // but it does not have a nice Editor widget
    TArray<FGKExperience const*> GetExperienceLevels() {
        TArray<FGKExperience const*> Out;
        ReadDataTable(ExperienceLevels, Out);
        return Out;
    }

    UFUNCTION(BlueprintPure)
        FGKExperience const& GetLevelSpec(int Level) {
        static FGKExperience Nothing;
        FGKExperience const* Result = GetRow<FGKExperience>(ExperienceLevels, FName(FString::FromInt(Level)));

        if (Result != nullptr)
            return *Result;

        return Nothing;
    }

public:
    void InitGlobalData();

private:
#if WITH_EDITOR
    void OnPreBeginPIE(const bool bIsSimulatingInEditor);
#endif // WITH_EDITOR

    void ResetCachedData();

    void HandlePreLoadMap(const FString& MapName);
};
