// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#pragma once

#include "Gamekit.h"

#include "CoreMinimal.h"
#include "Runtime/Core/Public/HAL/ThreadingBase.h"
#include "GameFramework/Volume.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Gamekit/FogOfWar/Strategy/GK_FoW_Strategy.h"
#include "Gamekit/Grid/GKGrid.h"

#include "GKFogOfWarVolume.generated.h"


#define DEFAULT_FoW_COLLISION ECC_GameTraceChannel1

/*! AGKFogOfWarVolume manages fog of war for multiple factions.
 * All units inside the same faction share visions.
 *
 * AGKFogOfWarVolume sets a timers that runs every 1/30 seconds, configurable through AGKFogOfWarVolume::FramePerSeconds
 * which computes the vision from each factions
 *
 * \rst
 * The fog of war can be rendered to the screen using two methods
 *
 * #. **PostProcessing**: which requires you do add the post processing step to all the camera components or add a global post-process volume.
 *    Using a post process material on CameraComponents is recommended as it gives the most flexibility.
 *
 * #. **DecalComponent** which applies a decal material on your entire map.
 *    This approach is not recommended. It is used by the :class:`AGKFogOfWarVolume` to display the fog in editor mode
 *
 * .. warning::
 *
 *    It requires a custom collision channel, set :member:`AGKFogOfWarVolume::FogOfWarCollisionChannel`
 *
 * .. note::
 *
 *    The fog of war will darken your screen, it is advised to disable UE4 default auto exposure using a Post Process Volume and
 *    enabling ``Exposure > Metering Mode > Auto Exposure Basic`` as well as setting min and max brightness to 1
 *
 * \endrst
 * 
 *  TODO: create a radar version that follows the actor.
 *  The actor will have a fog that follows him.
 */
UCLASS(Blueprintable)
class GAMEKIT_API AGKFogOfWarVolume : public AVolume
{
	GENERATED_BODY()

public:
    AGKFogOfWarVolume();

    void BeginPlay();

    //! Returns the render target associated with the faction name
    UFUNCTION(BlueprintCallable, Category = FogOfWar, meta = (AutoCreateRefTerm = "CreateRenderTarget"))
    class UCanvasRenderTarget2D* GetFactionRenderTarget(FName name, bool CreateRenderTarget = true);

    //! Returns the exploration render target associated with the faction name
    UFUNCTION(BlueprintCallable, Category = FogOfWar, meta = (AutoCreateRefTerm = "CreateRenderTarget"))
    class UCanvasRenderTarget2D* GetFactionExplorationRenderTarget(FName name, bool CreateRenderTarget = true);

    //! Collision Channel used to draw the line of sight
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    TEnumAsByte<ECollisionChannel> FogOfWarCollisionChannel;

    //! If true exploration texture will be created
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    bool EnableExploration;

    //! Returns the material parameter collection used to configure the Fog of War
    UFUNCTION(BlueprintCallable, Category = FogOfWar)
    class UMaterialParameterCollection* GetMaterialParameterCollection();

    //! Returns the color of the fog of war
    UFUNCTION(BlueprintCallable, Category = FogOfWar)
    FLinearColor GetColor();

    //! Returns the size of the texture used to draw the fog of war
    UFUNCTION(BlueprintCallable, Category = FogOfWar)
    FLinearColor GetTextureSize();

    //! Set the fog of war color
    UFUNCTION(BlueprintCallable, Category = FogOfWar)
    void SetColor(FLinearColor color);

    //! Returns the size covered by the fog of war (in cm)
    UFUNCTION(BlueprintCallable, Category = FogOfWar)
    FLinearColor GetMapSize();

    //! Register a new actor to the fog of war volume
    void RegisterActorComponent(class UGKFogOfWarComponent* c);

    //! Unregister the actor to the fog of war volume
    void UnregisterActorComponent(class UGKFogOfWarComponent* c);

    //! Draw the fog of war for each factions
    void DrawFactionFog();

    //! Note because the terrain can stop the sight we always will have to cast
    //! rays all around the target
    void DrawObstructedLineOfSight(class UGKFogOfWarComponent *c);
    
    //! Draw the line of sight using LineTrace
    void DrawObstructedLineOfSight_RayCastV1(class UGKFogOfWarComponent* c);

    //! Generates Triangles of vision per actor and draw them
    //! Drawing triangles is more expensive than simple lines, you should lower
    //! the number of trace done by each actors
    //! Even with a low trace count the field of view will still render 
    //! as a circle thanks to its material
    //! 
    void DrawObstructedLineOfSight_RayCastV2(class UGKFogOfWarComponent* c);

    //! V3 find all the obstacle and try to draw more traces neat the bounds
    //! There is a sorting problem, the angles behaves a bit strangely
    //! although they are sorted the triangles are not drawn correctly
    //! 
    //!     * Generate minimum Line traces <----------------------------+
    //!     * Get all obstacle in a radius                              |
    //!     * For each obstacle add 2 traces                            |
    //!     * Add Line traces if angle between 2 traces are too wide <--+
    //!     * Generate triangles
    //!     * Draw
    void DrawObstructedLineOfSight_RayCastV3(UGKFogOfWarComponent *c);

    void DrawObstructedLineOfSight_DiscreteV1(UGKFogOfWarComponent *c);

    //! Draw the ligne of sight using a material (no collision)
    void DrawUnobstructedLineOfSight(class UGKFogOfWarComponent* c);

    //! Draw the line of sight using the right method
    void DrawLineOfSight(class UGKFogOfWarComponent* c);

    //! Returns the texture coordinate given world coordinates
    UFUNCTION(BlueprintCallable, Category = FogOfWar)
    inline FVector2D GetTextureCoordinate(FVector loc) {
        return FVector2D(
            (loc.X / MapSize.X + 0.5) * TextureSize.X,
            (0.5 - loc.Y / MapSize.Y) * TextureSize.Y
        );
    }

    //! Sets the texture parameters FoWView & FoWExploration
    UFUNCTION(BlueprintCallable, Category = FogOfWar, meta = (AutoCreateRefTerm = "CreateRenderTarget"))
    void SetFogOfWarMaterialParameters(FName name, class UMaterialInstanceDynamic* Material, bool CreateRenderTarget = false);

    //! Returns the post process material the actor should use for its camera
    UFUNCTION(BlueprintCallable, Category = FogOfWar, meta = (AutoCreateRefTerm = "CreateRenderTarget"))
    class UMaterialInterface* GetFogOfWarPostprocessMaterial(FName name, bool CreateRenderTarget = false);

    //! TODO: for debugging only
    UFUNCTION(BlueprintCallable, Category = FogOfWar)
    class UTexture2D *GetFactionTexture(FName name);

    //! Represents how often the fog is updated
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    float FramePerSeconds;

    //! Parameter collection used to talk to shader/materials
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    class UMaterialParameterCollection* FogMaterialParameters;

    //! Material used to draw unobstructed vision
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    class UMaterialInterface* UnobstructedVisionMaterial;

    //! Material used to draw unobstructed vision with Triangles
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|RayCast")
    class UMaterialInterface *TrianglesMaterial;

    //! Margin for Actor Rays
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|RayCast|V3")
    float Margin;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|Discrete|V1")
    FGKGrid Grid;

    FIntVector ToGridTexture(FIntVector Pos) {
        return FIntVector(
            Pos.X + TextureSize.X / 2,
            TextureSize.Y / 2 - Pos.Y,
            Pos.Z
        );
    }

    FIntVector FromTextureToGrid(FIntVector Pos) {
        return FIntVector(
            Pos.X -  TextureSize.X / 2,
            TextureSize.Y / 2 - Pos.Y,
            Pos.Z
        );
    }

    //! Base Material used to draw the fog of war in a post process step,
    //! it uses the texture parameters FoWView & FoWExploration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    class UMaterialInterface* BasePostProcessMaterial;

    //! Maps the faction to their exploration texture
    //! This can be manually set or they will be automatically generated at runtime
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    TMap<FName, class UCanvasRenderTarget2D*> FogFactions;

    //! Maps the faction to their fog texture
    //! This can be manually set or they will be automatically generated at runtime
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    TMap<FName, class UCanvasRenderTarget2D*> Explorations;

    //! Used to controlled the size of the underlying texture given the terrain size
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    float TextureScale;

    //! Use Decal FoWDecal rendering
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    bool UseFoWDecalRendering;

    //! Faction that is displayed by the client
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    FName Faction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    class UDecalComponent* DecalComponent;

    //! Allow you to disable all the functionality related to Fog of war
    //! without removing it
    //! This only pause the timer so the fog of war is not updated anymore
    //! It will also set a dynamic material parameter so material can disable the post processing
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    bool bFoWEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    int FogVersion;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    bool bDebug;

    EDrawDebugTrace::Type DebugTrace() { 
        if (bDebug) 
            return EDrawDebugTrace::ForOneFrame;
        return EDrawDebugTrace::None;
    }

    /* If I could use a UTextureRenderTarget2DArray for everything
    * it would be great all the Fields of views would be available in one
    * texture instead of X textures
    * The main issue here is that UTextureRenderTarget2DArray do not have a Canvas version
    * to draw on them programmatically, it might be doable to implement one
    * the issue is I am not sure how much trouble it is going to be
    * is it worth the effort
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    class UTextureRenderTarget2DArray* FieldOfViews;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    class UTextureRenderTarget2DArray* Explorations2;
    */

    void SetFoWEnabledParameter(bool Enabled);

    void Tick(float DeltaTime);

    protected:
    void GetBrushSizes(FVector2D& TextureSize, FVector2D& MapSize);

    //! Updates the Texture Size, given the size covered by the Fog of War volume
    void UpdateVolumeSizes();

    //! Clear all the exploration textures
    void ClearExploration();

    //! Add current sight to exploration textures
    void UpdateExploration();

private:
    // Private because they do not lock the mutex
    // UpdateVolumeSizes does that for them
    void SetMapSize(FVector2D size);
    void SetTextureSize(FVector2D size);
    void DrawTriangles(UGKFogOfWarComponent *c);
    void GenerateTrianglesFromAngles(UGKFogOfWarComponent *c, TArray<float>& Angles);

private:
    void InitDecalRendering();
    void InitializeStrategy();

    TMap<FName, int>  NameToIndex;

    FCriticalSection Mutex;             // Mutex to sync adding/removing components with the fog compute
    FTimerHandle     FogComputeTimer;   // Compute the fog every few frames (or so)

    FVector2D        MapSize;           // from the Volume box
    FVector2D        TextureSize;       // == MapSize * TextureScale
   
    TArray<class UGKFogOfWarComponent*> ActorComponents;
    TMap<FName, class UMaterialInterface*> PostProcessMaterials;

    class UMaterialInstanceDynamic* DecalMaterialInstance;

    // Used by RayCastV2
    TArray<FCanvasUVTri> Triangles;

    UPROPERTY(VisibleAnywhere)
    class UGKFogOfWarStrategy* Strategy;

    friend class UGKFogOfWarStrategy;
    friend class UGKShadowCasting;
};
