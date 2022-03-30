// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Projectiles/GKAbilityEffectActor.h"

// Unreal Engine
#include "CoreMinimal.h"

// Generated
#include "GKAOEActor.generated.h"

UCLASS()
class GAMEKIT_API AGKAOEActor: public AGKAbilityEffectActor
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
