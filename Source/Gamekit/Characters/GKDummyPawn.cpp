// BSD 3-Clause License
//
// Copyright (c) 2019, Pierre Delaunay
// All rights reserved.

#include "Characters/GKDummyPawn.h"

#include "Characters/GKUnitCharacter.h"
#include "Grid/GKHexGridUtilities.h"

#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/HUD.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathVectorCommon.h"
#include "Components/DecalComponent.h"
#include "Materials/MaterialParameterCollection.h"

// Sets default values
AGKDummyPawn::AGKDummyPawn()
{
	RootScene = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootScene);
	CameraBoom->bDoCollisionTest = false;												// dont colide with things on the scene
	CameraBoom->TargetArmLength = 800.f;												// 8 meters
	CameraBoom->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(CameraBoom);

	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);

	GridSize.X = 100;
	GridSize.Y = 50;

//	static ConstructorHelpers::FObjectFinder<UMaterialParameterCollection> TileParametersRes(TEXT("MaterialParameterCollection'/Game/Tile/TileMatParameters.TileMatParameters'"));
//	if (TileParametersRes.Succeeded()){
//		auto obj = TileParametersRes.Object;
//		auto val = obj->GetVectorParameterByName("Tile Size");
//		if (val == nullptr){
//			UE_LOG(LogTemp, Warning, TEXT("Tile Size not found!"));
//		} else {
//			GridSize.X = val->DefaultValue.R;
//			GridSize.Y = val->DefaultValue.G;
//		}
//	} else {
//		UE_LOG(LogTemp, Warning, TEXT("Tile Parameter Asset not found"));
//	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> DecalMaterialAsset(TEXT("Material'/Gamekit/Tiles/CursorDecal.CursorDecal'"));
	if (DecalMaterialAsset.Succeeded()){
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	} else {
		UE_LOG(LogTemp, Warning, TEXT("Decal Material not found"));
	}
	CursorToWorld->DecalSize = FVector(100.0f, 100.0f, 100.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 30.0f).Quaternion());

	Margin = 50;
	MinCamSpeed = 50;
	MaxCamSpeed = 100;
	ZoomSpeed = 10;
	ZoomMin = 10;
	ZoomMax = 2000;

	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// BindDynamic(this, OnScreenGrab);
}

void AGKDummyPawn::ZoomCamera(float delta){
	if (CameraBoom != nullptr){
		auto newZoom = CameraBoom->TargetArmLength - delta * ZoomSpeed;
		CameraBoom->TargetArmLength = FMath::Clamp(newZoom, ZoomMin, ZoomMax);
	}
}

// Called when the game starts or when spawned
void AGKDummyPawn::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<APlayerController>(GetController());

	auto HUD = UGameplayStatics::GetPlayerController(this, 0)->GetHUD();
	PlayerHUD = Cast<AHUD>(HUD);
}

FVector ones = FVector(1.f, 1.f, 1.f);

// returns -1, 0 or 1 depending on the sign of the number
FVector sign(FVector v) {
	return FVector(
			(v.X > 0.f) - (v.X < 0.f),
			(v.Y > 0.f) - (v.Y < 0.f),
			(v.Z > 0.f) - (v.Z < 0.f));
}

// Called every frame
void AGKDummyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// EdgePan: Move the Camera
	auto dir = GetCameraPanDirection();
	AddActorWorldOffset(sign(dir) * MinCamSpeed + (MaxCamSpeed - MinCamSpeed) * dir);

	// Move our cursor
	if (GetCursorToWorld() == nullptr || PlayerController == nullptr){
		UE_LOG(LogTemp, Warning, TEXT("Controller or Cursor not set"));
		return;
	}

	FHitResult TraceHitResult;
	PlayerController->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
	auto CursorPosition = UGKHexGridUtilities::SnapToGrid(GridSize, TraceHitResult.Location - FVector(0, 0, 1));
	GetCursorToWorld()->SetWorldLocation(CursorPosition);

	// FVector CursorFV = TraceHitResult.ImpactNormal;
	// FRotator CursorR = CursorFV.Rotation();
	// CursorToWorld->SetWorldRotation(CursorR);
}

// Called to bind functionality to input
void AGKDummyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

FVector AGKDummyPawn::GetCameraPanDirection(){
	FVector CamDir(0.f, 0.f, 0.f);

	if (PlayerController == nullptr)
		return CamDir;

	int32 vsizeX;
	int32 vsizeY;

	float x = 0;
	float y = 0;
	float offset = 0;

	PlayerController->GetViewportSize(vsizeX, vsizeY);
	PlayerController->GetMousePosition(x, y);

	float xLeft    = -1 * float(x < Margin) * (Margin - x) / Margin;
	float yForward = +1 * float(y < Margin) * (Margin - y) / Margin;

	offset = vsizeX - Margin;
	float xRight   = +1 * float(x > offset) * (x - offset) / Margin;

	offset = vsizeY - Margin;
	float yBack    = -1 * float(y > offset) * (y - offset) / Margin;

	CamDir.X = yForward + yBack;
	CamDir.Y = xLeft + xRight;

	return CamDir;
}
