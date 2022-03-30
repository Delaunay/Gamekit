// BSD 3-Clause License
//
// Copyright (c) 2022, Pierre Delaunay
// All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Gamekit.h"

// Unreal Engine
#include "GameFramework/Pawn.h"

// Generated
#include "GKTopDownPawn.generated.h"


/**
 * A Edge paning camera and a cursor
 * Used when the controller is top-down god-like i.e not attached to a specific character
 * 
 * Movement Specific: EdgePan (Cursor to Edges to move), Screen Grab (click & move), Screen WASD
 */
UCLASS(Blueprintable)
class GAMEKIT_API AGKTopDownPawn: public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGKTopDownPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* RootScene;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Returns CameraBoom subobject **/
	UFUNCTION(BlueprintGetter)
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent;

	/** Returns TopDownCameraComponent subobject **/
	UFUNCTION(BlueprintGetter)
	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComponent; }

	UPROPERTY()
	class APlayerController* PlayerController;

	UFUNCTION(BlueprintCallable)
	FVector GetCameraPanDirection();

	UFUNCTION(BlueprintCallable)
	void ZoomCamera(float delta);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float Margin;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float MinCamSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float MaxCamSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float ZoomSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float ZoomMin;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float ZoomMax;
};
