// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Experimental/GKGameResource.h"

// Sets default values
AGKGameResource::AGKGameResource()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    auto scene    = CreateDefaultSubobject<USceneComponent>("Root");
    RootComponent = scene;

    ResourceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ResourceMesh"));
    ResourceMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AGKGameResource::BeginPlay() { Super::BeginPlay(); }

// Called every frame
void AGKGameResource::Tick(float DeltaTime) { Super::Tick(DeltaTime); }
