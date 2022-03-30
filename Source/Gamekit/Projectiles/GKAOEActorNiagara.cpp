// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Projectiles/GKAOEActorNiagara.h"

// Unreal Engine
#include "NiagaraComponent.h"

AGKAOEActorNiagara::AGKAOEActorNiagara()
{
    NiagaraEffect                            = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraEffect"));
    NiagaraEffect->bWantsInitializeComponent = false;
}

void AGKAOEActorNiagara::BeginPlay()
{
    NiagaraEffect->InitializeComponent();
    Super::BeginPlay();
}