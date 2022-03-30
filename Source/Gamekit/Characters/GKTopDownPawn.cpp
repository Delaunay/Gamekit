// BSD 3-Clause License
//
// Copyright (c) 2022, Pierre Delaunay
// All rights reserved.

#include "Gamekit/Characters/GKTopDownPawn.h"

// Gamekit
#include "Gamekit/Characters/GKUnitCharacter.h"
#include "Gamekit/Grid/GKHexGridUtilities.h"

// Unreal Engine
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialParameterCollection.h"
#include "Math/UnrealMathVectorCommon.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AGKTopDownPawn::AGKTopDownPawn()
{
    RootScene     = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootScene"));
    RootComponent = RootScene;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootScene);
    CameraBoom->bDoCollisionTest = false; // dont colide with things on the scene
    CameraBoom->TargetArmLength  = 800.f; // 8 meters
    CameraBoom->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComponent->SetupAttachment(CameraBoom);

    Margin      = 50;
    MinCamSpeed = 50;
    MaxCamSpeed = 100;
    ZoomSpeed   = 10;
    ZoomMin     = 10;
    ZoomMax     = 2000;

    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick          = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
}

void AGKTopDownPawn::ZoomCamera(float delta)
{
    if (CameraBoom != nullptr)
    {
        auto newZoom                = CameraBoom->TargetArmLength - delta * ZoomSpeed;
        CameraBoom->TargetArmLength = FMath::Clamp(newZoom, ZoomMin, ZoomMax);
    }
}

// Called when the game starts or when spawned
void AGKTopDownPawn::BeginPlay()
{
    Super::BeginPlay();
    PlayerController = Cast<APlayerController>(GetController());
}

FVector ones = FVector(1.f, 1.f, 1.f);

// returns -1, 0 or 1 depending on the sign of the number
FVector sign(FVector v)
{
    return FVector((v.X > 0.f) - (v.X < 0.f), (v.Y > 0.f) - (v.Y < 0.f), (v.Z > 0.f) - (v.Z < 0.f));
}

// Called every frame
void AGKTopDownPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // EdgePan: Move the Camera
    auto dir = GetCameraPanDirection();
    AddActorWorldOffset(sign(dir) * MinCamSpeed + (MaxCamSpeed - MinCamSpeed) * dir);
}

// Called to bind functionality to input
void AGKTopDownPawn::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

FVector AGKTopDownPawn::GetCameraPanDirection()
{
    FVector CamDir(0.f, 0.f, 0.f);

    if (PlayerController == nullptr)
        return CamDir;

    int32 vsizeX;
    int32 vsizeY;

    float x      = 0;
    float y      = 0;
    float offset = 0;

    PlayerController->GetViewportSize(vsizeX, vsizeY);
    PlayerController->GetMousePosition(x, y);

    float xLeft    = -1 * float(x < Margin) * (Margin - x) / Margin;
    float yForward = +1 * float(y < Margin) * (Margin - y) / Margin;

    offset       = vsizeX - Margin;
    float xRight = +1 * float(x > offset) * (x - offset) / Margin;

    offset      = vsizeY - Margin;
    float yBack = -1 * float(y > offset) * (y - offset) / Margin;

    CamDir.X = yForward + yBack;
    CamDir.Y = xLeft + xRight;

    return CamDir;
}
