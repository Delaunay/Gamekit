// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Controllers/GKPlayerController.h"

// Gamekit
#include "Gamekit/Characters/GKCharacter.h"
#include "Gamekit/GKGameInstance.h"
#include "Gamekit/GKSaveGame.h"
#include "Gamekit/Abilities/GKAbilityInterface.h"

// Unreal Engine
#include "Engine/NetDriver.h"
#include "GameFramework/GameNetworkManager.h"
#include "Net/UnrealNetwork.h"

AGKPlayerController::AGKPlayerController() {}

const TArray<FInputActionKeyMapping> &AGKPlayerController::GetKeysForAction(const FName ActionName)
{
    return PlayerInput->GetKeysForAction(ActionName);
}

void AGKPlayerController::BeginPlay() { Super::BeginPlay(); }

void AGKPlayerController::AcknowledgePossession(APawn *P)
{
    Super::AcknowledgePossession(P);

    AGKCharacterBase *CharacterBase = Cast<AGKCharacterBase>(P);
    if (CharacterBase)
    {
        CharacterBase->GetAbilitySystemComponent()->InitAbilityActorInfo(CharacterBase, CharacterBase);
    }

    ReceiveAcknowledgePossession(P);
}

void AGKPlayerController::ServerAcknowledgePossession_Implementation(APawn *P) {
    Super::ServerAcknowledgePossession_Implementation(P);

    // Possessed Pawn get the same TeamId
    auto Agent = Cast<IGenericTeamAgentInterface>(P);
    if (Agent)
    {
        Agent->SetGenericTeamId(GetGenericTeamId());
    }
}

void AGKPlayerController::GetNetworkMetrics()
{
    auto NetDriver = GetWorld()->GetNetDriver();
    // NetDriver->DrawNetDriverDebug
}


void AGKPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AGKPlayerController, TeamId);
}

// IGenericTeamAgentInterface
// --------------------------

void AGKPlayerController::OnRep_TeamChange() { OnTeamChange();}

void AGKPlayerController::OnTeamChange() { 
}

void AGKPlayerController::SetGenericTeamId(const FGenericTeamId& TeamID) { 
    // Only authority can change TeamID
    if (GetNetMode() == ENetMode::NM_Client)
    {
        return;
    }

    if (TeamId == TeamID)
    {
        return;
    }

    TeamId = TeamID;

    // Change Possessed pawn to the new TeamId
    auto Agent = Cast<IGenericTeamAgentInterface>(GetPawn());
    if (Agent)
    {
        Agent->SetGenericTeamId(TeamId);
    }

    //
    OnTeamChange();
}
