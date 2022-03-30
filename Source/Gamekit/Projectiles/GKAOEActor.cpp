// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Projectiles/GKAOEActor.h"

// Sets default values
AGKAOEActor::AGKAOEActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGKAOEActor::BeginPlay() { Super::BeginPlay(); }

// Called every frame
void AGKAOEActor::Tick(float DeltaTime) { Super::Tick(DeltaTime); }
