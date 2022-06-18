// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "GKGameMode.h"

// Gamekit
#include "Gamekit/Controllers/GKPlayerController.h"
#include "Gamekit/States/GKGameState.h"
#include "Gamekit/GKLog.h"
#include "Gamekit/Blueprint/GKUtilityLibrary.h"
#include "Gamekit/GKTeamPlayerStart.h"

// Unreal Engine
#include "GameFramework/PlayerStart.h"
#include "Engine/PlayerStartPIE.h"
#include "EngineUtils.h"
#include "Dom/JsonValue.h"


AGKGameModeBaseBase::AGKGameModeBaseBase()
{
    GameStateClass        = AGKGameStateBase::StaticClass();
    PlayerControllerClass = AGKPlayerController::StaticClass();
    bGameOver             = false;
}

void AGKGameModeBaseBase::ResetLevel() { K2_DoRestart(); }

bool AGKGameModeBaseBase::HasMatchEnded() const { return bGameOver; }

void AGKGameModeBaseBase::GameOver()
{
    if (bGameOver == false)
    {
        K2_OnGameOver();
        bGameOver = true;
    }
}

FString AGKGameModeBaseBase::InitNewPlayer(
    APlayerController * NewPlayerController,
    const FUniqueNetIdRepl & UniqueId,
    const FString & Options,
    const FString & Portal)
{

    auto NetMode = UGKUtilityLibrary::NetModeToString(GetNetMode());

    GK_DISPLAY(TEXT("NetMode  %s"), *NetMode.ToString());
    GK_DISPLAY(TEXT(" UniqueId %s"), *UniqueId.ToJson().Get().AsString());
    GK_DISPLAY(TEXT(" Options  %s"), *Options);
    GK_DISPLAY(TEXT(" Portal   %s"), *Portal);

    auto ErrorMessage = AGameModeBase::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
    
    GK_DISPLAY(TEXT(" Ret      %s"), *ErrorMessage);
    return ErrorMessage;
}

void AGKGameModeBaseBase::SetGenericTeamIdFromPlayerStart(AController *NewPlayer, AActor *StartSpot) {
    auto PlayerStart = Cast<AGKTeamPlayerStart>(StartSpot);
    auto TeamActor = Cast<IGenericTeamAgentInterface>(NewPlayer);

    if (PlayerStart && TeamActor)
    {
        auto Settings = UGKGamekitSettings::Get();

        if (!Settings)
        {
            GK_WARNING(TEXT("WorldSetting does not hold Team info"));
            return;
        }
        
        auto TeamInfo = Settings->GetTeamInfoFromName(PlayerStart->TeamName);

        if (!TeamInfo)
        {
            GK_WARNING(TEXT("Could not find Team %s"), *PlayerStart->TeamName.ToString());
            return;
        }
        
         TeamActor->SetGenericTeamId(TeamInfo->TeamId);
    }
    else
    {
        if (!PlayerStart)
            GK_WARNING(TEXT("Player start was not a TeamPlayer start"));

        if (!TeamActor)
            GK_WARNING(TEXT("Player Pawn does not implement IGenericTeamAgentInterface"));
    }
}

APawn* AGKGameModeBaseBase::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
    SetGenericTeamIdFromPlayerStart(NewPlayer, StartSpot);
    return AGameModeBase::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);
}


APawn* AGKGameModeBaseBase::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
    FActorSpawnParameters SpawnInfo;
    SpawnInfo.Instigator = GetInstigator();
    SpawnInfo.ObjectFlags |= RF_Transient;	// We never want to save default player pawns into a map
    SpawnInfo.bDeferConstruction = true;

    UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer);

    APawn* ResultPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo);

    // Populate replicated data so everything gets populated
    // when the pawn gets sent down to clients
    PawnConstruction(NewPlayer, ResultPawn);

    ResultPawn->FinishSpawning(SpawnTransform);

    if (!ResultPawn)
    {
        UE_LOG(LogGameMode, Warning, TEXT("SpawnDefaultPawnAtTransform: Couldn't spawn Pawn of type %s at %s"), *GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString());
    }
    return ResultPawn;
}

void AGKGameModeBaseBase::PawnConstruction(AController* NewPlayer, APawn* NewPawn) {
    auto TeamActor = Cast<IGenericTeamAgentInterface>(NewPlayer);
    auto PawnActor = Cast<IGenericTeamAgentInterface>(NewPawn);

    if (TeamActor && PawnActor) {
        PawnActor->SetGenericTeamId(TeamActor->GetGenericTeamId());
    }
}
