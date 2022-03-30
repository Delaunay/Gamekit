// BSD 3-Clause License
//
// Copyright (c) 2022, Pierre Delaunay
// All rights reserved.

#pragma once

// Unreal Engine
#include "CoreMinimal.h"
#include "GameFramework/Character.h"

// Generated
#include "GKUnitCharacter.generated.h"

/** Simple character that is controlled by an AI by default and can be possed by a player
 * Standard Unit in a RTS-like game
 */
UCLASS(Blueprintable)
class GAMEKIT_API AGKUnitCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGKUnitCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable)
	void MoveUnit(FVector dest);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* StatusDisplay;

};
