// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Projectiles/GKAOEActor.h"

// Unreal Engine
#include "CoreMinimal.h"

// Generated
#include "GKAOEActorNiagara.generated.h"

/**
 *
 */
UCLASS()
class GAMEKIT_API AGKAOEActorNiagara: public AGKAOEActor
{
    GENERATED_BODY()

    public:
    AGKAOEActorNiagara();

    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly);
    class UNiagaraComponent *NiagaraEffect;
};
