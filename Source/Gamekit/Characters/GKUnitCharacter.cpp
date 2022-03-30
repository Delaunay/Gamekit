// BSD 3-Clause License
//
// Copyright (c) 2022, Pierre Delaunay
// All rights reserved.

#include "Gamekit/Characters/GKUnitCharacter.h"

// Gamekit
#include "Gamekit/Controllers/GKUnitController.h"
#include "Gamekit/Grid/GKMovementUtility.h"
#include "Gamekit/Characters/GKTopDownPawn.h"

// Unreal Engine
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Blueprint/BlueprintExtension.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"


// Sets default values
AGKUnitCharacter::AGKUnitCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Each Units have their own controller
	// which holds Action/Stance (attack, hold position, etc...)
	AIControllerClass = AGKUnitAIController::StaticClass();
	// All units have their own AI & players issue commands to Unit AI
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	StatusDisplay = CreateDefaultSubobject<UWidgetComponent>(TEXT("StatusDisplay"));

	// The root Component is only defined here
	// we need to set it to get correct world location
	StatusDisplay->SetupAttachment(GetCapsuleComponent());
}

// Called when the game starts or when spawned
void AGKUnitCharacter::BeginPlay(){
	Super::BeginPlay();
}


void AGKUnitCharacter::Tick(float delta)
{
	Super::Tick(delta);

	// Make the UI follow the player
	// -----------------------------
	// UGameplayStatics::GetPlayerPawn();
	auto pawn = GetWorld()->GetFirstPlayerController()->GetPawn();
    auto tactician = Cast<AGKTopDownPawn>(pawn);

	if (tactician == nullptr){
		UE_LOG(LogTemp, Warning, TEXT("Unit is not owned by a tactician"));
		return;
	}

	auto cameraLoc = tactician->GetCameraComponent()->GetComponentLocation();
	auto widgetLoc = StatusDisplay->GetComponentLocation();
	auto rot = UKismetMathLibrary::FindLookAtRotation(widgetLoc, cameraLoc);
	StatusDisplay->SetWorldRotation(rot);
}


void AGKUnitCharacter::MoveUnit(FVector dest){
	auto controller = GetController();

	if (controller == nullptr){
		UE_LOG(LogTemp, Warning, TEXT("Unit does not have a controller, Abort move"));
		return;
	}

	float const Distance = FVector::Dist(dest, GetActorLocation());

	if (Distance > 100.0f){
		UGKMovementUtility::SimpleMoveToLocationExact(controller, dest, 1);
		// UAIBlueprintHelperLibrary::SimpleMoveToLocation(Controller, dest);
	}
}
