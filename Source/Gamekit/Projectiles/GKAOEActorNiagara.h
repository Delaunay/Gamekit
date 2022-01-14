// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Gamekit/Projectiles/GKAOEActor.h"
#include "GKAOEActorNiagara.generated.h"

/**
 * 
 */
UCLASS()
class GAMEKIT_API AGKAOEActorNiagara : public AGKAOEActor
{
	GENERATED_BODY()

public:
	AGKAOEActorNiagara();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly);
	class UNiagaraComponent* NiagaraEffect;
};
