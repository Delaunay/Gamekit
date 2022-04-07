// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

// Gamekit
#include "Gamekit/Controllers/GKTacticianController.h"
#include "Gamekit/Characters/GKTopDownPawn.h"
#include "Gamekit/Characters/GKUnitCharacter.h"
#include "Gamekit/Grid/GKHexGridUtilities.h"

// Unreal Engine
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Engine/World.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"

AGKTacticianController::AGKTacticianController()
{
    bShowMouseCursor   = true;
    DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void AGKTacticianController::PlayerTick(float DeltaTime) { Super::PlayerTick(DeltaTime); }

void AGKTacticianController::SetupInputComponent()
{
    // set up gameplay key bindings
    Super::SetupInputComponent();

    InputComponent->BindAxis("ZoomView", this, &AGKTacticianController::OnCameraZoom);
    InputComponent->BindAction("Select", IE_Released, this, &AGKTacticianController::OnSelect);
}

void AGKTacticianController::OnCameraZoom(float delta)
{
    auto pawn = Cast<AGKTopDownPawn>(GetPawn());
    pawn->ZoomCamera(delta);
}

void AGKTacticianController::OnSelect()
{
    // Trace to see what is under the mouse cursor
    FHitResult Hit;
    GetHitResultUnderCursor(ECC_Pawn, false, Hit);

    if (SelectedUnit != nullptr && Hit.bBlockingHit)
    {
        auto worldPos = Hit.ImpactPoint;

        if (GetPawn() != nullptr)
        { /*
auto size    = Cast<AGKTopDownPawn>(GetPawn())->GridSize;
auto gridPos = UGKHexGridUtilities::WorldToGrid(size, Hit.ImpactPoint);
worldPos = UGKHexGridUtilities::GridToWorld(size, gridPos);
*/
        }

        UE_LOG(LogTemp, Warning, TEXT("Moving unit to %f x %f"), worldPos.X, worldPos.Y);
        SelectedUnit->MoveUnit(worldPos);
    }

    auto HitActor = Hit.HitObjectHandle.FetchActor<AActor>();

    if (!HitActor)
    {
        SetSelectedUnit(nullptr);
        UE_LOG(LogTemp, Warning, TEXT("Selection is invalid"));
        return;
    }

    SetSelectedUnit(Cast<AGKUnitCharacter>(HitActor));
}

void AGKTacticianController::SetSelectedUnit(AGKUnitCharacter *unit)
{
    if (unit != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Selected unit"));
        SelectedUnit = unit;
        OnUnitSelection();
    }
    else if (SelectedUnit != nullptr)
    {
        SelectedUnit = unit;
        UE_LOG(LogTemp, Warning, TEXT("Unselect unit"));
        OnUnitUnselect();
    }
}

AGKUnitCharacter *AGKTacticianController::GetSelectedUnit() { return SelectedUnit; }

void AGKTacticianController::OnUnitSelection_Implementation() {}

void AGKTacticianController::OnUnitUnselect_Implementation() {}
