// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "GKGameMode.h"

// Gamekit
#include "Gamekit/Controllers/GKPlayerController.h"
#include "Gamekit/Experimental/GKGameState.h"
#include "Gamekit/GKLog.h"
#include "Gamekit/Blueprint/GKUtilityLibrary.h"
#include "Gamekit/GKTeamPlayerStart.h"

// Unreal Engine
#include "GameFramework/PlayerStart.h"
#include "Engine/PlayerStartPIE.h"
#include "EngineUtils.h"


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

APawn* AGKGameModeBaseBase::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
    auto PlayerStart = Cast<AGKTeamPlayerStart>(StartSpot);

    auto Actor = AGameModeBase::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);

    auto TeamActor = Cast<IGenericTeamAgentInterface>(Actor);

    if (PlayerStart && TeamActor)
    {
        GK_WARNING(TEXT("Setting %s"), *PlayerStart->TeamName.ToString());
        auto TeamId = UGKUtilityLibrary::GetTeamFromName(GetWorld(), PlayerStart->TeamName);
        TeamActor->SetGenericTeamId(TeamId);
    }
    else
    {
        if (!PlayerStart)
            GK_WARNING(TEXT("Player start was not a TeamPlayer start"));

        if (!TeamActor)
            GK_WARNING(TEXT("Player Pawn does not implement IGenericTeamAgentInterface"));
    }

    return Actor;
}