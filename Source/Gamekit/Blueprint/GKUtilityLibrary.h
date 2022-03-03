// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Abilities/GKAbilityStatic.h"

#include "GKUtilityLibrary.generated.h"

/**
 * 
 */
UCLASS()
class GAMEKIT_API UGKUtilityLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    
public:
    //! This cannot be exposed to blueprint because fo the const
    //! It is fine though we should create accessor lile the GetMapSize
    static class AWorldSettings const* GetWorldSetting(const UObject *World);

    //! Return the map size stored inside WorldSettings
    UFUNCTION(BlueprintPure, Category = "Level|Size", meta = (WorldContext = "World"))
    static FVector2D GetWorldMapSize(const UObject *World);

    // Returns the project version set in the 'Project Settings' > 'Description' section
    // of the editor
    UFUNCTION(BlueprintPure, Category = "Project")
    static FString GetProjectVersion();

    //! Return the map size stored inside the Fog of War Volume if any
    UFUNCTION(BlueprintPure, Category = "Level|Size", meta = (WorldContext = "World"))
    static FVector GetFogOfWarMapSize(const UObject *World);

    //! Given a player controller it will output the 4 corners seen by the player
    //! projected on the ground
    UFUNCTION(BlueprintCallable, Category = "Controller", meta = (WorldContext = "World"))
    static void GetControllerFieldOfView(const UObject *           World,
                                         class APlayerController * Controller,
                                         ETraceTypeQuery           TraceChannel,
                                         TArray<FVector>         & Corners, 
                                         FVector2D                 Margin = FVector2D(0, 0));

    //! Draw a polygon given its corners
    //! It will close the polygon at the end
    UFUNCTION(BlueprintCallable, Category = "Controller", meta = (WorldContext = "World"))
    static void DrawPolygon(const UObject *              World,
                            class UCanvasRenderTarget2D *Target,
                            TArray<FVector>              Corners,
                            FVector2D                    MapSize,
                            FLinearColor                 Color,
                            float                        Thickness);


    UFUNCTION(BlueprintPure, Category = "Rotation")
    static FRotator BetterLookAtRotation(FVector ActorLocation,
                                         FVector LookAt,
                                         FVector UpDirection = FVector(0, 0, 1.f));


    UFUNCTION(BlueprintPure, Category = "Widget")
    static class UWidget *GetPanelSlotContent(class UPanelSlot* Slot);

    // UFUNCTION(BlueprintPure, Category = "Widget")
    // Don't use
    static class UWidget *GetWidgetUnderCursor();

    UFUNCTION(BlueprintPure, Category = "ItemSlot")
    static EGK_ItemSlot ItemSlotFromInteger(int SlotId);

    static float GetYaw(FVector Origin, FVector Target);

    //! Returns the two points of an actor that are visible from Location
    UFUNCTION(BlueprintPure, Category = "Bounds")
    static void GetVisibleBounds(FVector Location, AActor* Actor, FVector& OutMin, FVector& OutMax) ;

    UFUNCTION(BlueprintCallable, Category = "Rendering")
    static void ClearTexture(class UTexture *Texture, FLinearColor ClearColor);

    UFUNCTION(BlueprintPure, Category = "Multiplayer", meta = (WorldContext = "World"))
    static FName GameInstanceMode(const UObject *World);

    UFUNCTION(BlueprintPure, Category = "Multiplayer")
    static FString GetNetConfig(const AActor *Actor);
};
