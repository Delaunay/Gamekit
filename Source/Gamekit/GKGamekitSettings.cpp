// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

// include
#include "GKGamekitSettings.h"

// Gamekit
#include "Gamekit/Gamekit.h"
#include "Gamekit/GKLog.h"
#include "Gamekit/Abilities/GKAbilitySystemGlobals.h"

// Unreal Engine
#if WITH_EDITOR
#include "Editor.h"
#endif

UGKGamekitSettings* UGKGamekitSettings::Get() {
    return GetMutableDefault<UGKGamekitSettings>();
}


UGKGamekitSettings::UGKGamekitSettings(const FObjectInitializer& ObjectInitializer): 
    Super(ObjectInitializer) 
{
    AbilitySystemGlobalsClassName = UGKAbilitySystemGlobals::StaticClass();
}

void UGKGamekitSettings::InitGlobalData() {
    GetTeamTable();
    GetExperienceLevel();

    // Register for PreloadMap so cleanup can occur on map transitions
    FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UGKGamekitSettings::HandlePreLoadMap);

#if WITH_EDITOR
    // Register in editor for PreBeginPlay so cleanup can occur when we start a PIE session
    if (GIsEditor)
    {
        FEditorDelegates::PreBeginPIE.AddUObject(this, &UGKGamekitSettings::OnPreBeginPIE);


        // Override their class with ours, our setting takes precedence because it is configurable inside the
        // editor itself rather than modifying the config file
        UGKAbilitySystemGlobals::Get().AbilitySystemGlobalsClassName = AbilitySystemGlobalsClassName;
    }
#endif
}

class UDataTable* UGKGamekitSettings::GetTeamTable() {
	if (!Teams && TeamDataTable.IsValid())
	{
		Teams = Cast<UDataTable>(TeamDataTable.TryLoad());
	}
	return Teams;
}

class UDataTable* UGKGamekitSettings::GetExperienceLevel() {
	if (!ExperienceLevels && ExperienceDataTable.IsValid())
	{
		ExperienceLevels = Cast<UDataTable>(ExperienceDataTable.TryLoad());
	}
	return ExperienceLevels;
}

void UGKGamekitSettings::BuildTeamCache() const {
    if (TeamCache.Num() == 0 && Teams) {
        ReadDataTable(Teams, TeamCache);

        for (int i = 0; i < TeamCache.Num(); i++) {
            TeamCache[i]->TeamId = i;
        }
    }

    if (Teams && TeamCache.Num() != Teams->GetRowMap().Num())
    {
        GK_WARNING(TEXT("Team count changed at runtime!"));
    }
}

FGKTeamInfo const* UGKGamekitSettings::GetTeamInfo(int Index) const {
    BuildTeamCache();

    if (Index < TeamCache.Num())
        return TeamCache[Index];

    return nullptr;
}

FGKTeamInfo const* UGKGamekitSettings::GetTeamInfoFromName(FName Name) const {
    BuildTeamCache();

    for (auto const& TeamInfo : TeamCache)
    {
        if (TeamInfo->Name == Name)
        {
            return TeamInfo;
        }
    }
    return nullptr;
}


#if WITH_EDITOR
void UGKGamekitSettings::OnPreBeginPIE(const bool bIsSimulatingInEditor)
{
    ResetCachedData();
}
#endif // WITH_EDITOR

void UGKGamekitSettings::HandlePreLoadMap(const FString& MapName)
{
    ResetCachedData();
}

void UGKGamekitSettings::ResetCachedData()
{

}