// BSD 3-Clause License Copyright (c) 2021, Pierre Delaunay All rights reserved.

#include "GKPlayerController.h"

#include "Characters/GKCharacter.h"
#include "GKGameInstance.h"
#include "GKSaveGame.h"
#include "Items/GKItem.h"

#include "Engine/NetDriver.h"
#include "GameFramework/GameNetworkManager.h"


AGKPlayerController::AGKPlayerController() {
}


const TArray<FInputActionKeyMapping>& AGKPlayerController::GetKeysForAction(const FName ActionName) {
	return PlayerInput->GetKeysForAction(ActionName);
}

void AGKPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AGKPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	AGKCharacterBase* CharacterBase = Cast<AGKCharacterBase>(P);
	if (CharacterBase)
	{
		CharacterBase->GetAbilitySystemComponent()->InitAbilityActorInfo(CharacterBase, CharacterBase);
	}
}


void AGKPlayerController::GetNetworkMetrics() { 
	auto NetDriver = GetWorld()->GetNetDriver();
	// NetDriver->DrawNetDriverDebug

}
