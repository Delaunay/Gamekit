// BSD 3-Clause License
//
// Copyright (c) 2022, Pierre Delaunay
// All rights reserved.

#pragma once

// Unreal Engine
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

// Generated
#include "GKTacticianController.generated.h"

UCLASS(Blueprintable)
class AGKTacticianController : public APlayerController
{
	GENERATED_BODY()

public:
	AGKTacticianController();

protected:
	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	UFUNCTION(BlueprintCallable)
	void OnCameraZoom(float delta);

	// UI State
	UFUNCTION(BlueprintCallable)
	void OnSelect();

	UPROPERTY()
	class AGKUnitCharacter* SelectedUnit;

	UFUNCTION(BlueprintCallable)
	void SetSelectedUnit(class AGKUnitCharacter* unit);

	UFUNCTION(BlueprintCallable)
	class AGKUnitCharacter* GetSelectedUnit();

	UFUNCTION(BlueprintNativeEvent, Category = "Selection")
	void OnUnitSelection();

	UFUNCTION(BlueprintNativeEvent, Category = "Selection")
	void OnUnitUnselect();

	void OnUnitSelection_Implementation();

	void OnUnitUnselect_Implementation();
};
