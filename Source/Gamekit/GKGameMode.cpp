// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "GKGameMode.h"
#include "Controllers/GKPlayerController.h"
#include "Experimental/GKGameState.h"

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