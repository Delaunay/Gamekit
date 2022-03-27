// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.


#include "Projectiles/GKAOEActorNiagara.h"

#include "NiagaraComponent.h"


AGKAOEActorNiagara::AGKAOEActorNiagara() {
    NiagaraEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraEffect"));
    NiagaraEffect->bWantsInitializeComponent = false;
}

void AGKAOEActorNiagara::BeginPlay() {
    NiagaraEffect->InitializeComponent();
    Super::BeginPlay();
}