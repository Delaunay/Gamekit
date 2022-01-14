// BSD 3-Clause License
//
// Copyright (c) 2019, Pierre Delaunay
// All rights reserved.

#pragma once

#include "Gamekit.h"
#include "GameFramework/Pawn.h"
#include "GKDummyPawn.generated.h"

/**
 * A Edge paning camera and a cursor
 * Used when the controller is top-down god-like i.e not attached to a specific character
 */
UCLASS(Blueprintable)
class GAMEKIT_API AGKDummyPawn: public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGKDummyPawn();

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


//	UFUNCTION()
//	void OnScreenGrab();

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

	UPROPERTY()
	class AHUD* PlayerHUD;

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

	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

	UPROPERTY()
	FVector2D GridSize;

private:

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;
};
