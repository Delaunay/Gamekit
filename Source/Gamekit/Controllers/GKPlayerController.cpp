// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Controllers/GKPlayerController.h"

// Gamekit
#include "Gamekit/Characters/GKCharacter.h"
#include "Gamekit/GKGameInstance.h"
#include "Gamekit/GKSaveGame.h"
#include "Gamekit/Items/GKItem.h"

// Unreal Engine
#include "Engine/NetDriver.h"
#include "GameFramework/GameNetworkManager.h"

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
}

void AGKPlayerController::GetNetworkMetrics()
{
    auto NetDriver = GetWorld()->GetNetDriver();
    // NetDriver->DrawNetDriverDebug
}
