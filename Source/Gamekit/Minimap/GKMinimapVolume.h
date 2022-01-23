// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#pragma once

#include "Gamekit.h"
#include "CoreMinimal.h"
#include "Runtime/Core/Public/HAL/ThreadingBase.h"
#include "GameFramework/Volume.h"

#include "GKMinimapVolume.generated.h"

/*! Minimap Volume is a static volume for RTS like games
 * 
 * Notes
 * -----
 * 
 * This is not Blueprintable, the blueprint version does not work
 * as expected, the Brush size is incorrect.
 */
UCLASS()
class GAMEKIT_API AGKMinimapVolume : public AVolume
{
	GENERATED_BODY()

public:
    AGKMinimapVolume();

    void BeginPlay() override;

    // This generate a separate texture from the Fog Of War Volume
    // You can combine them using a material 

    //! Represents how often the Minimap is redrawn
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Minimap)
    float FramePerSeconds;

    // Texture used to render component on the minimap
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Minimap)
    class UCanvasRenderTarget2D* MinimapCanvas;

    // Texture used to render the minimap capture from the scene
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Minimap)
    class UCanvasRenderTarget2D* MinimapCaptureTexture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Minimap)
    bool bMinimapEnabled;

public:
    void RegisterActorComponent(class UGKMinimapComponent* c);

    void UnregisterActorComponent(class UGKMinimapComponent* c);

    void DrawMinimap();

    void DrawActorCompoment(class UGKMinimapComponent* Compoment, class UCanvas* Canvas);

    void UpdateSizes();

    //! Returns the texture coordinate given world coordinates
    UFUNCTION(BlueprintPure, Category = Coordinate)
    inline FVector2D GetTextureCoordinate(FVector loc) const {
        return FVector2D(
            (loc.X / MapSize.X + 0.5),
            (0.5 - loc.Y / MapSize.Y)
        );
    }

    UFUNCTION(BlueprintPure, Category = Coordinate)
    inline FVector2D GetScreenCoordinate(FVector loc) const {
        //
        return GetTextureCoordinate(loc) * GetTextureSize();
    }

    UFUNCTION(BlueprintPure, Category = Coordinate)
    inline FVector2D GetMapSize() const {
        //
        return MapSize;
    }

    UFUNCTION(BlueprintPure, Category = Coordinate)
    FVector2D GetTextureSize() const;

    // Set your landscape actor here
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Minimap")
	TArray<AActor*> ShowOnlyActors;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
    class USceneCaptureComponent2D* MinimapCapture;

private:
    FCriticalSection Mutex;               // Mutex to sync adding/removing components with the fog compute
    FTimerHandle     MinimapComputeTimer; // Compute the fog every few frames (or so)
    FVector2D        MapSize;  
    TArray<class UGKMinimapComponent*> ActorComponents;
};
