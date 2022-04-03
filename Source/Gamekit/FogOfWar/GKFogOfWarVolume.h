// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/FogOfWar/Strategy/GK_FoW_Strategy.h"
#include "Gamekit/FogOfWar/Upscaler/GK_Upscaler_Strategy.h"
#include "Gamekit/Gamekit.h"
#include "Gamekit/Grid/GKGrid.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "GameFramework/Volume.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Runtime/Core/Public/HAL/ThreadingBase.h"

// Generated
#include "GKFogOfWarVolume.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGKNewFactionDelegate, FName, Name);

#define DEFAULT_FoW_COLLISION ECC_GameTraceChannel1

struct FGKFactionFog
{
    FGKFactionFog()
    {
        Buffer         = nullptr;
        Exploration    = nullptr;
        Vision         = nullptr;
        PreviousFrameVision = nullptr;
        UpScaledVision = nullptr;
        VisibleEnemies.Reserve(128);
        Allies.Reserve(128);
    }

    FName           Name;
    class UTexture *Exploration;
    class UTexture *Vision;
    class UTexture *PreviousFrameVision;
    class UTexture *UpScaledVision;
    bool            bDiscrete;

    TSet<class UGKFogOfWarComponent *>   VisibleEnemies;
    TArray<class UGKFogOfWarComponent *> Allies;

    void *Buffer;
};

/*! AGKFogOfWarVolume manages fog of war for multiple factions.
 * All units inside the same faction share visions.
 *
 * \rst
 *
 * .. note::
 *
 *    The fog of war will darken your screen, it is advised to disable UE4 default auto exposure using a Post Process
 *    Volume and enabling ``Exposure > Metering Mode > Auto Exposure Basic`` as well as setting min and max brightness
 * to 1
 *
 * \endrst
 *
 *  TODO: create a radar version that follows the actor. The actor will have a fog that follows him.
 */
UCLASS(Blueprintable)
class GAMEKIT_API AGKFogOfWarVolume: public AVolume
{
    GENERATED_BODY()

    public:
    AGKFogOfWarVolume();

    void BeginPlay();

    //! Returns the vision texture
    UFUNCTION(BlueprintCallable, Category = FogOfWar)
    class UTexture *GetFactionTexture(FName name);

    //! Returns the vision texture before upscaling
    UFUNCTION(BlueprintCallable, Category = FogOfWar)
    class UTexture *GetOriginalFactionTexture(FName name);

    //! Returns the exploration render target associated with the faction name
    UFUNCTION(BlueprintCallable, Category = FogOfWar)
    class UTexture *GetFactionExplorationTexture(FName name);

    //! Sets the texture parameters FoWView & FoWExploration
    UFUNCTION(BlueprintCallable, Category = FogOfWar)
    void SetFogOfWarMaterialParameters(FName name, class UMaterialInstanceDynamic *Material);

    //! Returns the post process material the actor should use for its camera
    UFUNCTION(BlueprintCallable, Category = FogOfWar)
    class UMaterialInterface *GetFogOfWarPostprocessMaterial(FName name);

    UFUNCTION(BlueprintCallable, Category = FogOfWar)
    TArray<class UGKFogOfWarComponent *> const &GetBlocking() const { return Blocking; }

    // Properties
    // ----------

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|TickLimit")
    float LimitFramePerSeconds;
    float DeltaAccumulator;

    //! Represents how often the fog is updated when AsyncDraw is true
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|Async")
    float FramePerSeconds;

    //! Enable Async drawing
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|Async")
    bool bAsyncDraw;

    //! class used to draw the vision
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar")
    TSubclassOf<UGKFogOfWarStrategy> VisionDrawingStrategy;

    //! Used to control the size of the underlying texture given the terrain size
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|RayCast")
    float TextureScale;

    //! Collision Channel used to draw the line of sight
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|RayCast")
    TEnumAsByte<ECollisionChannel> FogOfWarCollisionChannel;

    //! Material used to draw unobstructed vision
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|RayCast")
    class UMaterialInterface *UnobstructedVisionMaterial;

    //! Material used to draw unobstructed vision with Triangles
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|RayCast")
    class UMaterialInterface *TrianglesMaterial;

    //! Margin for Actor Rays
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|RayCast")
    float Margin;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|Discrete")
    FGKGrid Grid;

    //! Class used to perform the upscaling
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|Upscaling")
    TSubclassOf<UGKTransformerStrategy> UpscalerClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|Upscaling")
    bool bUpscaling;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|Upscaling")
    class UMaterialInterface *UpscaleMaterial;

    //! It is the recommended way to set the upscale size
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|Upscaling")
    FIntVector FixedSize;

    //! Upscale multipler, be careful not go beyond GPU limits
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|Upscaling")
    int Multiplier;

    //! Use fixed size for the upscaling
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|Upscaling")
    bool bFixedSize;

    //! class used to generate the exploration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|Exploration")
    TSubclassOf<UGKTransformerStrategy> ExplorationClass;

    //! If true exploration texture will be created
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|Exploration")
    bool bExploration;

    //! If true exploration will use the upscaled vision texture
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|Exploration")
    bool bUseUpscaledVision;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|Editor")
    bool bDebug;

    //! Use Decal FoWDecal rendering
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|Editor")
    bool UseFoWDecalRendering;

    //! Faction that is displayed by the client
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|Editor")
    FName PreviewFaction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|Editor")
    class UDecalComponent *PreviewDecalComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|Editor")
    TMap<FName, class UCanvasRenderTarget2D *> DebugDumpVision;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|Editor")
    TMap<FName, class UCanvasRenderTarget2D *> DebugDumpVisionUpscaled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar|Editor")
    TMap<FName, class UCanvasRenderTarget2D *> DebugDumpExploration;

    //! Parameter collection used to talk to shader/materials
    //! The volume sets the following parameters
    //!     WithExploration
    //!     FoWEnabled
    //!     MapSize
    //!     TextureSize
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar")
    class UMaterialParameterCollection *FogMaterialParameters;

    //! Base Material used to draw the fog of war in a post process step,
    //! it uses the texture parameters FoWView & FoWExploration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FogOfWar")
    class UMaterialInterface *BasePostProcessMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    TArray<AActor *> ActorsToIgnore;

    //! Allow you to disable all the functionality related to Fog of war
    //! without removing it
    //! This only pause the timer so the fog of war is not updated anymore
    //! It will also set a dynamic material parameter so material can disable the post processing
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    bool bFogOfWarEnabled;

    EDrawDebugTrace::Type DebugTrace()
    {
        if (bDebug)
            return EDrawDebugTrace::ForOneFrame;
        return EDrawDebugTrace::None;
    }

    void Tick(float DeltaTime);

    void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    protected:
    //! Draw the fog of war for each factions
    void DrawFactionFog();

    void GetBrushSizes(FVector2D &TextureSize, FVector &MapSize);

    //! Updates the Texture Size, given the size covered by the Fog of War volume
    void UpdateVolumeSizes();

    void SetMatrialParams();
    void SetMaterialParam_MapSize(FVector Size);
    void SetMaterialParam_TextureSize(FVector2D Size);
    void SetMaterialParam_FoWEnabled(int Enabled);
    void SetMaterialParam_Exploration(int Enabled);

    //! Returns the material parameter collection used to configure the Fog of War
    UFUNCTION(BlueprintCallable, Category = FogOfWar)
    class UMaterialParameterCollection *GetMaterialParameterCollection();

    void DumpToDebugRenderTarget();

    public:
    //! Size of the Volume box
    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = FogOfWar)
    FVector MapSize;

    FVector GetMapSize() const { return MapSize; }

    void SetTextureSize(FVector2D Size)
    {
        TextureSize = Size;
        SetMaterialParam_TextureSize(Size);
    }

    public:
    // Helpers
    // -----------------------------

    //! Returns the texture coordinate given world coordinates
    UFUNCTION(BlueprintCallable, Category = FogOfWar)
    inline FVector2D GetTextureCoordinate(FVector loc)
    {
        return FVector2D((loc.X / MapSize.X + 0.5) * TextureSize.X, (0.5 - loc.Y / MapSize.Y) * TextureSize.Y);
    }

    FIntVector ToGridTexture(FIntVector Pos)
    {
        return FIntVector(TextureSize.Y / 2 - Pos.Y, Pos.X + TextureSize.X / 2, Pos.Z);
    }

    FIntVector FromTextureToGrid(FIntVector Pos)
    {
        return FIntVector(Pos.X - TextureSize.X / 2, TextureSize.Y / 2 - Pos.Y, Pos.Z);
    }

    private:
    void InitDecalRendering();

    FCriticalSection Mutex;           // Mutex to sync adding/removing components with the fog compute
    FTimerHandle     FogComputeTimer; // Compute the fog every few frames (or so)

    FVector2D TextureSize; // == MapSize * TextureScale

    //! Decal Material used to draw in the editor
    class UMaterialInstanceDynamic *DecalMaterialInstance;

    public:
    // Editor Stuff
    // -------------
    void PostEditChangeProperty(struct FPropertyChangedEvent &e);

    public:
    // TODO: Remove this
    friend class UGKFogOfWarStrategy;
    friend class UGKShadowCasting;
    friend class UGKRayCasting_Line;
    friend class UGKRayCasting_Triangles;

    private:
    //! Register a new actor to the fog of war volume
    void RegisterActorComponent(class UGKFogOfWarComponent *c);

    //! Unregister the actor to the fog of war volume
    void UnregisterActorComponent(class UGKFogOfWarComponent *c);

    friend class UGKFogOfWarComponent;

    FGKFactionFog &GetFactionFogs(FName Faction);

    TMap<FName, FGKFactionFog> FactionFogs;

    TArray<class UGKFogOfWarComponent *> Blocking;

    protected:
    void InitializeStrategy();
    void InitializeUpscaler();
    void InitializeExploration();

    //! Actor Component that does the actual vision computation
    UPROPERTY(Transient)
    class UGKFogOfWarStrategy *Strategy;

    UPROPERTY(Transient)
    class UGKTransformerStrategy *Upscaler;

    UPROPERTY(Transient)
    class UGKTransformerStrategy *Exploration;

    UPROPERTY(Transient)
    TArray<class UGKFogOfWarComponent *> ActorComponents;

    //! Post Processing materials
    UPROPERTY(Transient)
    TMap<FName, class UMaterialInterface *> PostProcessMaterials;

    bool bReady;
};
