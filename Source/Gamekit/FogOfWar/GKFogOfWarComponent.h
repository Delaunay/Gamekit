#pragma once
// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

// Gamekit
#include "Gamekit/Utilities/GKBitFlag.h"


// Unreal Engine
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"

// Generated
#include "GKFogOfWarComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSightingEventSignature, AActor *, Actor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSightedEventSignature, AActor *, Actor);


/*! UGKFogOfWarComponent is used to defines the spec of the actor participating in the fog of war
 * the fog of war itself is computed inside the AGKFogOfWarVolume in a separate thread.
 *
 * \rst
 *
 * .. warning::
 *
 *    The components need to block vision to receive the OnSighted event because
 *    the same ray is used for vision and sighting.
 *
 * \endrst
 *
 */
UCLASS(Blueprintable, ClassGroup = (FogOfWar), meta = (BlueprintSpawnableComponent))
class GAMEKIT_API UGKFogOfWarComponent: public UActorComponent
{
    GENERATED_BODY()

    public:
    // Sets default values for this component's properties
    UGKFogOfWarComponent();

    protected:

    // Called when the game starts
    virtual void BeginPlay() override;

    // Unregister the component
    virtual void BeginDestroy() override;

    //! Attempts to register the component
    UFUNCTION(BlueprintCallable, Category = FogOfWar)
    bool RegisterComponent();

    public:
    // Replication
    // -----------

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

    bool bWasRegistered;

    //! Returns the render target associated with its faction, only use for debugging
    //! the texture cannot be changed to refelect team change
    UFUNCTION(BlueprintCallable, Category = "FogOfWar|Debug")
    class UTexture *GetVisionTexture();

    //! Returns the render target associated with its faction, only use for debugging
    //! the texture cannot be changed to refelect team change
    UFUNCTION(BlueprintCallable, Category = "FogOfWar|Debug")
    class UTexture *GetExplorationTexture();

    //! Returns the fog of war volume this component is associated to
    UFUNCTION(BlueprintCallable, Category = FogOfWar)
    class AGKFogOfWarVolume *GetFogOfWarVolume();

    UFUNCTION(BlueprintCallable, Category = FogOfWar)
    FName GetFaction();

    //! Returns the post process material the actor should use for its camera
    UFUNCTION(BlueprintCallable, Category = FogOfWar)
    class UMaterialInterface *GetFogOfWarPostprocessMaterial();

    UFUNCTION(BlueprintCallable, Category = FogOfWar)
    void SetCameraPostprocessMaterial(class UCameraComponent *CameraComponent);

    //! Sets the texture parameters FoWView & FoWExploration
    UFUNCTION(BlueprintCallable, Category = FogOfWar)
    void SetFogOfWarMaterialParameters(class UMaterialInstanceDynamic *Material);

    // Properties
    // ----------
    
    //! Line Tickness to draw sights, smaller tickness is more precise but requires more rays
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    float LineTickness;

    //! Number of rays to cast to draw the line of sight
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    int TraceCount;

    //! View max size
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    float Radius;

    //! Vision offset
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    float InnerRadius;

    /*! If true vision will be given around the the actor ignoring all collisions.
     * This method use a material to draw the vision and does not cast rays.
     * This means it is fairly cheap but it comes at the cost of not being able to
     * broadcast sighting events when the field of view is inferior to 360 degree.
     */
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    bool UnobstructedVision;

    /*! if false, the actor will not be reducing the fog arround him
     * No sighting event will be broadcasted
     */
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    bool GivesVision;

    /*! if true, other actors will not be able to see through this actor.
     * if false it will indirectly disable ``OnSighted`` broadcast events as a side effect.
     */
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    bool BlocksVision;

    /*! Field of view of the actor, 360 is all around like real time strategy,
     * Humans vision span is about 120 but most of it peripheral vision (i.e super bad),
     * central vision is about 60 degree */
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = FogOfWar)
    float FieldOfView;

    public:
    //! Called when the actor is seeing another actor in its line of sight
    //! Executed on the server only
    UPROPERTY(BlueprintAssignable, Category = FogOfWar)
    FSightingEventSignature OnSighting;

    //! Called when the actor is entering the line of sight of another actor
    //! Executed on the server only
    UPROPERTY(BlueprintAssignable, Category = FogOfWar)
    FSightedEventSignature OnSighted;

    FGenericTeamId GetGenericTeamId() const;

    //! Returns true if current team can see the target
    bool IsVisible(AActor const* Target) const;

    bool IsVisible(FVector Loc) const;

    private:
    // Keep track of materials this unit belongs to, so they can be 
    // updated in case of a faction change
    // in case of cheating the fog will simply stop working as it will draw the wrong fog
    // but the fog drawing is driven by server data which will be outdated
    // TArray<class UMaterialInstanceDynamic *> Materials;

    class AGKFogOfWarVolume *FogOfWarVolume;

    void SetCollisionFoWResponse(class UPrimitiveComponent *Primitive, ECollisionChannel Channel);

    //! Try to get the faction using the IGenericTeamAgentInterface
    FName DeduceFaction() const;

    //! Faction this unit belongs to
    FName Faction;

    friend class AGKFogOfWarVolume;
};
