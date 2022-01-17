// BSD 3-Clause License
//
// Copyright (c) 2019, Pierre Delaunay
// All rights reserved.

#include "Controllers/GKTacticianController.h"
#include "Characters/GKTopDownPawn.h"
#include "Characters/GKUnitCharacter.h"
#include "Grid/GKHexGridUtilities.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Engine/World.h"
#include "GameFramework/SpringArmComponent.h"


AGKTacticianController::AGKTacticianController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void AGKTacticianController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void AGKTacticianController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAxis("ZoomView", this, &AGKTacticianController::OnCameraZoom);
	InputComponent->BindAction("Select", IE_Released, this, &AGKTacticianController::OnSelect);
}

void AGKTacticianController::OnCameraZoom(float delta){
	auto pawn = Cast<AGKTopDownPawn>(GetPawn());
	pawn->ZoomCamera(delta);
}

void AGKTacticianController::OnSelect(){
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Pawn, false, Hit);

	if (SelectedUnit != nullptr && Hit.bBlockingHit){
		auto worldPos = Hit.ImpactPoint;

		if (GetPawn() != nullptr) { /*
            auto size    = Cast<AGKTopDownPawn>(GetPawn())->GridSize;
			auto gridPos = UGKHexGridUtilities::WorldToGrid(size, Hit.ImpactPoint);
			worldPos = UGKHexGridUtilities::GridToWorld(size, gridPos);
            */
		}

		UE_LOG(LogTemp, Warning, TEXT("Moving unit to %f x %f"), worldPos.X, worldPos.Y);
		SelectedUnit->MoveUnit(worldPos);
	}

	if (!Hit.Actor.IsValid()){
		SetSelectedUnit(nullptr);
		UE_LOG(LogTemp, Warning, TEXT("Selection is invalid"));
		return;
	}

	SetSelectedUnit(Cast<AGKUnitCharacter>(Hit.Actor));
}

void AGKTacticianController::SetSelectedUnit(AGKUnitCharacter* unit){
	if (unit != nullptr){
		UE_LOG(LogTemp, Warning, TEXT("Selected unit"));
		SelectedUnit = unit;
		OnUnitSelection();
	} else if (SelectedUnit != nullptr) {
		SelectedUnit = unit;
		UE_LOG(LogTemp, Warning, TEXT("Unselect unit"));
		OnUnitUnselect();
	}
}

AGKUnitCharacter* AGKTacticianController::GetSelectedUnit() {
	return SelectedUnit;
}

void AGKTacticianController::OnUnitSelection_Implementation() {

}

void AGKTacticianController::OnUnitUnselect_Implementation() {

}
