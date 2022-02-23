// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#pragma once

#include "Gamekit.h"

#include "CoreMinimal.h"
#include "Runtime/Core/Public/HAL/ThreadingBase.h"
#include "GameFramework/Volume.h"
#include "Kismet/KismetSystemLibrary.h"

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
 *  TODO: add a upscale layer
 *  The actor will have a fog that follows him.
 */
UCLASS(Blueprintable)
class GAMEKIT_API AGKFogOfWarVolume : public AVolume
{
	GENERATED_BODY()

public:
    AGKFogOfWarVolume();

    void BeginPlay();

    //! Returns the vision texture
    UFUNCTION(BlueprintCallable, Category = FogOfWar)
    class UTexture *GetFactionTexture(FName name);

    UFUNCTION(BlueprintCallable, Category = FogOfWar)
    class UTexture *GetOriginalFactionTexture(FName name);

    //! Returns the exploration render target associated with the faction name
    UFUNCTION(BlueprintCallable, Category = FogOfWar, meta = (AutoCreateRefTerm = "CreateRenderTarget"))
    class UCanvasRenderTarget2D* GetFactionExplorationRenderTarget(FName name, bool CreateRenderTarget = true);

    //! Draw the fog of war for each factions
    void DrawFactionFog();

    //! Sets the texture parameters FoWView & FoWExploration
    UFUNCTION(BlueprintCallable, Category = FogOfWar, meta = (AutoCreateRefTerm = "CreateRenderTarget"))
    void SetFogOfWarMaterialParameters(FName name, class UMaterialInstanceDynamic* Material, bool CreateRenderTarget = false);

    //! Returns the post process material the actor should use for its camera
    UFUNCTION(BlueprintCallable, Category = FogOfWar, meta = (AutoCreateRefTerm = "CreateRenderTarget"))
    class UMaterialInterface* GetFogOfWarPostprocessMaterial(FName name, bool CreateRenderTarget = false);

    // Properties
    // ----------

    //! Collision Channel used to draw the line of sight
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|RayCast")
    TEnumAsByte<ECollisionChannel> FogOfWarCollisionChannel;

    //! If true exploration texture will be created
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    bool EnableExploration;

    //! Represents how often the fog is updated
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    float FramePerSeconds;

    //! Material used to draw unobstructed vision
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|RayCast")
    class UMaterialInterface* UnobstructedVisionMaterial;

    //! Material used to draw unobstructed vision with Triangles
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|RayCast")
    class UMaterialInterface *TrianglesMaterial;

    //! Margin for Actor Rays
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|RayCast|V3")
    float Margin;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|Discrete|V1")
    FGKGrid Grid;

    //! Base Material used to draw the fog of war in a post process step,
    //! it uses the texture parameters FoWView & FoWExploration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    class UMaterialInterface* BasePostProcessMaterial;

    //! Maps the faction to their fog texture
    //! This can be manually set or they will be automatically generated at runtime
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    TMap<FName, class UCanvasRenderTarget2D*> Explorations;

    //! Use Decal FoWDecal rendering
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    bool UseFoWDecalRendering;

    //! Faction that is displayed by the client
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    FName PreviewFaction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    class UDecalComponent* PreviewDecalComponent;

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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|Experimental")
    bool bUpscaling;

    EDrawDebugTrace::Type DebugTrace() { 
        if (bDebug) 
            return EDrawDebugTrace::ForOneFrame;
        return EDrawDebugTrace::None;
    }

    void Tick(float DeltaTime);

    void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
    void GetBrushSizes(FVector2D& TextureSize, FVector& MapSize);

    //! Updates the Texture Size, given the size covered by the Fog of War volume
    void UpdateVolumeSizes();

    //! Clear all the exploration textures
    void ClearExploration();

    //! Add current sight to exploration textures
    void UpdateExploration();

    //! Parameter collection used to talk to shader/materials
    //! The volume sets the following parameters
    //!     WithExploration
    //!     FoWEnabled
    //!     MapSize
    //!     TextureSize
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    class UMaterialParameterCollection* FogMaterialParameters;

    void SetMatrialParams();
    void SetMaterialParam_MapSize(FVector Size);
    void SetMaterialParam_TextureSize(FVector2D Size);
    void SetMaterialParam_FoWEnabled(int Enabled);
    void SetMaterialParam_Exploration(int Enabled);

        //! Returns the material parameter collection used to configure the Fog of War
    UFUNCTION(BlueprintCallable, Category = FogOfWar)
    class UMaterialParameterCollection* GetMaterialParameterCollection();

public:
    //! Used to controlled the size of the underlying texture given the terrain size
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|RayCast")
    float TextureScale;

    //! Size of the Volume box
    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = FogOfWar)
    FVector MapSize; 

    FVector GetMapSize() const { return MapSize; }

    void SetTextureSize(FVector2D Size) {
        TextureSize = Size;
        SetMaterialParam_TextureSize(Size);
    }

public:
    //! Actor Component that does the actual vision computation
    UPROPERTY(Transient)
    class UGKFogOfWarStrategy* Strategy;

    UPROPERTY(Transient)
    class UGKUpscalerStrategy *Upscaler;

    private:
    // Private because they do not lock the mutex
    // UpdateVolumeSizes does that for them
    void InitializeStrategy_Line();
    void InitializeStrategy_Triangle();
    void InitializeStrategy_Less();
    void InitializeStrategy_ShadowCasting();

private:
    void InitDecalRendering();
    void InitializeStrategy();

    TMap<FName, int>  NameToIndex;

    FCriticalSection Mutex;             // Mutex to sync adding/removing components with the fog compute
    FTimerHandle     FogComputeTimer;   // Compute the fog every few frames (or so)

    FVector2D        TextureSize;       // == MapSize * TextureScale

    TArray<class UGKFogOfWarComponent*> ActorComponents;

    //! Post Processing materials
    TMap<FName, class UMaterialInterface*> PostProcessMaterials;

    //! Decal Material used to draw in the editor
    class UMaterialInstanceDynamic* DecalMaterialInstance;

    // TODO: Remove this
    friend class UGKFogOfWarStrategy;
    friend class UGKShadowCasting;
    friend class UGKRayCasting_Line;
    friend class UGKRayCasting_Triangles;

 public:

    //! Register a new actor to the fog of war volume
    void RegisterActorComponent(class UGKFogOfWarComponent* c);

    //! Unregister the actor to the fog of war volume
    void UnregisterActorComponent(class UGKFogOfWarComponent* c);

    // Helpers
    // -----------------------------
   
    //! Returns the texture coordinate given world coordinates
    UFUNCTION(BlueprintCallable, Category = FogOfWar)
    inline FVector2D GetTextureCoordinate(FVector loc) {
        return FVector2D(
            (loc.X / MapSize.X + 0.5) * TextureSize.X,
            (0.5 - loc.Y / MapSize.Y) * TextureSize.Y
        );
    }

    FIntVector ToGridTexture(FIntVector Pos) {
        return FIntVector(
            TextureSize.Y / 2 - Pos.Y, 
            Pos.X + TextureSize.X / 2, 
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

public:
    // Editor Stuff
    // -------------
    void PostEditChangeProperty(struct FPropertyChangedEvent& e);

public:
    void TextureReady(FName Name);
};
