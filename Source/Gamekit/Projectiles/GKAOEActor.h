// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Projectiles/GKAbilityEffectActor.h"

#include "GKAOEActor.generated.h"

UCLASS()
class GAMEKIT_API AGKAOEActor : public AGKAbilityEffectActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGKAOEActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
