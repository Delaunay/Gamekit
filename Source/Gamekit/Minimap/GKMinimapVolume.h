// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#pragma once

#include "Gamekit.h"
#include "CoreMinimal.h"
#include "Runtime/Core/Public/HAL/ThreadingBase.h"
#include "GameFramework/Volume.h"

#include "GKMinimapVolume.generated.h"

/*!
 */
UCLASS(Blueprintable)
class GAMEKIT_API AGKMinimapVolume : public AVolume
{
	GENERATED_BODY()

public:
    AGKMinimapVolume();

    void BeginPlay();

    //! Represents how often the fog is updated
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Minimap)
    float FramePerSeconds;

    // Texture used to render component on the minimap
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Minimap)
    class UCanvasRenderTarget2D* Minimap;

public:
    void RegisterActorComponent(class UGKMinimapComponent* c);

    void UnregisterActorComponent(class UGKMinimapComponent* c);

private:
    FCriticalSection Mutex;             // Mutex to sync adding/removing components with the fog compute

    TArray<class UGKMinimapComponent*> ActorComponents;
};
