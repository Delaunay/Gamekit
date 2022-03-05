// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#pragma once

#include "Gamekit.h"
#include "CoreMinimal.h"
#include "Runtime/Core/Public/HAL/ThreadingBase.h"
#include "GameFramework/Volume.h"

#include "GKMinimapVolume.generated.h"


struct FGKFactionMinimap
{
    FGKFactionMinimap()
    {
        MinimapCapture = nullptr;
        MinimapCanvas  = nullptr;
        Allies.Reserve(128);
    }

    FName                                Name;
    class UTexture *                     MinimapCapture;
    class UTexture *                     MinimapCanvas;
    bool                                 bDiscrete;
    TArray<class UGKFogOfWarComponent *> Allies;
};

/*! Minimap Volume is a static volume for RTS like games
 * 
 * \rst
 * 
 * .. note::
 *    
 *    This is not Blueprintable, the blueprint version does not work
 *    as expected, the Brush size is incorrect.
 * 
 * .. note::
 * 
 *    This is the static version that works on a entire fixed map
 * 
 * \endrst
 * 
 * TODO: create a radar version that follows the actor
 */
UCLASS()
class GAMEKIT_API AGKMinimapVolume : public AVolume
{
	GENERATED_BODY()

public:
    AGKMinimapVolume();

    void BeginPlay() override;

    void Tick(float Delta) override;

    // This generate a separate texture from the Fog Of War Volume
    // You can combine them using a material 

    //! Represents how often the Minimap is redrawn
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap|Async")
    float FramePerSeconds;

    //! Texture used to render component on the minimap
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Minimap)
    class UCanvasRenderTarget2D* MinimapCanvas;

    //! Texture used to render the minimap capture from the scene
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Minimap)
    class UCanvasRenderTarget2D* MinimapCaptureTexture;

    //! Toggle to disable Minimap drawing altogether
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Minimap)
    bool bMinimapEnabled;

    //! Bit of a work around since we cannot modify the location of a C++ component
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Minimap)
    FVector CameraPosition;

public:
    //! Called by UGKMinimapComponent to register themselves
    void RegisterActorComponent(class UGKMinimapComponent* c);

    //! Called by UGKMinimapComponent to unregister themselves
    void UnregisterActorComponent(class UGKMinimapComponent* c);

    //! Iterate through all the actor component and draw each component 
    //! on the minimap
    void DrawMinimap();

    //! Draw a single component on the given canvas
    void DrawActorCompoment(class UGKMinimapComponent* Compoment, class UCanvas* Canvas);

    //! Fetch the Volume size and update MapSize
    //! It will update the capture component with the new size
    //! sets its texture target and ShowOnlyActors as well
    void UpdateSizes();

    //! Returns the texture (UV) coordinate given a world coordinates
    UFUNCTION(BlueprintPure, Category = Coordinate)
    inline FVector2D GetTextureCoordinate(FVector loc) const {
        return FVector2D(
            (loc.X / MapSize.X + 0.5),
            (0.5 - loc.Y / MapSize.Y)
        );
    }

    //! Returns the screen (Pixel) coordinate given a world coordinates
    UFUNCTION(BlueprintPure, Category = Coordinate)
    inline FVector2D GetScreenCoordinate(FVector loc) const {
        return GetTextureCoordinate(loc) * GetTextureSize();
    }

    //! Returns the volume size
    UFUNCTION(BlueprintPure, Category = Coordinate)
    inline FVector2D GetMapSize() const {
        return MapSize;
    }

    //! Returns the size of the texture we are writing to
    UFUNCTION(BlueprintPure, Category = Coordinate)
    FVector2D GetTextureSize() const;

    //! List of actors that are rendered on the minimap
    //! Your landscape should be there
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Minimap")
	TArray<AActor*> ShowOnlyActors;

    //! The scene capture component used to generate the minimap
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minimap")
    class USceneCaptureComponent2D* MinimapCapture;

    // Fetch all the actors of a given class and add them to our allow list
    // Defaults to ALandscape
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Minimap")
    TSubclassOf<AActor> AllowClass;

private:
    TMap<FName, FGKFactionMinimap> FactionMinimap;

    FCriticalSection                    Mutex; // Mutex to sync adding/removing components with the fog compute
    FVector2D                           MapSize;
    TArray<class UGKMinimapComponent *> ActorComponents;
};
