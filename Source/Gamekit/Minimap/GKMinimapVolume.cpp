// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#include "Minimap/GKMinimapVolume.h"
#include "Minimap/GKMinimapComponent.h"

#include "TimerManager.h"
#include "Components/BrushComponent.h"
#include "Engine/Canvas.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialInstanceDynamic.h"


AGKMinimapVolume::AGKMinimapVolume() {
}

void AGKMinimapVolume::RegisterActorComponent(UGKMinimapComponent* c) {
    FScopeLock ScopeLock(&Mutex);
    ActorComponents.Add(c);
}

void AGKMinimapVolume::UnregisterActorComponent(UGKMinimapComponent* c) {
    FScopeLock ScopeLock(&Mutex);
    ActorComponents.Remove(c);
}

void AGKMinimapVolume::BeginPlay() {
    Super::BeginPlay();
}